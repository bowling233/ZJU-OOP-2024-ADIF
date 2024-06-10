#include "adif.hpp"

#include <algorithm>
#include <ios>
#include <istream>
#include <fstream>
#include <limits>
#include <string>
#include <map>

namespace adif
{
  std::string getUtf8CJK(std::istream &is, int len)
  {
    std::string value;
    while (len > 0)
    {
      unsigned char byte = is.get();

      if ((byte & 0x80) == 0x00) // ASCII: U+0000 to U+007F
      {
        value += byte;
        len--;
      }
      else if ((byte & 0xF0) == 0xE0) // CJK: U+0800 to U+FFFF (not accurate)
      {                               // CJK is 3 bytes, but required to see as 2 length
        value += byte;
        value += is.get();
        value += is.get();
        len -= 2;
      }
      else if (byte != '\n') // ignore newline
      {
        std::cerr << "[Error] Invalid character: neither ASCII nor CJK " + std::to_string(byte) + ". Skipped." << std::endl;
      }
    }
    if (len < 0)
      std::cerr << "[Warning] field length not match" << std::endl;
    return value;
  }

  struct Field getField(std::istream &is)
  {
    // ignore characters until start of field
    is.ignore(std::numeric_limits<std::streamsize>::max(), '<');
    if (!is)
      return {};

    std::string field;
    std::getline(is, field, '>');
    if (field.empty())
      return {};
    std::transform(field.begin(), field.end(), field.begin(), ::toupper);
    if (field == "EOR" || field == "EOH")
      return {field, 0, ""};

    // parse field name and length
    size_t pos = field.find(':');
    if (pos == std::string::npos)
      throw std::runtime_error("Invalid field format: " + field);
    unsigned len = std::stoul(field.substr(pos + 1));
    field = field.substr(0, pos);

    // read field value
    std::string value = getUtf8CJK(is, len);
    return {field, len, value};
  }

  Record getRecord(std::istream &is)
  {
    std::map<std::string, std::pair<unsigned, std::string>> record;
    while (true)
    {
      auto field = getField(is);
      if (field.name.empty()) // discard invalid fields
        return {};
      if (field.name == "EOR") // record terminator
        break;
      if (field.name == "EOH") // header terminator
        continue;
      record[field.name] = {field.length, field.value};
    }
    // check if primary key (QSO_DATE, TIME_ON) exists
    if (record.find("QSO_DATE") == record.end() || record.find("TIME_ON") == record.end())
    {
      std::cerr << "[Warning] Primary key not found in record, discard." << std::endl;
      std::cerr << "Errored Record: " << std::endl
                << record;
      return {};
    }
    return record;
  }

  Document::Document(const std::string &filename) : filename(filename)
  {
    this->Open(filename);
  }

  void Document::Clean()
  {
    filename.clear();
    records.clear();
    field_names.clear();
  }

  void Document::Open(const std::string &filename)
  {
    this->Clean();
    this->filename = filename;
    std::ifstream file(filename);
    if (!file)
    {
      std::cerr << "[Error] Failed to open file: " + filename << std::endl;
      return;
    }

    while (true)
    {
      auto record = getRecord(file);
      if (record.empty())
        break;
      records.push_back(record);
      for (const auto &field : record)
        field_names.insert(field.first);
    }

    file.close();
    if (records.empty())
      std::cerr << "[Warning] No record found in file: " + filename << std::endl;
  }

  Document::Document(const rapidcsv::Document &doc)
  {
    for (unsigned int row = 0; row < doc.GetRowCount(); row++)
    {
      std::map<std::string, std::pair<unsigned, std::string>> record;
      for (unsigned int col = 0; col < doc.GetColumnCount(); col++)
      {
        std::string field = doc.GetColumnName(col);
        std::string value = doc.GetCell<std::string>(col, row);
        record[field] = {value.length(), value};
        field_names.insert(field);
      }
      records.push_back(record);
    }
  }

  rapidcsv::Document Document::GetCSV() const
  {
    rapidcsv::Document doc;
    unsigned int col = 0;
    for (const auto &field : field_names)
    {
      std::vector<std::string> col_data;
      for (const auto &record : records)
        if (record.find(field) != record.end())
          col_data.push_back(record.at(field).second);
        else
          col_data.push_back("");
      doc.InsertColumn<std::string>(col, col_data, field);
      col++;
    }
    return doc;
  }

  void Document::Save(const std::string &filename) const
  {
    std::ofstream file(filename);
    if (!file)
      throw std::runtime_error("Failed to open file: " + filename);

    file << *this;

    file.close();
  }

  void Document::Merge(const Document &doc)
  {
    filename += " + " + doc.filename;
    for (const auto &record : doc.records)
    {
      records.push_back(record);
      for (const auto &field : record)
        field_names.insert(field.first);
    }
  }

  Document::Conflict Document::DetectConflicts(const Document &doc) const
  {
    // (QSO_DATE, TIME_ON) is primary key, check for duplicates
    Conflict conflicts;
    for (int i = 0; i < records.size(); i++)
    {
      auto qso_date = records[i].find("QSO_DATE")->second.second;
      auto time_on = records[i].find("TIME_ON")->second.second;
      // if (qso_date == record.end() || time_on == record.end())
      //   throw std::runtime_error("Primary key not found in record");
      std::vector<int> indexes = doc.Search({{"QSO_DATE", qso_date}, {"TIME_ON", time_on}});
      if (!indexes.empty())
      {
        conflicts.first.push_back(i);
        conflicts.second.insert(conflicts.second.end(), indexes.begin(), indexes.end());
      }
    }
    return conflicts;
  }

  Record Document::operator[](int index) const
  {
    if (index < 0 || index >= records.size())
      throw std::out_of_range("Index out of range");
    return records[index];
  }

  std::vector<int> Document::Search(const Fields &condition) const
  {
    std::vector<int> indexes;
    for (int i = 0; i < records.size(); i++)
    {
      bool match = true;
      for (const auto &cond : condition)
      {
        auto field = records[i].find(cond.first);
        if (field == records[i].end() || field->second.second != cond.second)
        {
          match = false;
          break;
        }
      }
      if (match)
        indexes.push_back(i);
    }
    return indexes;
  }

  unsigned calculateLength(const std::string &value)
  {
    unsigned len = 0;
    for (int i = 0; i < value.length();)
    {
      if ((value[i] & 0x80) == 0x00) // ASCII: U+0000 to U+007F
      {
        len++;
        i++;
      }
      else if ((value[i] & 0xF0) == 0xE0) // CJK: U+0800 to U+FFFF (not accurate)
      {                                   // CJK is 3 bytes, but required to see as 2 length
        len += 2;
        i += 3;
      }
      else
      {
        throw std::runtime_error("Invalid character: neither ASCII nor CJK" + std::to_string(value[i]));
      }
    }
    return len;
  }

  void Document::Update(int index, const Fields &fields)
  {
    if (index < 0 || index >= records.size())
      throw std::out_of_range("Index out of range");

    for (const auto &field : fields)
    {
      records[index][field.first] = {calculateLength(field.second), field.second};
      field_names.insert(field.first);
    }
  }

  void Document::Delete(std::vector<int> indexes)
  {
    std::sort(indexes.begin(), indexes.end(), std::greater<int>());
    for (const auto &index : indexes)
    {
      if (index < 0 || index >= records.size())
        throw std::out_of_range("Index out of range");
      records.erase(records.begin() + index);
    }
  }

  std::vector<std::vector<std::string>> Document::GetTable() const
  {
    std::vector<std::vector<std::string>> table;
    std::vector<std::string> header;
    for (const auto &field : field_names)
      header.push_back(field);
    table.push_back(header);
    for (const auto &record : records)
    {
      std::vector<std::string> row;
      for (const auto &field : field_names)
      {
        auto value = record.find(field);
        if (value != record.end())
          row.push_back(value->second.second);
        else
          row.push_back("");
      }
      table.push_back(row);
    }
    std::cerr << "[Warning] Table format not implemented" << std::endl;
    return table;
  }

  std::ostream &operator<<(std::ostream &os, const adif::Document &doc)
  {
    os << "File: " << doc.filename << std::endl
       << "Records: " << doc.records.size() << std::endl
       << "<EOH>" << std::endl;
    for (const auto &record : doc.records)
      os << record;
    return os;
  }

  std::ostream &operator<<(std::ostream &os, const adif::Record &record)
  {
    for (const auto &field : record)
      os << "<" << field.first << ":" << field.second.first << ">" << field.second.second;
    os << "<EOR>" << std::endl;
    return os;
  }

} // namespace adif

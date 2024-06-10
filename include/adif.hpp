#pragma once

#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include "rapidcsv.h"

namespace adif
{
  struct Field
  {
    std::string name;
    unsigned length;
    std::string value;
  };
  /**
   * @brief Utility function to extract field and value from ADIF record
   *
   * @param is input stream
   * @return Field field, empty field if not found or invalid field
   */
  struct Field getField(std::istream &is);

  using Record = std::map<std::string, std::pair<unsigned, std::string>>;
  /**
   * @brief Utility function to extract record from ADIF file
   *
   * @param is input stream
   * @return std::map<std::string, std::pair<unsigned, std::string>> record, empty map if not
   * found or invalid record, representing end of adif file
   */
  Record getRecord(std::istream &is);

  class Document
  {
  public:
    Document() = default;
    Document(const std::string &filename);
    Document(const rapidcsv::Document &doc);
    // void Display(std::ostream &os = std::cout);
    void Open(const std::string &filename);
    void Clean();
    rapidcsv::Document GetCSV() const;
    void Save(const std::string &filename) const;
    void Merge(const Document &doc);
    using Conflict = std::pair<std::vector<int>, std::vector<int>>;
    Conflict DetectConflicts(const Document &doc) const;
    friend std::ostream &operator<<(std::ostream &os, const Document &doc);
    Record operator[](int index) const;
    using Fields = std::vector<std::pair<std::string, std::string>>;
    std::vector<int> Search(const Fields &condition) const;
    void Update(int index, const Fields &fields);
    void Delete(std::vector<int>);
    std::vector<std::vector<std::string>> GetTable() const;

  private:
    std::string filename;
    std::vector<Record> records;
    std::set<std::string> field_names;
  };

  std::ostream &operator<<(std::ostream &os, const adif::Document &doc);
  std::ostream &operator<<(std::ostream &os, const adif::Record &record);

} // namespace adif

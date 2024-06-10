#include <iostream>
#include <functional>
#include <memory>
#include <string>

#include <vector>
#include "rapidcsv.h"
#include "adif.hpp"

std::vector<std::string> getTokens(const std::string &command)
{
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(command);
  while (std::getline(tokenStream, token, ' '))
  {
    tokens.push_back(token);
  }
  return tokens;
}

adif::Document::Fields getFields(const std::vector<std::string>::iterator &begin, const std::vector<std::string>::iterator &end)
{
  adif::Document::Fields fields;
  for (auto it = begin; it != end; it += 2)
  {
    std::transform(it->begin(), it->end(), it->begin(), ::toupper);
    fields.push_back({*it, *(it + 1)});
  }
  return fields;
}

bool checkTokens(const std::vector<std::string> &tokens, int n)
{
  if (tokens.size() == n)
  {
    return true;
  }
  else
  {
    std::cout << "Invalid number of arguments.\n";
    return false;
  }
}

int main(void)
{

  adif::Document adifdoc;

  std::cout << "> ";
  std::string command;
  std::getline(std::cin, command);
  std::vector<std::string> tokens = getTokens(command);

  while (tokens[0] != "exit")
  {
    // debug
    for (const auto &token : tokens)
    {
      std::cout << token << " ";
    }
    std::cout << std::endl;

    if (tokens.size() == 0 || tokens[0] == "help")
    {
      std::cout << "Usage:\n"
                << "  help: Display this help message.\n"
                << "  read <file>: Read an ADIF file. If there is already data in memory, it will be cleared.\n"
                << "  display [index1 index2 ...]: Display records.\n"
                << "  search <field> <value> [field value]...: Search records by field. Return indexes of all matched records.\n"
                << "  update <index> <field> <value> [field value]...: Update records by field.\n"
                << "  delete <index>: Delete records by field.\n"
                << "  merge <file>: Merge another ADIF file into the current data.\n"
                << "  save <file>: Save data to a new ADIF file.\n"
                << "  export <file>: Export data to a CSV file.\n"
                << "  quit: Exit the program.\n";
    }
    else if (tokens[0] == "read" && checkTokens(tokens, 2))
    {
      adifdoc.Open(tokens[1]);
    }
    else if (tokens[0] == "display")
    {
      if (tokens.size() == 1)
      {
        std::cout << adifdoc;
      }
      else
      {
        for (int i = 1; i < tokens.size(); i++)
        {
          int index = std::stoi(tokens[i]);
          using adif::operator<<;
          std::cout << adifdoc[index];
        }
      }
    }
    else if (tokens[0] == "search")
    {
      // argument check
      if (tokens.size() <= 1 || tokens.size() % 2 != 1)
      {
        std::cout << "Invalid number of arguments.\n";
        std::cout << "Usage: search <field> <value> [field value]...\n";
        continue;
      }
      adif::Document::Fields condition = getFields(tokens.begin() + 1, tokens.end());
      std::vector<int> indexes = adifdoc.Search(condition);
      for (const auto &index : indexes)
      {
        using adif::operator<<;
        std::cout << adifdoc[index];
      }
    }
    else if (tokens[0] == "update")
    {
      int index = std::stoi(tokens[1]);
      adif::Document::Fields fields = getFields(tokens.begin() + 2, tokens.end());
      adifdoc.Update(index, fields);
    }
    else if (tokens[0] == "delete")
    {
      if (tokens.size() == 1)
      {
        std::cout << "Invalid number of arguments.\n";
      }
      else
      {
        std::vector<int> indexes;
        for (int i = 1; i < tokens.size(); i++)
        {
          indexes.push_back(std::stoi(tokens[i]));
        }
        adifdoc.Delete(indexes);
      }
    }
    else if (tokens[0] == "merge" && checkTokens(tokens, 2))
    {
      adif::Document doc(tokens[1]);
      while (adifdoc.DetectConflicts(doc).first.size() > 0)
      {
        std::cout << "[Warning] Conflicts detected. Please resolve them first.\n";
        std::cout << "Conflicts: \n"
                  << "Index\tRecord\n";
        std::cout << "--In memory--\n";
        for (const auto &index : adifdoc.DetectConflicts(doc).first)
        {
          using adif::operator<<;
          std::cout << index << "\t" << adifdoc[index];
        }
        std::cout << "--To merge--\n";
        for (const auto &index : adifdoc.DetectConflicts(doc).second)
        {
          using adif::operator<<;
          std::cout << index << "\t" << doc[index];
        }
        std::cout << "Please enter the indexes of the records to delete: \n"
                  << "'o' represent the records in memory, 'n' represent the records to merge.\n"
                  << "example: o1 n2 n3\n"
                  << "(merge)> ";
        std::string indexes;
        std::getline(std::cin, indexes);
        std::vector<std::string> index_tokens = getTokens(indexes);
        for (const auto &index_token : index_tokens)
        {
          if (index_token[0] == 'o')
          {
            adifdoc.Delete({std::stoi(index_token.substr(1))});
          }
          else if (index_token[0] == 'n')
          {
            doc.Delete({std::stoi(index_token.substr(1))});
          }
        }
      }
      std::cout << "Merging...\n";
      adifdoc.Merge(doc);
    }
    else if (tokens[0] == "save" && checkTokens(tokens, 2))
    {
      adifdoc.Save(tokens[1]);
    }
    else if (tokens[0] == "export" && checkTokens(tokens, 2))
    {
      rapidcsv::Document csvdoc = adifdoc.GetCSV();
      csvdoc.Save(tokens[1]);
    }
    else if (tokens[0] == "quit")
    {
      break;
    }
    else if (tokens[0][0] != '#')
    {
      std::cout << "Invalid command. Type 'help' for usage.\n";
    }

    std::cout << "> ";
    std::getline(std::cin, command);
    tokens = getTokens(command);
  }

  return EXIT_SUCCESS;
}

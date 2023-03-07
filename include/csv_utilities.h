// *****************************************************
//    Copyright 2023 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef csv_utilities_h
#define csv_utilities_h

#include <string_view>
#include <vector>

#include <csvutil_export.h>

namespace csv
{
class CSVUTIL_EXPORT CSVFormat
{
private:
  enum class VariableColumnPolicy { THROW = -1, IGNORE_ROW = 0, KEEP = 1 };
  void assert_no_char_overlap();

  std::vector<char> trim_chars = {};

  bool no_quote = false;
  char quote_char = '"';
  std::vector<std::string> col_names = {};
  VariableColumnPolicy variable_column_policy = VariableColumnPolicy::IGNORE_ROW;
  CSVFormat& CSVFormat::quote(char quote);
  CSVFormat& CSVFormat::header_row(int row);

public:
  int header = 0;
  std::vector<char> possible_delimiters = {','};

  CSVFormat() = default;
  CSVFormat& CSVFormat::delimiter(const std::vector<char>& delim);
  static CSVFormat guess_csv();
  bool guess_delim();
};
class CSVUTIL_EXPORT CSVReader
{
private:
  CSVFormat _format;

public:
  CSVReader(std::string_view filename, CSVFormat format = CSVFormat::guess_csv()) {}
};
} // namespace csv
#endif // csv_utilities_h

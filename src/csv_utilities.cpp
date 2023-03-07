// *****************************************************
//    Copyright 2023 Videonetics Technology Pvt Ltd
// *****************************************************

#include "csv_utilities.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <set>
#include <stdexcept>

void csv::CSVFormat::assert_no_char_overlap()
{
  auto delims = std::set<char>(this->possible_delimiters.begin(), this->possible_delimiters.end()),
       trims = std::set<char>(this->trim_chars.begin(), this->trim_chars.end());

  // Stores intersection of possible delimiters and trim characters
  std::vector<char> intersection = {};

  // Find which characters overlap, if any
  std::set_intersection(delims.begin(), delims.end(), trims.begin(), trims.end(), std::back_inserter(intersection));

  // Make sure quote character is not contained in possible delimiters
  // or whitespace characters
  if (delims.find(this->quote_char) != delims.end() || trims.find(this->quote_char) != trims.end()) {
    intersection.push_back(this->quote_char);
  }

  if (!intersection.empty()) {
    std::string err_msg = "There should be no overlap between the quote character, "
                          "the set of possible delimiters "
                          "and the set of whitespace characters. Offending characters: ";

    // Create a pretty error message with the list of overlapping
    // characters
    for (size_t i = 0; i < intersection.size(); i++) {
      err_msg += "'";
      err_msg += intersection[i];
      err_msg += "'";

      if (i + 1 < intersection.size())
        err_msg += ", ";
    }

    throw std::runtime_error(err_msg + '.');
  }
}

csv::CSVFormat& csv::CSVFormat::quote(char quote)
{
  this->no_quote = false;
  this->quote_char = quote;
  this->assert_no_char_overlap();
  return *this;
}

csv::CSVFormat& csv::CSVFormat::header_row(int row)
{
  if (row < 0)
    this->variable_column_policy = VariableColumnPolicy::KEEP;

  this->header = row;
  this->col_names = {};
  return *this;
}

csv::CSVFormat& csv::CSVFormat::delimiter(const std::vector<char>& delim)
{
  this->possible_delimiters = delim;
  this->assert_no_char_overlap();
  return *this;
}

csv::CSVFormat csv::CSVFormat::guess_csv()
{
  csv::CSVFormat format;
  format.delimiter({',', '|', '\t', ';', '^'}).quote('"').header_row(0);

  return format;
}

bool csv::CSVFormat::guess_delim() { return this->possible_delimiters.size() > 1; }

csv::CSVReader::CSVReader(std::string_view filename, CSVFormat format) : _format(format)
{
  auto head = internals::get_csv_head(filename);
  using Parser = internals::MmapParser;

  /** Guess delimiter and header row */
  if (format.guess_delim()) {
    auto guess_result = internals::_guess_format(head, format.possible_delimiters);
    format.delimiter(guess_result.delim);
    format.header = guess_result.header_row;
    this->_format = format;
  }

  if (!format.col_names.empty())
    this->set_col_names(format.col_names);

  this->parser = std::unique_ptr<Parser>(new Parser(filename, format, this->col_names)); // For C++11
  this->initial_read();
}
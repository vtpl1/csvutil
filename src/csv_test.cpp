#include <thread>

#include "csv.hpp"

int main(int /*argc*/, char const* /*argv*/[])
{
  csv::CSVReader reader("v_channel.csv");

  for (csv::CSVRow& row : reader) { // Input iterator
    std::cout << row["major_url"].get<std::string>() << " , " << row["username"].get<std::string>() << " : "
              << row["password"].get<std::string>() << std::endl;
  }
  return 0;
}

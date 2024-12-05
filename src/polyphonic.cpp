#include "GPTSovits/polyphonic.h"
#include "GPTSovits/Utils/exception.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <filesystem>

namespace GPTSovits {

std::string polyphonic_data;
std::unordered_map<std::string_view, std::vector<std::string_view>> pp_dict;

const std::unordered_map<std::string_view, std::vector<std::string_view>> &
get_polyphonic_map() {
  if (!pp_dict.empty()) {
    return pp_dict;
  }
  if (polyphonic_data.empty()) {
    auto path = std::filesystem::current_path() / "res" / "polyphonic.data";
    #ifdef _HOST_WINDOWS_
    std::ifstream file(path.wstring());
    #else
    std::ifstream file(path);
    #endif
    if (!file.is_open()) {
      THROW_ERRORN("字典文件不存在!\nFrom:{}", path.string());
    }
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    polyphonic_data.reserve(fileSize);
    const std::size_t bufferSize = 8192;
    char buffer[bufferSize];
    while (file.read(buffer, bufferSize) || file.gcount() > 0) {
      polyphonic_data.append(buffer, file.gcount());
    }
  }
  std::string_view data_view(polyphonic_data);
  // Split data_view into lines using '\n'
  using boost::algorithm::split_iterator;
  using boost::algorithm::first_finder;
  auto it = split_iterator<std::string_view::const_iterator>(
    data_view.begin(), data_view.end(), first_finder("\n", boost::algorithm::is_equal()));

  while (it != split_iterator<std::string_view::const_iterator>()) {
    std::string_view line(it->begin(), it->end());
    ++it;

    // Split each line using '|'
    auto pipe_pos = line.find('|');
    if (pipe_pos == std::string_view::npos) {
      continue; // Skip lines without '|'
    }

    std::string_view key = line.substr(0, pipe_pos);
    std::string_view values = line.substr(pipe_pos + 1);

    // Split values using ',' and store them in a vector
    std::vector<std::string_view> value_list;
    auto val_it = split_iterator<std::string_view::const_iterator>(
      values.begin(), values.end(), first_finder(",", boost::algorithm::is_equal()));

    while (val_it != split_iterator<std::string_view::const_iterator>()) {
      value_list.emplace_back(val_it->begin(), val_it->end());
      ++val_it;
    }

    // Insert the key-value pair into the map
    pp_dict.emplace(key, std::move(value_list));
  }
  return pp_dict;
};

}
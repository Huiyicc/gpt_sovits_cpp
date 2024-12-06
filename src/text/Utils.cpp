//
// Created by 19254 on 24-11-30.
//
#include <GPTSovits/Text/Utils.h>
#include <srell.hpp>
#include <utf8/checked.h>

namespace GPTSovits::Text {

void SplitByRegex(std::vector<std::string> &result, const std::string &source, const std::string &regex) {
  srell::regex re(R"([,.;?!、，。？！;：…])");
  srell::sregex_token_iterator it(source.begin(), source.end(), re, -1);
  srell::sregex_token_iterator end;

  std::string lastToken;
  auto last_it = it;
  for (; it != end; ++it) {
    // 获取当前的分割部分
    std::string token = *it;
    // 如果上一个 token 不为空，说明有分隔符
    if (!lastToken.empty()) {
      // 将上一个 token 和当前的分隔符组合
      result.push_back(lastToken + (last_it != end ? last_it->str() : ""));
    }
    lastToken = token; // 更新上一个 token
    last_it = it;
  }

  // 处理最后一个 token
  if (!lastToken.empty()) {
    result.push_back(lastToken);
  }
}

void
SplitByRegex(std::vector<std::u32string> &result, const std::u32string &input, const std::u32string &delimiterPattern) {
  srell::u32regex regexPattern(U"(" + delimiterPattern + U")");
  srell::u32sregex_token_iterator iter(input.begin(), input.end(), regexPattern, {-1, 0});
  srell::u32sregex_token_iterator end;

  std::u32string currentPart;

  for (; iter != end; ++iter) {
    if (iter->str().empty()) {
      continue;
    }

    if (srell::regex_match(iter->str(), regexPattern)) {
      // If it's a delimiter, append it to the current part
      currentPart += *iter;
    } else {
      // If it's a non-delimiter part, push the current part to result and reset it
      if (!currentPart.empty()) {
        result.push_back(currentPart);
      }
      currentPart = *iter;
    }
  }

  // Push the last accumulated part
  if (!currentPart.empty()) {
    result.push_back(currentPart);
  }
}

bool safe_isalpha(int c) {
  return (c >= 0 && c <= 255) && std::isalpha(static_cast<unsigned char>(c));
}

bool safe_isdigit(int c) {
  return (c >= 0 && c <= 255) && std::isdigit(static_cast<unsigned char>(c));
}

// Helper function to check if a character is a space
bool safe_isspace(char32_t ch) {
  return ch == U' ' || ch == U'\t' || ch == U'\n' || ch == U'\r' || ch == U'\f' || ch == U'\v';
}

bool ispunct(char32_t ch) {
  return (ch >= 0x21 && ch <= 0x2F) || // !"#$%&'()*+,-./
         (ch >= 0x3A && ch <= 0x40) || // :;<=>?@
         (ch >= 0x5B && ch <= 0x60) || // [\]^_`
         (ch >= 0x7B && ch <= 0x7E);   // {|}~
};

// Function to trim spaces from both ends of a u32string
std::u32string U32trim(const std::u32string &str) {
  size_t first = 0;
  size_t last = str.size();

  // Find the first non-space character from the left
  while (first < last && safe_isspace(str[first])) {
    ++first;
  }

  // Find the first non-space character from the right
  while (last > first && safe_isspace(str[last - 1])) {
    --last;
  }

  // Return the trimmed substring
  return str.substr(first, last - first);
}

bool FindEnd(const std::string &source, const std::vector<char> &checks) {
  if (source.empty()) return false;
  return std::find(checks.begin(), checks.end(), source.back()) != checks.end();
}
bool FindEnd(const std::string &source, const std::vector<std::string> &checks) {
  for (const auto &check : checks) {
    if (source.size() >= check.size() &&
        source.compare(source.size() - check.size(), check.size(), check) == 0) {
      return true;
    }
  }
  return false;
}
bool FindEnd(const std::u32string &source, const std::vector<char32_t> &checks) {
  if (source.empty()) return false;
  return std::find(checks.begin(), checks.end(), source.back()) != checks.end();
}
bool FindEnd(const std::u32string &source, const std::vector<std::u32string> &checks) {
  for (const auto &check : checks) {
    if (source.size() >= check.size() &&
        std::equal(check.rbegin(), check.rend(), source.rbegin())) {
      return true;
    }
  }
  return false;
}

std::vector<std::pair<int, int>> UTF8CharacterBoundaryIndices(const std::string &input) {
  std::vector<std::pair<int, int>> boundaryIndexs;
  auto it = input.begin();
  auto end = input.end();
  auto oldIt = it;
  auto index = 0;
  while (it < end) {
    // 获取下一个 UTF-8 字符
    utf8::next(it, end);
    auto dist = std::distance(oldIt, it);
    boundaryIndexs.emplace_back(index, dist);
    index += dist;
    oldIt = it;
  }
  return boundaryIndexs;
}

}

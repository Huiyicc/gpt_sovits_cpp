//
// Created by 19254 on 24-11-30.
//
#include <GPTSovits/Text/Utils.h>
#include <srell.hpp>
#include <utf8/checked.h>

namespace GPTSovits::Text {

void SplitByRegex(std::vector<std::string> &result, const std::string &source, const std::string &regex) {
  srell::regex re(regex);
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

void SplitByRegex1(std::vector<std::u32string>&result,const std::u32string& text,const std::u32string&marks) {
  srell::u32regex marks_re(marks);

  srell::u32sregex_token_iterator iter(text.begin(), text.end(), marks_re, -1);
  srell::u32sregex_token_iterator end;

  while (iter != end) {
    result.push_back(*iter);
    ++iter;
  }

}

void FindAll1(std::vector<std::u32string>&result,const std::u32string& text,const std::u32string&marks) {
  srell::u32regex marks_re(marks);
  srell::u32sregex_iterator currentMatch(text.begin(), text.end(), marks_re);
  srell::u32sregex_iterator lastMatch;

  while (currentMatch != lastMatch) {
    srell::u32smatch match = *currentMatch;
    result.push_back(match.str());
    currentMatch++;
  }

}

void tolower(std::u32string& str) {
  for (char32_t& c : str) {
    // 处理基本拉丁字母（Basic Latin）A-Z
    if (c >= U'\x41' && c <= U'\x5A') {
      c += 32;
      continue;
    }

    // 处理拉丁补充字母-1（Latin-1 Supplement）À-Þ
    if (c >= U'\xC0' && c <= U'\xDE' && c != U'\xD7') {
      c += 32;
      continue;
    }

    // 处理拉丁扩展字母-A（Latin Extended-A）
    if (c >= U'\x0100' && c <= U'\x0136' && (c % 2 == 0)) {
      c += 1;
      continue;
    }
    if (c >= U'\x0139' && c <= U'\x0147' && (c % 2 == 1)) {
      c += 1;
      continue;
    }
    if (c >= U'\x014A' && c <= U'\x0178' && (c % 2 == 0)) {
      c += 1;
      continue;
    }
    if (c >= U'\x0179' && c <= U'\x017E' && (c % 2 == 1)) {
      c += 1;
      continue;
    }

    // 处理拉丁扩展字母-B（Latin Extended-B）
    // 常用范围
    if (c >= U'\x0181' && c <= U'\x0186') {
      switch (c) {
        case U'\x0181': c = U'\x0253'; break;
        case U'\x0182': c = U'\x0183'; break;
        case U'\x0184': c = U'\x0185'; break;
        case U'\x0186': c = U'\x0254'; break;
      }
      continue;
    }

    // 希腊字母（Greek and Coptic）
    if (c >= U'\x0391' && c <= U'\x03A9') {
      c += 32;
      continue;
    }

    // 西里尔字母（Cyrillic）
    if (c >= U'\x0410' && c <= U'\x042F') {
      c += 32;
      continue;
    }
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

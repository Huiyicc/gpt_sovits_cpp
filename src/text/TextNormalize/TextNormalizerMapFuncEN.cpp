//
// Created by 19254 on 24-11-10.
//
#include "GPTSovits/Text/TextNormalizerMap.h"
#include <regex>
#include <boost/algorithm/string.hpp>
#include <GPTSovits/Text/Coding.h>

namespace GPTSovits::Text {


std::u32string remove_commas(const srell::u32smatch &match) {
  std::u32string matched_str = match[1].str(); // Get the matched group
  std::u32string result;
  for (char32_t ch: matched_str) {
    if (ch != U',') { // Remove commas
      result += ch;
    }
  }
  return result;
}

std::u32string expand_decimal_point(const srell::u32smatch &m) {
  std::u32string number = m.str();
  std::size_t pos = number.find(U'.');
  if (pos != std::u32string::npos) {
    number.replace(pos, 1, U" point ");
  }
  return number;
}


std::u32string expand_dollars(const srell::u32smatch &m) {
  std::u32string match = m[1].str();
  std::size_t dot_pos = match.find(U'.');
  std::u32string dollars_str = dot_pos != std::u32string::npos ? match.substr(0, dot_pos) : match;
  std::u32string cents_str = dot_pos != std::u32string::npos ? match.substr(dot_pos + 1) : U"";

  int dollars = !dollars_str.empty() ? std::stoi(U32StringToString(dollars_str)) : 0;
  int cents = !cents_str.empty() ? std::stoi(U32StringToString(cents_str)) : 0;

  std::u32string result;
  if (dollars && cents) {
    std::string dollar_unit = dollars == 1 ? "dollar" : "dollars";
    std::string cent_unit = cents == 1 ? "cent" : "cents";
    result = StringToU32String(
      std::to_string(dollars) + " " + dollar_unit + ", " + std::to_string(cents) + " " + cent_unit);
  } else if (dollars) {
    std::string dollar_unit = dollars == 1 ? "dollar" : "dollars";
    result = StringToU32String(std::to_string(dollars) + " " + dollar_unit);
  } else if (cents) {
    std::string cent_unit = cents == 1 ? "cent" : "cents";
    result = StringToU32String(std::to_string(cents) + " " + cent_unit);
  } else {
    result = U"zero dollars";
  }

  return result;
}

static std::vector<std::u32string> belowTwenty = {U"", U"One", U"Two", U"Three", U"Four", U"Five", U"Six", U"Seven",
                                                  U"Eight",
                                                  U"Nine",
                                                  U"Ten",
                                                  U"Eleven", U"Twelve", U"Thirteen", U"Fourteen", U"Fifteen",
                                                  U"Sixteen",
                                                  U"Seventeen",
                                                  U"Eighteen", U"Nineteen"};
static std::vector<std::u32string> tens = {U"", U"", U"Twenty", U"Thirty", U"Forty", U"Fifty", U"Sixty", U"Seventy",
                                           U"Eighty",
                                           U"Ninety"};
static std::vector<std::u32string> thousands = {U"", U"Thousand", U"Million", U"Billion"};

std::u32string helper(int num) {
  if (num == 0) return U"";
  else if (num < 20) return belowTwenty[num] + U" ";
  else if (num < 100) return tens[num / 10] + U" " + helper(num % 10);
  else return belowTwenty[num / 100] + U" Hundred " + helper(num % 100);
}

std::u32string numberToWords(int num) {
  if (num == 0) return U"Zero";
  int i = 0;
  std::u32string words = U"";

  while (num > 0) {
    if (num % 1000 != 0) {
      words = helper(num % 1000) + thousands[i] + U" " + words;
    }
    num /= 1000;
    i++;
  }

  return words.substr(0, words.length() - 1); // Remove trailing space
}

std::u32string expand_ordinal(const srell::u32smatch &m) {
  std::u32string numberStr(m[0].first, m[0].second);
  int number = std::stoi(U32StringToString(numberStr));
  return numberToWords(number);
}

std::u32string expand_number(const srell::u32smatch &m) {
  std::u32string numberStr(m[0].first, m[0].second);
  int number = std::stoi(U32StringToString(numberStr));
  return numberToWords(number);
}

}
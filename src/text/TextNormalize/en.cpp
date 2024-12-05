//
// Created by 19254 on 24-12-2.
//
#include <GPTSovits/Text/TextNormalizer/en.h>
#include <map>
#include <GPTSovits/Text/Coding.h>
#include <GPTSovits/Text/TextNormalizerMap.h>
#include "srell.hpp"

namespace GPTSovits::Text {


std::u32string normalize_numbers(const std::u32string &text) {
  static srell::u32regex comma_number_re(UR"(([0-9][0-9,]+[0-9]))");
  auto pocText = process_with_regex(text, comma_number_re, remove_commas);

  static srell::u32regex pounds_re(UR"(£([0-9,]*[0-9]+))");
  pocText = srell::regex_replace(pocText, pounds_re, U"$1 pounds");

  static srell::u32regex dollars_re(UR"(\$([0-9.,]*[0-9]+))");
  pocText = process_with_regex(pocText, dollars_re, expand_dollars);


  static srell::u32regex decimal_number_re(UR"(([0-9]+.[0-9]+))");
  pocText = process_with_regex(pocText, decimal_number_re, expand_decimal_point);

  static srell::u32regex ordinal_re(UR"([0-9]+(st|nd|rd|th))");
  pocText = process_with_regex(pocText, ordinal_re, expand_ordinal);

  static srell::u32regex number_re(UR"([0-9]+)");
  pocText = process_with_regex(pocText, number_re, expand_number);

  // ++ -> plus plus
  static srell::u32regex plus_re(UR"((?<!\+)\+\+(?!\+))");
  pocText = srell::regex_replace(pocText, plus_re, U" plus plus ");

  // -- -> minus minus
  static srell::u32regex minus_re(UR"((?<!-)--(?!-))");
  pocText = srell::regex_replace(pocText, minus_re, U" minus minus ");

  return pocText;
}

std::unordered_map<char32_t, char32_t> accent_map = {
  {U'à', U'a'},
  {U'á', U'a'},
  {U'â', U'a'},
  {U'ä', U'a'},
  {U'è', U'e'},
  {U'é', U'e'},
  {U'ê', U'e'},
  {U'ë', U'e'},
  {U'ì', U'i'},
  {U'í', U'i'},
  {U'î', U'i'},
  {U'ï', U'i'},
  {U'ò', U'o'},
  {U'ó', U'o'},
  {U'ô', U'o'},
  {U'ö', U'o'},
  {U'ù', U'u'},
  {U'ú', U'u'},
  {U'û', U'u'},
  {U'ü', U'u'},
  {U'ç', U'c'},
  {U'ñ', U'n'},
  {U'ÿ', U'y'},
};

std::u32string strip_accents(const std::u32string &text) {
  std::u32string result;
  for (char32_t ch: text) {
    auto it = accent_map.find(ch);
    if (it != accent_map.end()) {
      result.push_back(it->second);
    } else {
      result.push_back(ch);
    }
  }
  return result;
}

std::u32string replace_consecutive_punctuation(const std::u32string& text) {
  static srell::u32regex regex_pattern(UR"(([!?…,.-])([!?…,.-])+)");

  // 使用正则表达式替换
  return srell::regex_replace(text, regex_pattern, U"$1");
}

std::string text_normalize_en(const std::string &text) {
  // 适配中文及 g2p_en 标点
  static std::vector<std::pair<srell::u32regex, std::u32string>> repList = {
    {srell::u32regex{UR"([;:：，；])"}, U","},
    {srell::u32regex{UR"(["’])"},    U"'"},
    {srell::u32regex{UR"(。)"},       U"."},
    {srell::u32regex{UR"(！)"},       U"!"},
    {srell::u32regex{UR"(？)"},       U"?"},
  };
  std::u32string result = StringToU32String(text);
  for (const auto &rep: repList) {
    const auto &[pattern, replacement] = rep;
    result = srell::regex_replace(result, pattern, replacement);
  }
  // 增加大写兼容
  result = normalize_numbers(result);
  result = strip_accents(result);

  // 删除特殊符号
  srell::u32regex del_re(UR"([^ A-Za-z'.,?!\\-])");
  result = srell::regex_replace(result, del_re, U"");

  // i.e. -> that is
  srell::u32regex ti_re(UR"((?i)i\.e\.)");
  result = srell::regex_replace(result, ti_re, U"that is");

  // e.g. -> for example
  srell::u32regex fe_re(UR"((?i)e\.g\.)");
  result = srell::regex_replace(result, fe_re, U"for example");

  result = replace_consecutive_punctuation(result);

  return U32StringToString(result);
}


}
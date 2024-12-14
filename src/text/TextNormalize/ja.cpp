//
// Created by 19254 on 24-12-12.
//
#include <GPTSovits/Text/TextNormalizer/ja.h>
#include <srell.hpp>

namespace GPTSovits::Text {

std::u32string replace_consecutive_punctuation_ja(const std::u32string& text) {
  // Define the punctuations to be considered
  std::u32string punctuations = U"!\\?…,\\.-";

  // Create the regex pattern
  std::u32string pattern = U"([!\\?…,\\.-])([!\\?…,\\.-])+";

  // Create a regex object
  srell::u32regex re(pattern);

  // Use regex_replace to replace occurrences
  std::u32string result = srell::regex_replace(text, re, U"$1");

  return result;
}

std::u32string text_normalize_ja(const std::u32string &text) {
  return replace_consecutive_punctuation_ja(text);
};

}
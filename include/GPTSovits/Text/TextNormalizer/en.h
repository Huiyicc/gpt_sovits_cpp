//
// Created by 19254 on 24-12-2.
//

#ifndef GPT_SOVITS_CPP_TEXT_NORMALIZER_EN_H
#define GPT_SOVITS_CPP_TEXT_NORMALIZER_EN_H

#include <regex>
#include "srell.hpp"
#include <GPTSovits/Text/TextNormalizer/base.h>

namespace GPTSovits::Text {

std::string text_normalize_en(const std::string &text);

//class TextNormalizerEN {
//public:
//  TextNormalizerEN();
////  std::u32string NormalizeSentence(const std::u32string &sentence);
////  std::vector<std::u32string> Normalize(const std::u32string &text);
//
//
//private:
////  srell::u32regex SENTENCE_SPLITOR;
////
////  std::vector<std::u32string> split(const std::u32string &text, const std::string &lang = "zh");
////
////  std::u32string postReplace(const std::u32string &sentence);
//};

}

#endif //GPT_SOVITS_CPP_TEXT_NORMALIZER_EN_H
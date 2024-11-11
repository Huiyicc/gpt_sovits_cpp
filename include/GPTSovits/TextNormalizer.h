//
// Created by 19254 on 24-11-10.
//

#ifndef GPT_SOVITS_CPP_TEXTNORMALIZER_H
#define GPT_SOVITS_CPP_TEXTNORMALIZER_H

#include <regex>
#include "srell.hpp"

namespace GPTSovits {

std::u32string StringToU32String(const std::string&text);
std::string U32StringToString(const std::u32string &text);

std::string text_normalize(const std::string &text);

class TextNormalizer {
public:
  TextNormalizer() : SENTENCE_SPLITOR(UR"(([：、，；。？！,;?!][”’]?))") {}
  std::u32string NormalizeSentence(const std::u32string &sentence);
  std::vector<std::u32string> Normalize(const std::u32string &text);


private:
  srell::u32regex SENTENCE_SPLITOR;

  std::vector<std::u32string> split(const std::u32string &text, const std::string &lang = "zh");

  std::u32string postReplace(const std::u32string &sentence);
};

};

#endif //GPT_SOVITS_CPP_TEXTNORMALIZER_H

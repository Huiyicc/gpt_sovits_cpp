//
// Created by 19254 on 24-11-29.
//
#include <GPTSovits/Text/Coding.h>
#include "utf8.h"

namespace GPTSovits::Text {

std::u32string StringToU32String(const std::string &text) {
  std::u32string out;
  utf8::utf8to32(text.begin(), text.end(), std::back_inserter(out));
  return out;
};

std::string U32StringToString(const std::u32string &text) {
  std::string out;
  utf8::utf32to8(text.begin(), text.end(), std::back_inserter(out));
  return out;
}

}
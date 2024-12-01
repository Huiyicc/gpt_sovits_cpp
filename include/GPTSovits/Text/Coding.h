//
// Created by 19254 on 24-11-29.
//

#ifndef GPT_SOVITS_CPP_CODING_H
#define GPT_SOVITS_CPP_CODING_H

#include <string>

namespace GPTSovits::Text {

std::u32string StringToU32String(const std::string&text);
std::string U32StringToString(const std::u32string &text);

}

#endif //GPT_SOVITS_CPP_CODING_H

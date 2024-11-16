//
// Created by 19254 on 24-11-17.
//

#ifndef GPT_SOVITS_CPP_UTILS_H
#define GPT_SOVITS_CPP_UTILS_H

#include <string>

namespace GPTSovits {

bool safe_isalpha(int c);

bool safe_isdigit(int c);

bool safe_isspace(char32_t ch);
std::u32string U32trim(const std::u32string& str);

}

#endif //GPT_SOVITS_CPP_UTILS_H

//
// Created by 19254 on 24-11-11.
//

#ifndef GPT_SOVITS_CPP_POLYPHONIC_H
#define GPT_SOVITS_CPP_POLYPHONIC_H

#include <string>
#include <unordered_map>
#include <vector>

namespace GPTSovits {
const std::unordered_map<std::string_view, std::vector<std::string_view>> &get_polyphonic_map();
};

#endif //GPT_SOVITS_CPP_POLYPHONIC_H

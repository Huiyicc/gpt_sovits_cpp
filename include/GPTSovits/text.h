//
// Created by 19254 on 24-11-10.
//

#ifndef GPT_SOVITS_CPP_TEXT_H
#define GPT_SOVITS_CPP_TEXT_H

#include <string>

namespace GPTSovits {

struct G2PRes {
  std::vector<std::string> phones;
  std::vector<int> word2ph;
  std::string norm_text;
};

G2PRes CleanText(const std::string &text);

std::tuple<std::vector<std::string>, std::vector<int>>
g2p(const std::string &text);

}

#endif //GPT_SOVITS_CPP_TEXT_H

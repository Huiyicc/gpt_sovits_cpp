//
// Created by 19254 on 24-12-1.
//

#ifndef GPT_SOVITS_CPP_G2P_EN_H
#define GPT_SOVITS_CPP_G2P_EN_H

#include <GPTSovits/G2P/g2p.h>
#include <string>
#include <map>

namespace GPTSovits::G2P {

namespace g2p_en {
std::vector<std::string> predict(const std::string &text);
}

class G2PEN : public IG2P {
public:
  G2PRes _cleanText(const std::string &text) override ;
};



};

#endif //GPT_SOVITS_CPP_G2P_EN_H

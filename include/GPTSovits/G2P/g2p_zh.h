//
// Created by 19254 on 24-12-1.
//

#ifndef GPT_SOVITS_CPP_G2P_ZH_H
#define GPT_SOVITS_CPP_G2P_ZH_H

#include <GPTSovits/G2P/g2p.h>

namespace GPTSovits::G2P {

class G2PZH : public IG2P {
protected:
  G2PRes _cleanText(const std::string &text) override ;
};

};

#endif //GPT_SOVITS_CPP_G2P_ZH_H

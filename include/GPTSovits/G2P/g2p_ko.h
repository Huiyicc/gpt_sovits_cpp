//
// Created by 19254 on 24-12-12.
//

#ifndef GPT_SOVITS_CPP_G2P_KO_H
#define GPT_SOVITS_CPP_G2P_KO_H

#include <GPTSovits/G2P/g2p.h>

namespace GPTSovits::G2P {

class G2PKO : public IG2P {
protected:
  G2PRes _cleanText(const std::string &text) override;
};


};// namespace GPTSovits::G2P


#endif//GPT_SOVITS_CPP_G2P_KO_H

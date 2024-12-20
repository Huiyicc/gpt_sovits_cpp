//
// Created by 19254 on 24-12-4.
//

#ifndef GPT_SOVITS_CPP_G2P_REGISTER_H
#define GPT_SOVITS_CPP_G2P_REGISTER_H

#include <string>
#include <map>
#include <memory>
#include <GPTSovits/G2P/g2p.h>
#include <GPTSovits/config.h>
#include <optional>

namespace GPTSovits::Bert {

struct BertRes {
  ~BertRes();
  std::shared_ptr<at::Tensor> PhoneSeq;
  std::shared_ptr<at::Tensor> BertSeq;
};
class IBert {
public:
  virtual BertRes Encode(G2P::G2PRes&g2pData) = 0;
};

extern std::map<std::string, std::shared_ptr<IBert>> g_bert_map;

void RegisterChinese(GPTSovits&gsv, const std::string &modelPath, const std::string &tokenizerPath,bool warmUP=false);
void RegisterEnglish(GPTSovits&gsv,bool warmUP=false);
void RegisterJapanese(GPTSovits&gsv,bool warmUP=false);

std::optional<std::shared_ptr<IBert>> MakeFromLang(const std::string &lang);

}

#endif //GPT_SOVITS_CPP_G2P_REGISTER_H

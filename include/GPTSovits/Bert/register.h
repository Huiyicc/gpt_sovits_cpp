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

void RegisterChinese(std::shared_ptr<TorchDevice> device, const std::string &modelPath, const std::string &tokenizerPath);
void RegisterEnglish(std::shared_ptr<TorchDevice> device);

std::shared_ptr<IBert> MakeFromLang(const std::string&lang);

}

#endif //GPT_SOVITS_CPP_G2P_REGISTER_H

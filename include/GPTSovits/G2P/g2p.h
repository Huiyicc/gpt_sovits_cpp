//
// Created by 19254 on 24-12-1.
//

#ifndef GPT_SOVITS_CPP_G2P_H
#define GPT_SOVITS_CPP_G2P_H

#include <memory>
#include <string>
#include <vector>

namespace GPTSovits {
class GPTSovits;
}
namespace at {
class Tensor;
}
namespace GPTSovits::Bert{
struct BertRes;
}
namespace GPTSovits::G2P {

struct G2PRes {
  std::vector<std::string> phones;
  std::vector<int> phone_ids;
  std::vector<int> word2ph;
  std::string norm_text;
};

class IG2P {

public:

  virtual G2PRes CleanText(const std::string &text) final;
private:
  // CleanText重写这个逻辑
  virtual G2PRes _cleanText(const std::string &text) = 0;
};

std::shared_ptr<IG2P> MakeFromLang(const std::string&lang);

std::shared_ptr<Bert::BertRes> GetPhoneAndBert(GPTSovits &gpt, const std::string &text);

}

#endif //GPT_SOVITS_CPP_G2P_H

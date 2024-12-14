//
// Created by 19254 on 24-12-14.
//
//#include <torch/torch.h>
#include <GPTSovits/Bert/CNBertModel.h>
#include <GPTSovits/Bert/register.h>
#include <GPTSovits/gpt_sovits.h>
#include <GPTSovits/plog.h>
#include <memory>
#include <torch/script.h>

namespace GPTSovits::Bert {

class BertJA : public IBert {
  std::shared_ptr<TorchDevice> m_device;

public:
  BertJA(std::shared_ptr<TorchDevice> device);

  BertRes
  Encode(G2P::G2PRes &g2pData) override;
};

BertJA::BertJA(std::shared_ptr<TorchDevice> device) : m_device(device) {}

BertRes BertJA::Encode(G2P::G2PRes &g2pData) {
  auto bert_seq = torch::zeros({static_cast<int64_t>(g2pData.phones.size()), 1024},
                               torch::TensorOptions().dtype(torch::kFloat))
                    .to(*m_device);

  auto phone_seq = torch::from_blob(g2pData.phone_ids.data(), {static_cast<long>(g2pData.phone_ids.size())},
                                    torch::kInt)
                     .to(*m_device)
                     .unsqueeze(0);
  return {
    std::make_unique<torch::Tensor>(torch::cat({phone_seq}, 1).to(*m_device)),
    std::make_unique<torch::Tensor>(torch::cat({bert_seq}, 0).to(*m_device)),
  };
}

void RegisterJapanese(GPTSovits&gsv,bool warmUP) {
  if (auto iter = g_bert_map.find("ja"); iter != g_bert_map.end()) {
    return;
  }
  g_bert_map["ja"] = std::make_shared<BertJA>(gsv.Device());
  // 预热
  if (warmUP) {
    PrintInfo("预热语言: ja");
    G2P::GetPhoneAndBert(gsv, "こんにちは","ja");
  }
}

};// namespace GPTSovits::Bert
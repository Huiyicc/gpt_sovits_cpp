//
// Created by 19254 on 24-12-4.
//
//#include <torch/torch.h>
#include <GPTSovits/Bert/CNBertModel.h>
#include <GPTSovits/Bert/register.h>
#include <GPTSovits/gpt_sovits.h>
#include <GPTSovits/plog.h>
#include <memory>
#include <torch/script.h>

namespace GPTSovits::Bert {

class BertEN : public IBert {
  std::shared_ptr<TorchDevice> m_device;

public:
  BertEN(std::shared_ptr<TorchDevice> device);

  BertRes
  Encode(G2P::G2PRes &g2pData) override;
};

BertEN::BertEN(std::shared_ptr<TorchDevice> device) : m_device(device) {}

BertRes BertEN::Encode(G2P::G2PRes &g2pData) {
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

void RegisterEnglish(GPTSovits&gsv, bool warmUP) {
  if (auto iter = g_bert_map.find("en"); iter != g_bert_map.end()) {
    return;
  }
  g_bert_map["en"] = std::make_shared<BertEN>(gsv.Device());
  // 预热
  if (warmUP) {
    PrintInfo("预热语言: en");
    G2P::GetPhoneAndBert(gsv, "hello","en");
  }
}

};// namespace GPTSovits::Bert
//
// Created by 19254 on 24-12-4.
//
//#include <torch/torch.h>
#include <torch/script.h>
#include <memory>
#include <GPTSovits/Bert/register.h>
#include <GPTSovits/Bert/CNBertModel.h>

namespace GPTSovits::Bert {

class BertCN : public IBert {
  std::unique_ptr<CNBertModel> m_zhBert;
  std::shared_ptr<TorchDevice> m_device;
public:
  BertCN(std::shared_ptr<TorchDevice> device, const std::string &modelPath, const std::string &tokenizerPath);

  BertRes
  Encode(G2P::G2PRes &g2pData) override;
};

BertCN::BertCN(std::shared_ptr<TorchDevice> device, const std::string &modelPath, const std::string &tokenizerPath)
  : m_device(device) {
  m_zhBert = CNBertModel::Make(*device, std::move(modelPath), std::move(tokenizerPath));
}

BertRes BertCN::Encode(G2P::G2PRes &g2pData) {
  // 中文编码
  auto encodeResult = m_zhBert->EncodeText(g2pData.norm_text);
  auto textWord2ph = torch::from_blob(g2pData.word2ph.data(), {static_cast<long>(g2pData.word2ph.size())},
                                      torch::kInt).to(*m_device);
  std::vector<torch::jit::IValue> inputs = {
    *encodeResult.TokenIds,
    *encodeResult.Masks,
    *encodeResult.TokenTypeIds,
    textWord2ph,
  };
  auto bert_seq = m_zhBert->GetModule()->forward(inputs).toTensor().to(*m_device);
  auto phone_seq = torch::from_blob(g2pData.phone_ids.data(), {static_cast<long>(g2pData.phone_ids.size())},
                                    torch::kInt).to(*m_device).unsqueeze(0);
  return {
    std::make_unique<torch::Tensor>(torch::cat({phone_seq}, 1).to(*m_device)),
    std::make_unique<torch::Tensor>(torch::cat({bert_seq}, 0).to(*m_device)),
  };
}

void
RegisterChinese(std::shared_ptr<TorchDevice> device, const std::string &modelPath, const std::string &tokenizerPath) {
  if (auto iter = g_bert_map.find("zh");iter != g_bert_map.end()) {
    return;
  }
  g_bert_map["zh"] = std::make_shared<BertCN>(device, modelPath, tokenizerPath);
}

};
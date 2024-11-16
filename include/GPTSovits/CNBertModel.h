//
// Created by 19254 on 24-11-10.
//

#ifndef GPT_SOVITS_CPP_CNBERTMODEL_H
#define GPT_SOVITS_CPP_CNBERTMODEL_H

#include <memory>
#include <tokenizers_cpp.h>
#include <torch/script.h>
#include "GPTSovits/tool.h"

namespace GPTSovits {
class GPTSovits;

struct BertRes;

class CNBertModel {
private:
  std::unique_ptr<tokenizers::Tokenizer> m_tokenzer;
  std::unique_ptr<torch::jit::Module> m_module;
  std::unique_ptr<TorchDevice> m_devices;

  friend class std::unique_ptr<CNBertModel>;

  friend std::unique_ptr<BertRes> GetPhoneAndBert(GPTSovits &gpt, const std::string &text);

  CNBertModel() {};

public:

  struct EncodeResult {
    torch::Tensor TokenIds;
    torch::Tensor TokenTypeIds;
    torch::Tensor Masks;
  };


  EncodeResult EncodeText(const std::string &text);

  static std::unique_ptr<CNBertModel>
  Make(TorchDevice &device, std::string_view bert_model_path, std::string_view tokenzer_path);

};

}

#endif //GPT_SOVITS_CPP_CNBERTMODEL_H

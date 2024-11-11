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

class CNBertModel {
private:
  std::unique_ptr<tokenizers::Tokenizer> m_tokenzer;
  std::unique_ptr<torch::jit::Module> m_module;

  friend class std::unique_ptr<CNBertModel>;
  explicit CNBertModel(std::unique_ptr<torch::jit::Module> bert_model,std::unique_ptr<tokenizers::Tokenizer> tokenzer);

public:

  static std::unique_ptr<CNBertModel> Make(TorchDevice &device, std::string_view bert_model_path, std::string_view tokenzer_path);

};

}

#endif //GPT_SOVITS_CPP_CNBERTMODEL_H

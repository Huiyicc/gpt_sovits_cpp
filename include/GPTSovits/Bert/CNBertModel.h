//
// Created by 19254 on 24-11-10.
//

#ifndef GPT_SOVITS_CPP_CNBERTMODEL_H
#define GPT_SOVITS_CPP_CNBERTMODEL_H
namespace at {
class Tensor;
}
#include <memory>
#include <tokenizers_cpp.h>
#include "GPTSovits/tool.h"
#include <GPTSovits/G2P/g2p.h>
namespace torch::jit {
struct Module;
}

namespace GPTSovits {
class GPTSovits;

class CNBertModel {
private:
  std::unique_ptr<tokenizers::Tokenizer> m_tokenzer;
  std::shared_ptr<torch::jit::Module> m_module;
  std::unique_ptr<TorchDevice> m_devices;

  friend class std::unique_ptr<CNBertModel>;

  friend class IG2P;
  friend class IBert;

  CNBertModel();

public:

  struct EncodeResult {
    std::unique_ptr<at::Tensor> TokenIds;
    std::unique_ptr<at::Tensor> TokenTypeIds;
    std::unique_ptr<at::Tensor> Masks;
  };


  EncodeResult EncodeText(const std::string &text);

  std::shared_ptr<torch::jit::Module>
  GetModule();

  static std::unique_ptr<CNBertModel>
  Make(TorchDevice &device, std::string_view bert_model_path, std::string_view tokenzer_path);

};

}

#endif //GPT_SOVITS_CPP_CNBERTMODEL_H

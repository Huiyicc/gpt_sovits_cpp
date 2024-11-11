//
// Created by 19254 on 24-11-6.
//
#include "GPTSovits/GPTSovits.h"
#include <fstream>

namespace GPTSovits {

std::unique_ptr<GPTSovitsConfig> GPTSovitsConfig::Make(std::string_view cn_bert_path,
                                                       std::string_view tokenizer_path,
                                                       std::string_view ssl_path) {
  return std::make_unique<GPTSovitsConfig>(cn_bert_path, tokenizer_path, ssl_path);
}


std::unique_ptr<GPTSovits> GPTSovitsConfig::Build(TorchDevice &device) {
  return GPTSovits::Make(
    CNBertModel::Make(device, std::move(m_CnBertPath), std::move(m_TokenizerPath)),
    std::make_unique<torch::jit::Module>(torch::jit::load(m_SslPath, device)),
    device
  );
}

}
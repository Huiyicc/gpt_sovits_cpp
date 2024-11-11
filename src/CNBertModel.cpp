//
// Created by 19254 on 24-11-10.
//
#include <utility>
#include <fstream>

#include "GPTSovits/CNBertModel.h"
#include "GPTSovits/exception.h"

namespace GPTSovits {

std::unique_ptr<CNBertModel>
CNBertModel::Make(TorchDevice &device, std::string_view bert_model_path, std::string_view tokenzer_path) {
  std::ifstream file(tokenzer_path.data());
  if (!file.is_open()) {
    THROW_ERRORN("加载Tokenizer失败\nBy:{}", tokenzer_path.data());
  }
  std::string content;
  file >> content;
  auto tokenizer = tokenizers::Tokenizer::FromBlobJSON(content);
  auto bert = std::make_unique<torch::jit::Module>(torch::jit::load(bert_model_path.data(), device));
  bert->eval();
  return std::unique_ptr<CNBertModel>(new CNBertModel(std::move(bert), std::move(tokenizer)));
};

CNBertModel::CNBertModel(std::unique_ptr<torch::jit::Module> bert_model, std::unique_ptr<tokenizers::Tokenizer> tokenzer)
  : m_module(std::move(bert_model)), m_tokenzer(tokenzer.release()) {

}


}

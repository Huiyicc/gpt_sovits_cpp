//
// Created by 19254 on 24-11-10.
//
#include <utility>
#include <fstream>

#include "GPTSovits/CNBertModel.h"
#include "GPTSovits/Utils/exception.h"
#include "GPTSovits/gpt_sovits.h"

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
  auto resPtr = std::unique_ptr<CNBertModel>(new CNBertModel());
  resPtr->m_module = std::move(bert);
  resPtr->m_tokenzer = std::move(tokenizer);
  resPtr->m_devices = std::make_unique<TorchDevice>(device);
  return resPtr;
};

CNBertModel::EncodeResult CNBertModel::EncodeText(const std::string &text) {
  auto rust_res = m_tokenzer->EncodeEx(text, true);
  EncodeResult res;
  res.TokenIds = std::move(
    torch::from_blob(rust_res.TokenIds.data(),
                     {static_cast<long>(rust_res.TokenIds.size())},
                     torch::kInt).unsqueeze(0).to(*m_devices));
  res.TokenTypeIds = std::move(
    torch::from_blob(rust_res.TokenTypeIds.data(),
                     {static_cast<long>(rust_res.TokenTypeIds.size())},
                     torch::kInt)
      .unsqueeze(0).to(*m_devices));
  res.Masks = std::move(
    torch::from_blob(rust_res.Masks.data(),
                     {static_cast<long>(rust_res.Masks.size())},
                     torch::kInt)
      .unsqueeze(0).to(*m_devices));
  return std::move(res);
};


}

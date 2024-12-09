//
// Created by 19254 on 24-11-6.
//
#include "GPTSovits/GPTSovits.h"
#include <torch/script.h>
//#include <torch/torch.h>
#include <fstream>

namespace GPTSovits {

std::unique_ptr<GPTSovitsConfig> GPTSovitsConfig::Make(std::string_view defaultLang,std::string_view ssl_path) {
  return std::make_unique<GPTSovitsConfig>(ssl_path,defaultLang);
}


std::unique_ptr<GPTSovits> GPTSovitsConfig::Build(std::shared_ptr<TorchDevice> device) {
  auto ssl = std::make_unique<torch::jit::Module>(torch::jit::load(m_SslPath, *device));
  ssl->to(*device);
  if (!ssl) {
    THROW_ERRORN("加载ssl模型失败!\nFrom:{}", m_SslPath);
  }
  auto r = GPTSovits::Make(
    m_defaultLang,
    std::move(ssl),
    device
  );
  return r;
}

}
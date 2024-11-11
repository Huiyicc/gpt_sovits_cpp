//
// Created by 19254 on 24-11-10.
//
#include "GPTSovits/GPTSovits.h"

namespace GPTSovits {

std::unique_ptr<GPTSovits>
GPTSovits::Make(std::unique_ptr<CNBertModel> cnBertModel,
                std::unique_ptr<torch::jit::Module> ssl,
                TorchDevice &devices) {
  return std::unique_ptr<GPTSovits>(new GPTSovits(std::move(cnBertModel), std::move(ssl), devices));
};

GPTSovits::GPTSovits(std::unique_ptr<CNBertModel> cnBertModel,
                     std::unique_ptr<torch::jit::Module> ssl,
                     TorchDevice &devices)
  : m_zhBert(std::move(cnBertModel)),
    m_ssl(std::move(ssl)),
    m_devices(devices) {

};

}
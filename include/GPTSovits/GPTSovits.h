//
// Created by 19254 on 24-11-6.
//

#ifndef _GPTSOVITS_ALL_IN_ONE_H_
#define _GPTSOVITS_ALL_IN_ONE_H_

#include "plog.h"
#include <torch/script.h>
#include <torch/torch.h>
#include "./exception.h"
#include "./config.h"
#include "./tool.h"
#include "./symbols.h"
#include "./CNBertModel.h"
#include "./text.h"
namespace GPTSovits {

class GPTSovits {
  std::unique_ptr<CNBertModel> m_zhBert;
  std::unique_ptr<torch::jit::Module> m_ssl;
  TorchDevice m_devices;
  explicit GPTSovits(std::unique_ptr<CNBertModel> cnBertModel,
                     std::unique_ptr<torch::jit::Module> ssl,
                     TorchDevice& devices);
public:
  static std::unique_ptr<GPTSovits> Make(std::unique_ptr<CNBertModel> cnBertModel,
                                         std::unique_ptr<torch::jit::Module> ssl,
                                         TorchDevice &devices);

};

}

#endif //_GPTSOVITS_ALL_IN_ONE_H_

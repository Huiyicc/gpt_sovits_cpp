//
// Created by 19254 on 24-11-6.
//

#ifndef _GPTSOVITS_CONFIG_H_
#define _GPTSOVITS_CONFIG_H_

#include <string>
#include <memory>
#include "tool.h"
#include <tokenizers_cpp.h>

namespace GPTSovits {

class GPTSovits;

class GPTSovitsConfig {
  std::string m_CnBertPath;
  std::string m_TokenizerPath;
  std::string m_SslPath;

public:
  explicit GPTSovitsConfig(std::string_view cn_bert_path,
                           std::string_view tokenizer_path,
                           std::string_view ssl_path)
    : m_CnBertPath(cn_bert_path),
      m_TokenizerPath(tokenizer_path),
      m_SslPath(ssl_path) {};

  static std::unique_ptr<GPTSovitsConfig> Make(std::string_view cn_bert_path,
                                               std::string_view tokenizer_path,
                                               std::string_view ssl_path);

  std::unique_ptr<GPTSovits> Build(TorchDevice &device);

};

};

#endif //_GPTSOVITS_CONFIG_H_

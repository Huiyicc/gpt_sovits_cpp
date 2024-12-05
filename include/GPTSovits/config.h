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
  std::string m_SslPath;
  std::string m_defaultLang;

public:
  explicit GPTSovitsConfig(std::string_view ssl_path,std::string_view defaultLang)
    : m_SslPath(ssl_path) ,m_defaultLang(defaultLang) {};

  static std::unique_ptr<GPTSovitsConfig> Make(std::string_view defaultLang,std::string_view ssl_path);

  std::unique_ptr<GPTSovits> Build(std::shared_ptr<TorchDevice> device);

};

};

#endif //_GPTSOVITS_CONFIG_H_

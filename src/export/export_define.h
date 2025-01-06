//
// Created by 19254 on 24-12-28.
//

#ifndef EXPORT_DEFINE_H
#define EXPORT_DEFINE_H

#include <memory>
#include "GPTSovits/GPTSovits.h"
#include "GPTSovits/GPTSovits_c.h"

struct device_info {
  std::shared_ptr<GPTSovits::TorchDevice> device;
  std::string name;
};

struct gsv_cli {
  std::unique_ptr<GPTSovits::GPTSovits> gsv;
};

struct gsv_audio {
  std::unique_ptr<GPTSovits::AudioTools> audio;
};

struct gsv_sentence {
  std::unique_ptr<GPTSovits::Text::Sentence> sentence;
};

#endif //EXPORT_DEFINE_H

//
// Created by 19254 on 24-11-6.
//
#include "GPTSovits/GPTSovits.h"


namespace GPTSovits {
TorchDevice GetDefaultDevice() {
  if (torch::cuda::is_available()) {
    return {"cuda:0"};
  } else {
    return {"cpu"};
  }
}

TorchDevice GetDevice(std::string_view device) {
  if (torch::cuda::is_available()) {
    return {device.data()};
  } else {
    return {"cpu"};
  }
}

}
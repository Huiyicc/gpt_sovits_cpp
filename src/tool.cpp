//
// Created by 19254 on 24-11-6.
//
#include "GPTSovits/GPTSovits.h"
#include <torch/torch.h>

namespace GPTSovits {
std::shared_ptr<TorchDevice> GetDefaultDevice() {
  if (torch::cuda::is_available()) {
    auto lc = torch::cuda::device_count();
    for (size_t i = 0; i < lc; i++) {
      std::shared_ptr<TorchDevice> d(nullptr);
      try {
        d = std::make_shared<TorchDevice>(c10::DeviceType::CUDA, i);
      } catch (...) {
        continue;
      }
      return d;
    }
    return std::make_shared<TorchDevice>(c10::DeviceType::CPU);
  } else {
    return std::make_shared<TorchDevice>(c10::DeviceType::CPU);
  }
}

std::shared_ptr<TorchDevice> GetDevice(std::string_view device) {
  if (torch::cuda::is_available()) {
    return std::make_shared<TorchDevice>(device.data());
  } else {
    return std::make_shared<TorchDevice>("cpu");
  }
}


std::string DeviceToString(const TorchDevice& device) {
  std::string outd;
  switch (device.type()) {
    case c10::DeviceType::CPU:
      outd += "CPU";
      break;
    case c10::DeviceType::CUDA:
      outd += "CUDA";
      outd += ":" + std::to_string(device.index());
      break;
    case c10::DeviceType::MKLDNN:
      outd += "MKLDNN";
      break;
    case c10::DeviceType::OPENGL:
      outd += "OPENGL";
      break;
    case c10::DeviceType::OPENCL:
      outd += "OPENCL";
      break;
    case c10::DeviceType::IDEEP :
      outd += "IDEEP";
      break;
    case c10::DeviceType::HIP :
      outd += "HIP";
      break;
    case c10::DeviceType::FPGA:
      outd += "FPGA";
      break;
    case c10::DeviceType::MAIA:
      outd += "MAIA";
      break;
    case c10::DeviceType::XLA :
      outd += "XLA";
      break;
    case c10::DeviceType::Vulkan:
      outd += "Vulkan";
      break;
    case c10::DeviceType::Metal :
      outd += "Metal";
      break;
    case c10::DeviceType::XPU :
      outd += "XPU";
      break;
    case c10::DeviceType::MPS:
      outd += "MPS";
      break;
    case c10::DeviceType::Meta :
      outd += "Meta";
      break;
    case c10::DeviceType::HPU :
      outd += "HPU";
      break;
    case c10::DeviceType::VE :
      outd += "VE";
      break;
    case c10::DeviceType::Lazy :
      outd += "Lazy";
      break;
    case c10::DeviceType::IPU :
      outd += "IPU";
      break;
    case c10::DeviceType::MTIA :
      outd += "MTIA";
      break;
    case c10::DeviceType::PrivateUse1 :
      outd += "PrivateUse1";
      break;
    case c10::DeviceType::COMPILE_TIME_MAX_DEVICE_TYPES:
      break;
  }

  return outd;
}


}
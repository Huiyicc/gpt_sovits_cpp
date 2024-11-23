//
// Created by 19254 on 24-11-6.
//

#ifndef _GPTSOVITS_TOOL_H_
#define _GPTSOVITS_TOOL_H_

#include <memory>

namespace GPTSovits {

typedef c10::Device TorchDevice;

std::shared_ptr<TorchDevice> GetDefaultDevice();

std::shared_ptr<TorchDevice> GetDevice(std::string_view device);

// 当GetDefaultDevice().str()崩溃的时候可以使用这个
std::string DeviceToString(const TorchDevice &device);

}

#endif //_GPTSOVITS_TOOL_H_

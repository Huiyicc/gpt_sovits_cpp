//
// Created by 19254 on 24-11-6.
//

#ifndef _GPTSOVITS_TOOL_H_
#define _GPTSOVITS_TOOL_H_

namespace GPTSovits {

typedef c10::Device TorchDevice;

TorchDevice GetDefaultDevice();

TorchDevice GetDevice(std::string_view device);

}

#endif //_GPTSOVITS_TOOL_H_

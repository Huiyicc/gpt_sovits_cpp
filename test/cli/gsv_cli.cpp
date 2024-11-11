//
// Created by 19254 on 24-11-10.
//
#include <iostream>
#include "GPTSovits/GPTSovits.h"

int main() {
  try {
    std::cout << GPTSovits::Symbols << std::endl;
    auto gpt_config = GPTSovits::GPTSovitsConfig::Make(
      R"(G:\models\tts\gpt_sovits\base\bert_model.pt)",
      R"(G:\models\tts\gpt_sovits\base\tokenizer.json)",
      R"(F:\Engcode\rust\gpt_sovits_rs\resource\ssl_model.pt)");
    auto device = GPTSovits::GetDefaultDevice();
    PrintDebug("device: {}", device.str());
    auto gpt_sovits = gpt_config->Build(device);

  } catch (const GPTSovits::Exception &e) {
    PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
  }
  return 0;
}
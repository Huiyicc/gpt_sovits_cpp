//
// Created by 19254 on 24-11-10.
//
#include <iostream>
#include "GPTSovits/GPTSovits.h"

int main() {
  try {
    auto gpt_config = GPTSovits::GPTSovitsConfig::Make(
      R"(G:\models\tts\gpt_sovits\base\bert_model.pt)",
      R"(G:\models\tts\gpt_sovits\base\tokenizer.json)",
      R"(G:\models\tts\gpt_sovits\base\ssl_model.pt)");
    auto device = GPTSovits::GetDefaultDevice();
    PrintDebug("device: {}", device.str());
    auto gpt_sovits = gpt_config->Build(device);
    gpt_sovits->CreateSpeaker("firefly",
                              "G:/models/tts/gpt_sovits/firefly/241105/gpt_sovits_model.pt",
                              "G:/models/tts/gpt_sovits/firefly/241105/ref.wav",
                              "虽然我也没太搞清楚状况…但他说，似乎只有直率、纯真、有童心的小孩子才能看见它……");
    auto resAudio = gpt_sovits->Infer("firefly","叹息声一声接着一声传出，木兰对着房门织布。");
    resAudio->SaveToFile(R"(C:\Users\19254\Downloads\cpp_out.wav)");

  } catch (const GPTSovits::Exception &e) {
    PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
  } catch (const std::exception &e) {
    PrintError("{}", e.what());
  }
  return 0;
}
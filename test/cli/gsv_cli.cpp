//
// Created by 19254 on 24-11-10.
//
#include <iostream>
#include <filesystem>
#include <GPTSovits/GPTSovits.h>

int main() {
  try {
    auto currentPath = std::filesystem::current_path().string();
    PrintDebug("Current Path:{}", currentPath);
    auto device = GPTSovits::GetDefaultDevice();
    PrintDebug("device: {}", GPTSovits::DeviceToString(*device));

    GPTSovits::Bert::RegisterChinese(device,
                                     currentPath + R"(/../test/model_base/bert_model.pt)",
                                     currentPath + R"(/../test/model_base/tokenizer.json)");
    GPTSovits::Bert::RegisterEnglish(device);

    auto gpt_config = GPTSovits::GPTSovitsConfig::Make("zh",
                                                       currentPath + R"(/../test/model_base/ssl_model.241124.pt)");
    auto gpt_sovits = gpt_config->Build(device);

    gpt_sovits->CreateSpeaker("firefly",
                              currentPath + "/../test/model/gpt_sovits_model.pt",
                              currentPath + "/../test/model/ref.wav",
                              "我突然意识到,如果钟表匠真是无名客，那他岂不是你的老前辈？哎，你想和他拍照留个念吗？我可以帮你哦。");

    std::vector<std::string> texts = {
      "今天是2021年11月23日,天气晴,气温32°C.天下之大内有乾坤。",
      "这个project的schedule有些问题，尤其是buffer不多。另外，cost也偏高。目前我们没法confirm手上的 resource能完全take得了。Anyway我们还是先pilot一下，再follow up最终的output，看能不能run的比较smoothly，更重要的是evaluate所有的cost能不能完全被cover掉……",
      "The C++ support in Jupyter is powered by the xeus-cling C++ kernel.This function is used to find the length (in code points) of a UTF-8 encoded string."
    };
    int n = 0;
    GPTSovits::Text::Sentence sentence;
    sentence.AppendCallBack([&](const std::string &text) -> bool {
      auto resAudio = gpt_sovits->Infer("firefly", text);
      auto outPath = std::filesystem::current_path() / std::format("cpp_out_{}_{}.wav", n, n);
      resAudio->SaveToFile(outPath.string());
      PrintInfo("write from {}", outPath.string());
      n++;
      return true;
    });
    for (auto&text:texts) {
      sentence.Append(text);
    }
    sentence.Flush();

  } catch (const GPTSovits::Exception &e) {
    PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
  } catch (const std::exception &e) {
    PrintError("{}", e.what());
  }
}
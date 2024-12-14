//
// Created by 19254 on 24-11-10.
//
#include <GPTSovits/GPTSovits.h>
#include <filesystem>
#include <iostream>

int main() {
  try {
    auto currentPath = std::filesystem::current_path().string();
    PrintDebug("Current Path:{}", currentPath);
    auto device = GPTSovits::GetDevice("cuda:1");
    PrintDebug("device: {}", GPTSovits::DeviceToString(*device));

    auto gpt_config = GPTSovits::GPTSovitsConfig::Make("zh",
                                                       currentPath + R"(/../model_base/ssl_model.pt)");
    auto gpt_sovits = gpt_config->Build(device);

    GPTSovits::Bert::RegisterChinese(*gpt_sovits,
                                     currentPath + R"(/../model_base/bert_model.pt)",
                                     currentPath + R"(/../model_base/tokenizer.json)", true);
    GPTSovits::Bert::RegisterEnglish(*gpt_sovits, true);
    GPTSovits::Bert::RegisterJapanese(*gpt_sovits, true);

    gpt_sovits->CreateSpeaker("firefly",
                              currentPath + "/../model/gpt_sovits_model.pt",
                              currentPath + "/../model/ref.wav",
                              "虽然我也没太搞清楚状况,但他说，似乎只有直率、纯真、有童心的小孩子才能看见它.", "zh",true);

    std::vector<std::string> texts = {
//      "今天是2021年11月23日,天气:晴,气温32°C.",
//      "天下之大内有乾坤。",
//      "这个project的schedule有些问题，尤其是buffer不多。另外，cost也偏高。目前我们没法confirm手上的 resource能完全take得了。Anyway我们还是先pilot一下，再follow up最终的output，看能不能run的比较smoothly，更重要的是evaluate所有的cost能不能完全被cover掉……",
//      "The C++ support in Jupyter is powered by the xeus-cling C++ kernel.This function is used to find the length (in code points) of a UTF-8 encoded string.",
      "皆さん、我在インターネット上看到someone把几国language混在一起speak。我看到之后be like：それは我じゃないか！私もtry一tryです。\n"
      "虽然是混乱している句子ですけど、中文日本語プラスEnglish、挑戦スタート！\n"
      "我study日本語的时候，もし有汉字，我会很happy。\n"
      "Bueause中国人として、when I see汉字，すぐに那个汉字がわかります。\n"
      "But 我hate外来語、什么マクドナルド、スターバックス、グーグル、ディズニーランド、根本记不住カタカナhow to写、太難しい。\n"
      "2021年6月25日,今天32°C。以上です，byebye！\n"
    };
    gpt_sovits->ManualSeed(1733476914);
    GPTSovits::Text::Sentence sentence;
    std::shared_ptr<GPTSovits::AudioTools> outaudio;
    auto start = std::chrono::high_resolution_clock::now();
    sentence.AppendCallBack([&](const std::string &text) -> bool {
      auto resAudio = gpt_sovits->Infer("firefly", text);
//      PrintDebug("Audio Size:{}", resAudio->ReadSamples().size());
      if (!outaudio) {
        outaudio = std::make_shared<GPTSovits::AudioTools>(*resAudio);
      } else {
        outaudio->AppendEmpty(300);
        outaudio->Append(*resAudio);
      }
      return true;
    });
    for (auto &text: texts) {
      sentence.Append(text);
    }
    sentence.Flush();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    auto outPath = std::filesystem::current_path() / fmt::format("cpp_out.wav");
    outaudio->SaveToFile(outPath.string());

    std::cout << "音频写入: " << outPath << std::endl;
    std::cout << "推理耗时: " << duration.count() << " ms" << std::endl;

  } catch (const GPTSovits::Exception &e) {
    PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
  } catch (const std::exception &e) {
    PrintError("{}", e.what());
  }
}
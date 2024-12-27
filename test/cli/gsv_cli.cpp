//
// Created by 19254 on 24-11-10.
//
#include <GPTSovits/GPTSovits.h>
#include <boost/atomic/atomic.hpp>
#include <filesystem>
#include <future>
#include <iostream>
#include <thread>

/*
std::vector<std::string> texts = {
    // "今天是2021年11月23日,天气:晴,气温32°C.",
    "天下之大内有乾坤。",
    //      "这个project的schedule有些问题，尤其是buffer不多。另外，cost也偏高。目前我们没法confirm手上的 resource能完全take得了。Anyway我们还是先pilot一下，再follow up最终的output，看能不能run的比较smoothly，更重要的是evaluate所有的cost能不能完全被cover掉……",
    //      "The C++ support in Jupyter is powered by the xeus-cling C++ kernel.This function is used to find the length (in code points) of a UTF-8 encoded string.",
    // "皆さん、我在インターネット上看到someone把几国language混在一起speak。我看到之后be like：それは我じゃないか！私もtry一tryです。\n"
    // "虽然是混乱している句子ですけど、中文日本語プラスEnglish、挑戦スタート！\n"
    // "我study日本語的时候，もし有汉字，我会很happy。\n"
    // "Bueause中国人として、when I see汉字，すぐに那个汉字がわかります。\n"
    // "But 我hate外来語、什么マクドナルド、スターバックス、グーグル、ディズニーランド、根本记不住カタカナhow to写、太難しい。\n"
    // "2021年6月25日,今天32°C。以上です，byebye！\n"
  };

* */
std::unique_ptr<GPTSovits::GPTSovits> g_gpt_sovits;
std::string g_current_path;

void init_model() {
  auto device = GPTSovits::GetDevice("cuda:1");
  PrintDebug("device: {}", GPTSovits::DeviceToString(*device));

  auto gpt_config = GPTSovits::GPTSovitsConfig::Make("zh", g_current_path + R"(/../model_base/ssl_model.pt)");
  g_gpt_sovits = gpt_config->Build(device);
  g_gpt_sovits->ManualSeed(1733476914);

  GPTSovits::Bert::RegisterChinese(*g_gpt_sovits,
                                   g_current_path + R"(/../model_base/bert_model.pt)",
                                   g_current_path + R"(/../model_base/tokenizer.json)", true);
  GPTSovits::Bert::RegisterEnglish(*g_gpt_sovits,true);
  GPTSovits::Bert::RegisterJapanese(*g_gpt_sovits,true);
}

void register_speaker() {
  g_gpt_sovits->CreateSpeaker("firefly",
                              R"(G:/models/tts/gpt_sovits/firefly/tmp/gpt_sovits_model.pt)",
                              g_current_path + "/../model/ref.wav",
                              "虽然我也没太搞清楚状况,但他说，似乎只有直率、纯真、有童心的小孩子才能看见它.", "zh", true);
}

void infer_texts(const std::vector<std::string> &texts, const std::string &out_filename) {
  GPTSovits::Text::Sentence sentence;
  std::shared_ptr<GPTSovits::AudioTools> outaudio;

  sentence.AppendCallBack([&](const std::string &text) -> bool {
    auto resAudio = g_gpt_sovits->Infer("firefly", text);
    if (!outaudio) {
      outaudio = std::make_shared<GPTSovits::AudioTools>(*resAudio);
    } else {
      outaudio->AppendEmpty(300);
      outaudio->Append(*resAudio);
    }
    return true;
  });

  for (const auto &text: texts) {
    sentence.Append(text);
  }

  sentence.Flush();
  auto outPath = std::filesystem::current_path() / out_filename;
  outaudio->SaveToFile(outPath.string());
  PrintDebug("Audio Size:{}", outaudio->ReadSamples().size());
}

void test_infer_thread1(int id) {
  std::vector<std::string> texts = {
    "今天是2021年11月23日,天气:晴,气温32°C.",
    "天下之大内有乾坤。",
    "这个project的schedule有些问题，尤其是buffer不多。另外，cost也偏高。目前我们没法confirm手上的 resource能完全take得了。Anyway我们还是先pilot一下，再follow up最终的output，看能不能run的比较smoothly，更重要的是evaluate所有的cost能不能完全被cover掉……",
    "The C++ support in Jupyter is powered by the xeus-cling C++ kernel.This function is used to find the length (in code points) of a UTF-8 encoded string.",

  };
  infer_texts(texts, fmt::format("cpp_out_{}.wav", id));
}

void test_infer_thread2(int id) {
  std::vector<std::string> texts = {
    "皆さん、我在インターネット上看到someone把几国language混在一起speak。我看到之后be like：それは我じゃないか！私もtry一tryです。\n"
    "虽然是混乱している句子ですけど、中文日本語プラスEnglish、挑戦スタート！\n"
    "我study日本語的时候，もし有汉字，我会很happy。\n"
    "Bueause中国人として、when I see汉字，すぐに那个汉字がわかります。\n"
    "But 我hate外来語、什么マクドナルド、スターバックス、グーグル、ディズニーランド、根本记不住カタカナhow to写、太難しい。\n"
    "2021年6月25日,今天32°C。以上です，byebye！\n"};
  infer_texts(texts, fmt::format("cpp_out_{}.wav", id));
}

int main() {
  try {
    g_current_path = std::filesystem::current_path().string();
    PrintDebug("Current Path:{}", g_current_path);

    init_model();
    register_speaker();

    std::vector<std::function<void(int)>> threadFunctions = {test_infer_thread1, test_infer_thread2};
    std::vector<std::future<void>> futures;

    auto start_time = std::chrono::high_resolution_clock::now();

    futures.reserve(threadFunctions.size());
    for (int i = 0; i < threadFunctions.size(); ++i) {
      futures.push_back(std::async(std::launch::async, threadFunctions[i], i));
    }

    for (auto& future : futures) {
        future.get();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    PrintDebug("Total time taken: {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
    PrintDebug("All threads completed.");

    } catch (const GPTSovits::Exception &e) {
        PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
    } catch (const std::exception &e) {
        PrintError("{}", e.what());
    }
}
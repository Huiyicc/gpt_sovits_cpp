//
// Created by 19254 on 24-11-10.
//
#include <iostream>
#include <filesystem>
#include <bits/fs_fwd.h>
#include <GPTSovits/GPTSovits.h>

int main() {
  try {
    auto gpt_config = GPTSovits::GPTSovitsConfig::Make(
      R"(../test/cli/model_base/bert_model.pt)",
      R"(../test/cli/model_base/tokenizer.json)",
      R"(../test/cli/model_base/ssl_model.pt)");
    auto device = GPTSovits::GetDefaultDevice();
    PrintDebug("device: {}", device.str());
    auto gpt_sovits = gpt_config->Build(device);
    gpt_sovits->CreateSpeaker("firefly",
                              "../test/cli/model/gpt_sovits_model.pt",
                              "../test/cli/model/ref.wav",
                              "虽然我也没太搞清楚状况…但他说，似乎只有直率、纯真、有童心的小孩子才能看见它……");
    // int num_iterations = 10;
    // // 计时开始
    // auto start = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < num_iterations; ++i) {
    //   auto start_ = std::chrono::high_resolution_clock::now();
    //   gpt_sovits->Infer("firefly","叹息声一声接着一声传出，木兰对着房门织布。");
    //   // 计时结束
    //   auto end_ = std::chrono::high_resolution_clock::now();
    //   std::chrono::duration<double, std::milli> elapsed = end_ - start_;
    //   PrintInfo("第 {} 次耗时: {} ms",i,elapsed.count());
    // }
    // auto resAudio = gpt_sovits->Infer("firefly","叹息声一声接着一声传出，木兰对着房门织布。");
    // auto resAudio = gpt_sovits->Infer("firefly","今天是2021年11月23日,天气晴,气温32°C.");// BUG!!!
    auto resAudio = gpt_sovits->Infer("firefly","收到王楚钦要结婚的消息时，孙颖莎正在斯洛文尼亚的饭店里吃饭。");
    // num_iterations++;
    // 计时结束
    auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> diff = end - start;

    // double it_per_sec = num_iterations / diff.count();
    // PrintInfo("Iterations: {}",num_iterations);
    // PrintInfo("Iterations per second: {}",it_per_sec);
    // PrintInfo("Average time per iteration: {} ms",(diff.count() * 1000) / num_iterations);
    auto outPath = std::filesystem::current_path() / "cpp_out.wav";
    resAudio->SaveToFile(outPath.c_str());
    PrintInfo("write from {}",outPath.c_str());
  } catch (const GPTSovits::Exception &e) {
    PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
  } catch (const std::exception &e) {
    PrintError("{}", e.what());
  }
  return 0;
}
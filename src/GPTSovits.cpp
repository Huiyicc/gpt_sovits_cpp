//
// Created by 19254 on 24-11-10.
//
#include "GPTSovits/GPTSovits.h"
#include "GPTSovits/Text/Coding.h"
//#include <torch/torch.h>
#include <GPTSovits/G2P/g2p.h>
#include <GPTSovits/Text/TextNormalizer/zh.h>
#include <cpp-pinyin/G2pglobal.h>
#include <filesystem>
#include <torch/script.h>

#include <utility>

namespace GPTSovits {

std::filesystem::path g_globalResourcesPath = std::filesystem::current_path() / "res";

void SetGlobalResourcesPath(const std::string &path) {
  g_globalResourcesPath = path;
}

GPTSovits::GPTSovits() {}

GPTSovits::~GPTSovits() {}

std::unique_ptr<GPTSovits>
GPTSovits::Make(const std::string &lang, std::unique_ptr<torch::jit::Module> ssl,
                std::shared_ptr<TorchDevice> devices) {
  auto p = std::unique_ptr<GPTSovits>(new GPTSovits());
  p->m_ssl = std::move(ssl);
  p->m_devices = devices;
  p->m_defaultLang = lang;
  return p;
};

const std::string &GPTSovits::DefaultLang() {
  return m_defaultLang;
};

std::shared_ptr<TorchDevice> GPTSovits::Device() {
  return m_devices;
};

const GPTSovits::SpeakerInfo &
GPTSovits::CreateSpeaker(const std::string &name, const std::string &modelPath, const std::string &audioPath, const std::string &refText, const std::string &lang) {
  {
    auto iter = m_SpeakerCacheMap.find(name);
    if (iter != m_SpeakerCacheMap.end()) {
      THROW_ERRORN("说话人已存在!");
    }
  }
  torch::jit::Module *gpt_model = nullptr;
  {
    auto iter = m_SpeakerModelCacheMap.find(modelPath);
    if (iter == m_SpeakerModelCacheMap.end()) {
      PrintDebug("Load mode: {}", modelPath);
      auto tmp_model = std::make_unique<torch::jit::Module>(torch::jit::load(modelPath, *m_devices));
      if (!tmp_model) {
        THROW_ERRORN("加载模型文件失败!\nFrom:{}", modelPath);
      }
      tmp_model->to(*m_devices);
      tmp_model->eval();
      m_SpeakerModelCacheMap[modelPath] = std::move(tmp_model);
    }
    gpt_model = m_SpeakerModelCacheMap[modelPath].get();
  }

  PrintInfo("Load Model {} Done.", modelPath);
  auto audio = AudioTools::FromFile(audioPath);
  auto samples = audio->ReadSamples();
  auto tempInfo = std::make_unique<SpeakerInfo>();
  tempInfo->ModifiedTextU32 = Text::StringToU32String(refText);
  if (tempInfo->ModifiedTextU32.empty() ||
      (tempInfo->ModifiedTextU32.back() != U'。' && tempInfo->ModifiedTextU32.back() != U'.')) {
    tempInfo->ModifiedTextU32 += U".";
  }
  tempInfo->ModifiedText = Text::U32StringToString(tempInfo->ModifiedTextU32);

  //  auto new_audio_16k = audio->ReSample(16000);
  //  auto new_audio_sample_16k = new_audio_16k->ReadSamples();
  //  tempInfo->Audio16k = std::make_unique<torch::jit::IValue>(
  //    torch::from_blob(new_audio_sample_16k.data(),
  //                     {static_cast<long>(new_audio_sample_16k.size())},
  //                     torch::kFloat32)
  //      .unsqueeze(0).to(*m_devices));
  //  tempInfo->Audio32k = std::make_unique<torch::jit::IValue>(
  //    torch::from_blob(new_audio_sample_16k.data(),
  //                     {static_cast<long>(new_audio_sample_16k.size())},
  //                     torch::kFloat32)
  //      .unsqueeze(0).to(*m_devices));
  tempInfo->Audio16k = Resample(*audio, 16000);
  tempInfo->Audio32k = Resample(*audio, 32000);
  {
    torch::NoGradGuard no_grad;
    tempInfo->SSLContent = std::make_unique<torch::Tensor>(
      m_ssl->forward({*tempInfo->Audio16k}).toTensor().to(*m_devices));

    tempInfo->RefBert = G2P::GetPhoneAndBert(*this, refText,lang);
    tempInfo->GPTSovitsModel = gpt_model;
    m_SpeakerCacheMap[name] = std::move(tempInfo);
    PrintInfo("Load Speaker {} Done.", name);
    return *m_SpeakerCacheMap[name];
  }
};

GPTSovits &GPTSovits::ManualSeed(uint64_t seed) {
  torch::manual_seed(seed);
  return *this;
}

std::unique_ptr<AudioTools> GPTSovits::Infer(const std::string &speakerName, const std::string &targetText) {
  PrintDebug("start infer");
  SpeakerInfo *speaker = nullptr;
  {
    auto iter = m_SpeakerCacheMap.find(speakerName);
    if (iter == m_SpeakerCacheMap.end()) {
      THROW_ERRORN("说话人 \"{}\" 不存在", speakerName);
    }
    speaker = iter->second.get();
  };
  {
    torch::NoGradGuard no_grad;
    auto bertRes = G2P::GetPhoneAndBert(*this, targetText);
    //    bool profiling_mode = torch::jit::getProfilingMode() = false;

    //    torch::autograd::profiler::RecordProfile guard("gemfield/gemfield.pt.trace.json");
    std::vector<torch::IValue> inputs = {
      *speaker->SSLContent,
      *speaker->Audio32k,
      *speaker->RefBert->PhoneSeq,
      *bertRes->PhoneSeq,
      *speaker->RefBert->BertSeq,
      *bertRes->BertSeq,
    };
    //    {
    //      std::vector<torch::Tensor> data;
    //      for (auto&i:inputs) {
    //        data.push_back(i.toTensor());
    //      }
    //      torch::save(data, "data.pt");
    //    }
    auto start = std::chrono::high_resolution_clock::now();
    auto result_v = speaker->GPTSovitsModel->forward(inputs);

    //    PrintDebug("Profiling mode is: {}" ,(profiling_mode ? "Enabled" : "Disabled"));
    //    torch::jit::setProfilingMode(false);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    start = std::chrono::high_resolution_clock::now();
    auto result = result_v.toTensor().to(torch::kCPU);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::vector<float> audio_data(result.data_ptr<float>(), result.data_ptr<float>() + result.numel());
    return AudioTools::FromByte(audio_data, 32000);
  }
}

std::unique_ptr<torch::jit::IValue> GPTSovits::Resample(AudioTools &audio, int target_sr) {
  torch::NoGradGuard no_grad;
  auto audoData = audio.ReadSamples();
  auto refAudio = torch::from_blob(audoData.data(), {(int64_t) audoData.size()}, torch::kFloat).to(*m_devices).unsqueeze(0);
  return std::make_unique<torch::jit::IValue>(
    m_ssl->run_method("resample", refAudio, torch::IValue(static_cast<int64_t>(audio.GetHeader().SampleRate)),
                      torch::IValue(static_cast<int64_t>(target_sr))));
}

}
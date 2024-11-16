//
// Created by 19254 on 24-11-16.
//

#ifndef GPT_SOVITS_CPP_GPT_SOVITS_H
#define GPT_SOVITS_CPP_GPT_SOVITS_H

#include <torch/script.h>
#include <torch/torch.h>
#include "./AudioTools.h"
#include "./tool.h"
#include "./text.h"

namespace GPTSovits {

struct BertRes;

class CNBertModel;

class GPTSovits {
  std::unique_ptr<CNBertModel> m_zhBert;
  std::unique_ptr<torch::jit::Module> m_ssl;
  std::unique_ptr<TorchDevice> m_devices;

  GPTSovits() {};


  friend std::unique_ptr<BertRes> GetPhoneAndBert(GPTSovits &gpt, const std::string &text);

public:

  struct SpeakerInfo {
    std::u32string ModifiedTextU32;
    std::string ModifiedText;
    std::unique_ptr<torch::IValue> Audio16k;
    std::unique_ptr<torch::IValue> Audio32k;
    std::unique_ptr<torch::Tensor> SSLContent;
    torch::jit::Module *GPTSovitsModel = nullptr;
    std::unique_ptr<BertRes> RefBert;
  };

  // model path -> Module
  std::unordered_map<std::string, std::unique_ptr<torch::jit::Module>> m_SpeakerModelCacheMap;
  std::unordered_map<std::string, std::unique_ptr<SpeakerInfo>> m_SpeakerCacheMap;

  static std::unique_ptr<GPTSovits> Make(std::unique_ptr<CNBertModel> cnBertModel,
                                         std::unique_ptr<torch::jit::Module> ssl,
                                         TorchDevice &devices);

  const SpeakerInfo &CreateSpeaker(const std::string &name, const std::string &modelPath, const std::string &audioPath,
                                   const std::string &refText);

  std::unique_ptr<AudioTools> Infer(const std::string&speakerName,const std::string&targetText);

  std::unique_ptr<torch::jit::IValue> Resample(AudioTools &audio, int target_sr);
};

}

#endif //GPT_SOVITS_CPP_GPT_SOVITS_H

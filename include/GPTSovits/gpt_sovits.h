//
// Created by 19254 on 24-11-16.
//

#ifndef GPT_SOVITS_CPP_GPT_SOVITS_H
#define GPT_SOVITS_CPP_GPT_SOVITS_H

#include "./AudioTools.h"
#include <unordered_map>
#include "./tool.h"
#include <GPTSovits/G2P/g2p.h>


namespace at {
class Tensor;
}
namespace torch::jit {
struct Module;
}
namespace c10 {
struct IValue;
}


namespace GPTSovits {

struct BertRes;

class CNBertModel;

class GPTSovits {
  std::unique_ptr<torch::jit::Module> m_ssl;
  std::shared_ptr<TorchDevice> m_devices;
  std::string m_defaultLang;

  GPTSovits();


  friend class IG2P;

public:
  ~GPTSovits();

  struct SpeakerInfo {
    std::u32string ModifiedTextU32;
    std::string ModifiedText;
    std::unique_ptr<c10::IValue> Audio16k;
    std::unique_ptr<c10::IValue> Audio32k;
    std::unique_ptr<at::Tensor> SSLContent;
    torch::jit::Module *GPTSovitsModel = nullptr;
    std::shared_ptr<Bert::BertRes> RefBert;
  };

  // model path -> Module
  std::unordered_map<std::string, std::unique_ptr<torch::jit::Module>> m_SpeakerModelCacheMap;
  std::unordered_map<std::string, std::unique_ptr<SpeakerInfo>> m_SpeakerCacheMap;

  static std::unique_ptr<GPTSovits> Make(const std::string& lang,std::unique_ptr<torch::jit::Module> ssl,
                                         std::shared_ptr<TorchDevice> devices);

  std::shared_ptr<TorchDevice> Device();
  const std::string& DefaultLang();

  const SpeakerInfo &CreateSpeaker(const std::string &name, const std::string &modelPath, const std::string &audioPath,
                                   const std::string &refText);

  std::unique_ptr<AudioTools> Infer(const std::string&speakerName,const std::string&targetText);

  std::unique_ptr<c10::IValue> Resample(AudioTools &audio, int target_sr);
};

}

#endif //GPT_SOVITS_CPP_GPT_SOVITS_H

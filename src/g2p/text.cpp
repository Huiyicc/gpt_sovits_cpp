//
// Created by 19254 on 24-11-10.
//
#include <GPTSovits/GPTSovits.h>
#include <torch/torch.h>
#include <tuple>
#include <unordered_map>
#include <srell.hpp>
#include <boost/algorithm/string.hpp>
#include <GPTSovits/Text/LangDetect.h>
#include <GPTSovits/Bert/register.h>
#include <GPTSovits/G2P/g2p.h>
#include "CLD2/compact_lang_det.h"

namespace GPTSovits::G2P {

template<typename T>
std::string format_vector(const std::vector<T> &vec) {
  std::string result = "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    result += std::format("{}", vec[i]);
    if (i < vec.size() - 1) {
      result += ", ";
    }
  }
  result += "]";
  return result;
}

std::shared_ptr<Bert::BertRes> GetPhoneAndBert(GPTSovits &gpt, const std::string &text) {
  auto [isReliable, lang] = Text::LangDetect::getInstance()->Detect(text);
  if (!isReliable) {
    lang = gpt.DefaultLang();
  }
  auto detects = Text::LangDetect::getInstance()->DetectSplit(lang, text);
  std::vector<at::Tensor> PhoneSeqs;
  std::vector<at::Tensor> BertSeqs;
  for (auto &detectText: detects) {
    auto g2p = MakeFromLang(detectText.language);
    auto g2pRes = g2p->CleanText(detectText.sentence);
    auto bertIter = Bert::g_bert_map.find(detectText.language);
    if (bertIter == Bert::g_bert_map.end()) {
      PrintError("No Bert Model for {}\nSentence: {}", detectText.language, detectText.sentence);
    }
    auto encodeResult = bertIter->second->Encode(g2pRes);
    PhoneSeqs.emplace_back(std::move(*encodeResult.PhoneSeq));
    BertSeqs.emplace_back(std::move(*encodeResult.BertSeq));
  }
  return std::make_shared<Bert::BertRes>(
    std::make_shared<torch::Tensor>(torch::cat({PhoneSeqs}, 1).to(*gpt.Device())),
    std::make_shared<at::Tensor>(torch::cat({BertSeqs}, 0).to(*gpt.Device()))
  );
}


}
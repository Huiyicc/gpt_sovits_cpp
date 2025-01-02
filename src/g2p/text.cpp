//
// Created by 19254 on 24-11-10.
//
#include <GPTSovits/GPTSovits.h>
//#include <torch/torch.h>
#include "CLD2/compact_lang_det.h"
#include <GPTSovits/Bert/register.h>
#include <GPTSovits/G2P/g2p.h>
#include <GPTSovits/Text/LangDetect.h>
#include <boost/algorithm/string/trim.hpp>
#include <fmt/format.h>
#include <torch/script.h>
#include <tuple>

namespace GPTSovits::G2P {


std::string format_vector(const std::vector<Text::LangDetect::LanguageSentence> &vec) {
  std::string result = "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    result += fmt::format("[{}]{}", vec[i].language, vec[i].sentence);
    if (i < vec.size() - 1) {
      result += ", ";
    }
  }
  result += "]";
  return result;
}

std::shared_ptr<Bert::BertRes> GetPhoneAndBert(GPTSovits &gpt, const std::string &text, const std::string &lang) {
  auto htext = boost::trim_copy(text);
  auto [isReliable, de_lang] = Text::LangDetect::getInstance()->Detect(htext);
  if (!isReliable) {
    de_lang = lang.empty() ? gpt.DefaultLang() : lang;
  }
  auto detects = Text::LangDetect::getInstance()->DetectSplit(de_lang, htext);
  // PrintDebug("[is reliable:{}]detect str:{} -> {}",isReliable,text, format_vector(detects));
  std::vector<at::Tensor> PhoneSeqs;
  std::vector<at::Tensor> BertSeqs;
  for (auto &detectText: detects) {
    auto g2p = MakeFromLang(detectText.language);
    auto g2pRes = g2p->CleanText(detectText.sentence);
    auto bert = Bert::MakeFromLang(detectText.language);
    if (!bert) {
      PrintError("No Bert Model for {}\nSentence: {}", detectText.language, detectText.sentence);
    }
    auto encodeResult = (*bert)->Encode(g2pRes);
    PhoneSeqs.emplace_back(std::move(*encodeResult.PhoneSeq));
    BertSeqs.emplace_back(std::move(*encodeResult.BertSeq));
  }
  return std::make_shared<Bert::BertRes>(Bert::BertRes{
    std::make_shared<torch::Tensor>(torch::cat({PhoneSeqs}, 1).to(*gpt.Device())),
    std::make_shared<at::Tensor>(torch::cat({BertSeqs}, 0).to(*gpt.Device()))});
}


}// namespace GPTSovits::G2P
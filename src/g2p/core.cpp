//
// Created by 19254 on 24-12-1.
//
//#include <torch/torch.h>

#include <GPTSovits/G2P/g2p.h>
#include <GPTSovits/G2P/g2p_zh.h>
#include <GPTSovits/G2P/g2p_en.h>
#include <GPTSovits/G2P/g2p_ja.h>
#include <GPTSovits/Utils/exception.h>
#include <GPTSovits/Text/Utils.h>
#include "GPTSovits/G2P/symbols.h"

namespace GPTSovits::G2P {

std::vector<int> cleaned_text_to_sequence(const std::vector<std::string> &phones) {
  std::vector<int> res;
  for (auto &phone: phones) {
    auto it = g_Symbols.find(phone);
    if (it != g_Symbols.end()) {
      res.push_back(it->second);
    }
  }
  return res;
}

G2PRes IG2P::CleanText(const std::string &text) {
//  auto input = text;
//  if (!Text::FindEnd(input, {".", ",", "?", "!", ";", "。", "，", "？", "！", ";"})) {
//    input += ".";
//  }
  auto res = _cleanText(text);
  for (auto &ph: res.phones) {
    // 检查 ph 是否在 symbols 中
    if (g_Symbols.find(ph) == g_Symbols.end()) {
      ph = "UNK";
    }
  }
  res.phone_ids = cleaned_text_to_sequence(res.phones);
  return res;
}

std::shared_ptr<IG2P> MakeFromLang(const std::string &lang) {
  if (lang == "zh") {
    return std::make_shared<G2PZH>();
  } else if (lang == "en") {
    return std::make_shared<G2PEN>();
  } else if (lang == "jp"||lang == "ja") {
    return std::make_shared<G2PJA>();
  }
  THROW_ERRORN("Not support language {}", lang);
}


}

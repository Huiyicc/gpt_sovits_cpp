//
// Created by 19254 on 24-11-27.
//
#include <iostream>
#include <fstream>
#include <tokenizers_cpp.h>
#include "GPTSovits/GPTSovits.h"

int main() {
  std::vector<GPTSovits::Text::Sentence::SentenceSplitMethod> ms = {
    GPTSovits::Text::Sentence::SentenceSplitMethod::Length,
    GPTSovits::Text::Sentence::SentenceSplitMethod::StrictPunctuation,
    GPTSovits::Text::Sentence::SentenceSplitMethod::Punctuation
  };
  for (auto m: ms) {
    std::vector<std::string> test_strs = {
      "皆さん、我在インターネット上看到someone把几国language混在一起speak。我看到之后be like：それは我じゃないか！私もtry一tryです。\n"
      "虽然是混乱している句子ですけど、中文日本語プラスEnglish、挑戦スタート！\n"
      "我study日本語的时候，もし有汉字，我会很happy。\n"
      "Bueause中国人として、when I see汉字，すぐに那个汉字がわかります。\n"
      "But 我hate外来語、什么マクドナルド、スターバックス、グーグル、ディズニーランド、根本记不住カタカナhow to写、太難しい。\n"
      "2021年6月25日,今天32°C。以上です，byebye！\n"
    };

    GPTSovits::Text::Sentence sentence(m);
    sentence.AppendCallBack([](const std::string &sentence) -> bool {
      PrintDebug("sentence:{}", sentence);
      auto detects = GPTSovits::Text::LangDetect::getInstance()->DetectSplit("zh", sentence);
      for (auto &detectText: detects) {
        PrintDebug("{} | {}", detectText.language, detectText.sentence);
      }
      return true;
    });

    for (auto &text: test_strs) {
      // 一定要确保字符是完整的,一次添加20字符,每50字符触发一次callback
      auto u32t = GPTSovits::Text::StringToU32String(text);
      int index = 0;
      int len = 20;

      while (index < u32t.size()) {
        sentence.Append(GPTSovits::Text::U32StringToString(u32t.substr(index, len)));
        index += len;
      }
      sentence.Flush();
    }
    PrintDebug("===========\n");
  }


  return 0;
}
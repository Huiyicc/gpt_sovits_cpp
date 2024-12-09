//
// Created by 19254 on 24-11-10.
//
#include <iostream>
#include "GPTSovits/GPTSovits.h"

template <typename T>
std::string format_vector(const std::vector<T>& vec) {
  std::string result = "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    result += fmt::format("{}", vec[i]);
    if (i < vec.size() - 1) {
      result += ", ";
    }
  }
  result += "]";
  return result;
}

// 切分中英混合
void spl_text() {

}

int main() {
  try {
    std::vector<std::string> test_strs = {
      "皆さん、我在インターネット上看到someone把几国language混在一起speak。我看到之后be like：それは我じゃないか！私もtry一tryです。\n"
      "虽然是混乱している句子ですけど、中文日本語プラスEnglish、挑戦スタート！\n"
      "我study日本語的时候，もし有汉字，我会很happy。\n"
      "Bueause中国人として、when I see汉字，すぐに那个汉字がわかります。\n"
      "But 我hate外来語、什么マクドナルド、スターバックス、グーグル、ディズニーランド、根本记不住カタカナhow to写、太難しい。\n"
      "2021年6月25日,今天32°C。以上です，byebye！\n"
      "我似乎会随긴긴시着时间的流逝,간이렇게보내다간말라죽을,最后枯萎거같아서死去"
    };
//    std::vector<std::string> test_strs = {
////      "This function is used to find the length (in code points) of a UTF-8 encoded string. The reason it is called distance, rather than, say, length is mainly because developers are used that length is an O(1) function. Computing the length of an UTF-8 string is a linear operation, and it looked better to model it after std::distance algorithm."
////    "在表达式中非法使用命名空间标识符表率。",
//    "表率"
//    };

    GPTSovits::Text::Sentence sentence;
    sentence.AppendCallBack([](const std::string &sentence) -> bool {
      PrintDebug("sentence:{}", sentence);
      auto detects = GPTSovits::Text::LangDetect::getInstance()->DetectSplit("zh", sentence);
      for (auto &detectText: detects) {
        auto g2p = GPTSovits::G2P::MakeFromLang("zh");
        auto res = g2p->CleanText(detectText.sentence);
//        PrintInfo("原始文本:{}", res.norm_text);
//        PrintInfo("phones:{}",format_vector(res.phones));
//        PrintInfo("word2ph:{}", format_vector(res.word2ph));

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


//    std::vector<std::string> test_strs = {
//      "你好%啊啊啊额妈妈hello、还是到付红四方。\n2021年6月25日,今天32°C",
//      "叹息声一声接着一声传出，木兰对着房门织布。",
//      "今天是2021年11月23日,天气晴,气温32°C.",
//    };

//    for (auto&text :test_strs) {
//      auto res = GPTSovits::CleanText(text);
//      PrintInfo("原始文本:{}", res.norm_text);
//      PrintInfo("phones:{}",format_vector(res.phones));
//      PrintInfo("word2ph:{}", format_vector(res.word2ph));
//    }

  } catch (const GPTSovits::Exception &e) {
    PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
  }
  return 0;
}
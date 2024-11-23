//
// Created by 19254 on 24-11-10.
//
#include <iostream>
#include <format>
#include "GPTSovits/GPTSovits.h"

template <typename T>
std::string format_vector(const std::vector<T>& vec) {
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


int main() {
  try {
    std::vector<std::string> test_strs = {
      "你好%啊啊啊额妈妈hello、还是到付红四方。\n2021年6月25日,今天32°C",
      "叹息声一声接着一声传出，木兰对着房门织布。",
      "今天是2021年11月23日,天气晴,气温32°C.",
    };

    for (auto&text :test_strs) {
      auto res = GPTSovits::CleanText(text);
      PrintInfo("原始文本:{}", res.norm_text);
      PrintInfo("phones:{}",format_vector(res.phones));
      PrintInfo("word2ph:{}", format_vector(res.word2ph));
    }

  } catch (const GPTSovits::Exception &e) {
    PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
  }
  return 0;
}
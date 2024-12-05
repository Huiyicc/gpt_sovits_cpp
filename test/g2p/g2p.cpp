//
// Created by 19254 on 24-12-1.
//
#include <iostream>
#include <format>
#include <map>
#include <filesystem>
#include "GPTSovits/GPTSovits.h"

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


int main() {
  try {
    std::map<std::string, std::vector<std::string>> test_strs = {
      {
        "en", {
        "The C++ support in Jupyter is powered by the xeus-cling C++ kernel. Together with xeus-cling, xtensor enables a similar workflow to that of NumPy with the IPython Jupyter kernel.",
        "This 2432324  25th encoded string.This function is used to find the length (in code points) of a UTF-8 encoded string. The reason it is called distance, rather than, say, length is mainly because developers are used that length is an O(1) function. Computing the length of an UTF-8 string is a linear operation, and it looked better to model it after std::distance algorithm.",
      }
      },
    };

    for (auto &[lang, texts]: test_strs) {
      for (auto &text: texts) {
        auto s = GPTSovits::G2P::MakeFromLang(lang);
        auto res = s->CleanText(text);
        PrintDebug("norm_text:{}",res.norm_text);
        PrintDebug("phones:{}",format_vector(res.phones));
        PrintDebug("phone_ids:{}",format_vector(res.phone_ids));
        PrintDebug("word2ph:{}",format_vector(res.word2ph));
      }
    }

  } catch (const GPTSovits::Exception &e) {
    PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
  }
  return 0;
}
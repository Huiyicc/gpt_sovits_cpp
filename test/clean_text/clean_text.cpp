//
// Created by 19254 on 24-11-10.
//
#include <iostream>
#include "GPTSovits/GPTSovits.h"

int main() {
  try {
    GPTSovits::CleanText("你好%啊啊啊额妈妈hello、还是到付红四方。\n2021年6月25日,今天32°C");
    GPTSovits::CleanText("2021年6月25日");
//    std::cout << GPTSovits::CleanText("你好") << std::endl;


  } catch (const GPTSovits::Exception &e) {
    PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
  }
  return 0;
}
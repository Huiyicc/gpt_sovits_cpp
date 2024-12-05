//
// Created by 19254 on 24-12-4.
//
#include <GPTSovits/Bert/register.h>
#include <GPTSovits/Utils/exception.h>

namespace GPTSovits::Bert {

std::map<std::string, std::shared_ptr<IBert>> g_bert_map;

BertRes::~BertRes() {};

std::shared_ptr<IBert> MakeFromLang(const std::string &lang) {
  auto iter = g_bert_map.find(lang);
  if (iter == g_bert_map.end()) {
    THROW_ERRORN("No such language {}", lang);
  }
  return iter->second;
}

}

//
// Created by 19254 on 25-1-2.
//
#include "global.h"

void gsv_bert_register_chinese(struct gsv_cli *gsv, const char *model_path, const char *tokenizer_path, bool warm_up) {
  GPTSovits::Bert::RegisterChinese(*gsv->gsv, model_path, tokenizer_path, warm_up);
}

void gsv_bert_register_english(struct gsv_cli *gsv, bool warm_up) {
  GPTSovits::Bert::RegisterEnglish(*gsv->gsv, warm_up);
};

void gsv_bert_register_japanese(struct gsv_cli *gsv, bool warm_up) {
  GPTSovits::Bert::RegisterJapanese(*gsv->gsv, warm_up);
}

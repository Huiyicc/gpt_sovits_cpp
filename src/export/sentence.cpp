//
// Created by 19254 on 25-1-6.
//
#include "./global.h"

struct gsv_sentence *APICALL gsv_sentence_create(SentenceSplitMethod cutMethod, int maxLength) {
  auto res = new gsv_sentence{std::make_unique<GPTSovits::Text::Sentence>(static_cast<GPTSovits::Text::Sentence::SentenceSplitMethod>(cutMethod), maxLength)};
  return res;
}

void APICALL gsv_sentence_free(struct gsv_sentence *sentence_cli) {
  delete sentence_cli;
};

void APICALL gsv_sentence_set_append_callback(struct gsv_sentence *sentence_cli, GSVSentenceAppendCallbackFunction callback, void *user_data) {
  sentence_cli->sentence->AppendCallBack([=](const std::string &s) -> bool {
    return callback(s.c_str(), user_data);
  });
}

void APICALL gsv_sentence_append(struct gsv_sentence * sentence_cli, const char* sentence) {
  sentence_cli->sentence->Append(sentence);
}

void APICALL gsv_sentence_flush(struct gsv_sentence * sentence_cli) {
  sentence_cli->sentence->Flush();
}



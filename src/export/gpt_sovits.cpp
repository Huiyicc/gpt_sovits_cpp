//
// Created by 19254 on 25-1-2.
//
#include "./global.h"
#include <GPTSovits/GPTSovits.h>

struct gsv_cli *APICALL gsv_cli_create(struct device_info *device, const char *default_lang, const char *ssl_path_path) {
  auto gpt_config = GPTSovits::GPTSovitsConfig::Make(default_lang, ssl_path_path);
  auto result = new gsv_cli{gpt_config->Build(device->device)};
  return result;
}

void APICALL gsv_cli_free(struct gsv_cli *gsv) {
  delete gsv;
};
void APICALL gsv_cli_manual_seed(struct gsv_cli *gsv, int64_t seed) {
  gsv->gsv->ManualSeed(seed);
};

bool APICALL gsv_cli_create_speaker(struct gsv_cli *gsv,
                                    const char *speaker_name, const char *model_path,
                                    const char *audio_path, const char *ref_text,
                                    const char *lang, bool warm_up) {
  try {
    gsv->gsv->CreateSpeaker(speaker_name, model_path, audio_path, ref_text, lang, warm_up);
    return true;
  } catch (std::exception &e) {
    g_last_error = e.what();
  }
  return false;
}

struct gsv_audio *APICALL gsv_cli_infer(struct gsv_cli *gsv, const char *speaker_name, const char *target_text) {
  try {
    auto r = new gsv_audio{gsv->gsv->Infer(speaker_name, target_text)};
    return r;
  } catch (std::exception &e) {
    g_last_error = e.what();
  }
  return nullptr;
}
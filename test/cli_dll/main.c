//
// Created by 19254 on 24-12-28.
//
#include <GPTSovits/GPTSovits_c.h>
#include <stdio.h>

int main() {
  struct device_info *device = gsv_device_create("cuda:1");
  printf("%s\n", gsv_device_get_name(device));
  struct gsv_cli *gsv = gsv_cli_create(device, "zh", "G:\\models\\tts\\gpt_sovits\\base\\ssl_model.pt");
  printf("register bert langs\n");
  gsv_bert_register_chinese(gsv, "G:\\models\\tts\\gpt_sovits\\base\\bert_model.pt",
                            "G:\\models\\tts\\gpt_sovits\\base\\tokenizer.json",
                            true);
  gsv_bert_register_english(gsv, true);
  gsv_bert_register_japanese(gsv, true);

  printf("register speaker\n");
  if (!gsv_cli_create_speaker(gsv,
                              "firefly", "G:\\models\\tts\\gpt_sovits\\firefly\\241105\\gpt_sovits_model.pt",
                             "G:\\models\\tts\\gpt_sovits\\firefly\\241105\\ref.wav",
                             "虽然我也没太搞清楚状况…但他说，似乎只有直率、纯真、有童心的小孩子才能看见它……",
                             "zh",
                             true)) {
    printf("register speaker failed: %s\n", gsv_get_last_error());
    exit(1);
  };
  gsv_cli_manual_seed(gsv, 1733476914);

  struct gsv_audio* res_audio = gsv_cli_infer(gsv, "firefly", "早上好呀~今天天气真不错！");
  if (!res_audio) {
    printf("infer failed: %s\n", gsv_get_last_error());
    exit(2);
  }
  gsv_audio_save_to_file(res_audio, "output.wav");
  gsv_audio_free(res_audio);

  printf("free cli\n");
  // 记得清理
  gsv_cli_free(gsv);
  gsv_device_free(device);

  return 0;
}
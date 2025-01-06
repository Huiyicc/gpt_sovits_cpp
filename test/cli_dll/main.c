#include <GPTSovits/GPTSovits_c.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifdef _WIN32
typedef HANDLE thread_t;
#else
typedef pthread_t thread_t;
#endif

#ifdef _WIN32
typedef DWORD WINAPI thread_func_t(LPVOID);
#else
typedef void *(*thread_func_t)(void *);
#endif

#ifdef _WIN32
#define CREATE_THREAD(thread, func, arg)                                                     \
  do {                                                                                       \
    DWORD threadId;                                                                          \
    *(thread) = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) (func), (arg), 0, &threadId); \
  } while (0)
#else
#define CREATE_THREAD(thread, func, arg) \
  pthread_create((thread), NULL, (func), (arg))
#endif

#ifdef _WIN32
#define JOIN_THREAD(thread)                  \
  do {                                       \
    WaitForSingleObject((thread), INFINITE); \
    CloseHandle(thread);                     \
  } while (0)
#else
#define JOIN_THREAD(thread) \
  pthread_join((thread), NULL)
#endif

#ifdef _WIN32
#define DEF_THREAD_FUNC(FUNCNAME, code)   \
  DWORD WINAPI FUNCNAME(LPVOID lpParam) { \
    code;                                 \
    return 0;                             \
  }
#else
#define DEF_THREAD_FUNC(FUNCNAME, code) \
  void *FUNCNAME(void *arg) {           \
    code;                               \
    return NULL;                        \
  }
#endif

struct device_info *g_device;
struct gsv_cli *g_gsv;

DEF_THREAD_FUNC(loading_model, {
  g_device = gsv_device_create("cuda:0");
  printf("%s\n", gsv_device_get_name(g_device));
  g_gsv = gsv_cli_create(g_device, "zh", "G:\\models\\tts\\gpt_sovits\\firefly\\tmp\\ssl_model.pt");
  printf("register bert langs\n");
  gsv_bert_register_chinese(g_gsv, "G:\\models\\tts\\gpt_sovits\\firefly\\tmp\\bert_model.pt",
                            "G:\\models\\tts\\gpt_sovits\\base\\tokenizer.json",
                            true);
  gsv_bert_register_english(g_gsv, true);
  gsv_bert_register_japanese(g_gsv, true);

  printf("register speaker\n");
  if (!gsv_cli_create_speaker(g_gsv,
                              "firefly", "G:\\models\\tts\\gpt_sovits\\firefly\\tmp\\gpt_sovits_model.pt",
                              "G:\\models\\tts\\gpt_sovits\\firefly\\241105\\ref.wav",
                              "虽然我也没太搞清楚状况…但他说，似乎只有直率、纯真、有童心的小孩子才能看见它……",
                              "zh",
                              true)) {
    printf("register speaker failed: %s\n", gsv_get_last_error());
    exit(1);
  };
  gsv_cli_manual_seed(g_gsv, 1733476914);

  printf("load done!");
});

char g_text[2048];

bool APICALL gsv_sentence_append_callback(const char *text, void *user_data) {
  struct gsv_audio *res_audio = gsv_cli_infer(g_gsv, "firefly", text);
  if (!res_audio) {
    printf("infer failed: %s\n", gsv_get_last_error());
    exit(2);
  }
  gsv_audio_append(user_data, res_audio);
  gsv_audio_free(res_audio);
  return true;
}

DEF_THREAD_FUNC(infer_func, {
  printf("infer start!\n");
  struct gsv_sentence *sentence = gsv_sentence_create(Punctuation, 50);
  struct gsv_audio *res_audio = gsv_audio_from_empty(32000, 1, SF_FORMAT_WAV | SF_FORMAT_PCM_16);
  gsv_sentence_set_append_callback(sentence, gsv_sentence_append_callback, res_audio);
  gsv_sentence_append(sentence, g_text);
  gsv_sentence_flush(sentence);
  gsv_audio_save_to_file(res_audio, "output.wav");
  gsv_sentence_free(sentence);
  gsv_audio_free(res_audio);
  printf("infer end!\n");
  printf("\n");
});

int main() {
  {
    thread_t load_module_thread;
    CREATE_THREAD(&load_module_thread, loading_model, NULL);
    JOIN_THREAD(load_module_thread);
  }


  while (true) {
    printf("input text: ");
    scanf("%s", g_text);
    if (strcmp(g_text, "exit") == 0) {
      break;
    }
    {
      thread_t inter_thread;
      CREATE_THREAD(&inter_thread, infer_func, NULL);
    }
  }

  printf("free cli\n");
  // 记得清理
  gsv_cli_free(g_gsv);
  gsv_device_free(g_device);

  return 0;
}
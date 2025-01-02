//
// Created by 19254 on 25-1-2.
//
#include "./global.h"

void APICALL gsv_audio_free(struct gsv_audio* audio) {
    delete audio;
};

void APICALL gsv_audio_save_to_file(struct gsv_audio* audio, const char* path) {
  audio->audio->SaveToFile(path);
}

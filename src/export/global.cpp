//
// Created by 19254 on 25-1-1.
//
#include "./global.h"
#include "GPTSovits/GPTSovits_c.h"

std::string g_last_error;

void gsv_set_last_error(const char *error) {
  g_last_error = error;
}

const char *gsv_get_last_error() {
  return g_last_error.c_str();
}

void gsv_global_resources_path_set(const char *path) {
  GPTSovits::SetGlobalResourcesPath(path);
}
//
// Created by 19254 on 24-12-28.
//
#include "./global.h"


struct device_info * APICALL gsv_device_create(const char *device) {
  try {
    auto out_ptr = new device_info{GPTSovits::GetDevice(device)};
    out_ptr->name = device;
    return out_ptr;
  } catch (const std::exception &e) {
    g_last_error = e.what();
    return nullptr;
  }
};

const char *APICALL gsv_device_get_name(const struct device_info *device) {
  return device->name.c_str();
};

void APICALL gsv_device_free(struct device_info* device) {
  delete device;
};
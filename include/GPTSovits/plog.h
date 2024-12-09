//
// Created by 19254 on 24-11-6.
//

#ifndef TORCH_TEST_PLOG_H
#define TORCH_TEST_PLOG_H

#include <chrono>
#include <fmt/format.h>
#include <iostream>

#ifdef _WIN32
#define LOCALTIME(time_str)                                                          \
  auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); \
  std::tm tm_info = {0};                                                             \
  localtime_s(&tm_info, &now);                                                       \
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);
#else
#define LOCALTIME(time_str)    \
  auto now = time(nullptr);    \
  struct tm tm_info = {0};     \
  localtime_r(&now, &tm_info); \
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);

#endif
#ifdef _HOST_ANDROID_
#include <android/log.h>

#define ___PLOG_COUTI__(COUT_CODE) __android_log_print(ANDROID_LOG_INFO, "GPTSovits C++", "%s", fmt::format(COUT_CODE).c_str())
#define ___PLOG_COUTD__(COUT_CODE) __android_log_print(ANDROID_LOG_DEBUG, "GPTSovits C++", "%s", fmt::format(COUT_CODE).c_str())
#define ___PLOG_COUTE__(COUT_CODE) __android_log_print(ANDROID_LOG_ERROR, "GPTSovits C++", "%s", fmt::format(COUT_CODE).c_str())
#else
#define ___PLOG_COUTI__(COUT_CODE) std::cout << COUT_CODE << std::endl;
#define ___PLOG_COUTD__(COUT_CODE) std::cout << COUT_CODE << std::endl;
#define ___PLOG_COUTE__(COUT_CODE) std::cerr << COUT_CODE << std::endl;
#endif

#define PrintInfo(fstr, ...)                                                                                                                          \
  do {                                                                                                                                                \
    char time_str[32];                                                                                                                                \
    LOCALTIME(time_str);                                                                                                                              \
    std::string_view __TMP__fPath__ = __FILE__;                                                                                                       \
    __TMP__fPath__ = __TMP__fPath__.substr(strlen(CPPMODULE_PROJECT_ROOT_PATH) + 1, __TMP__fPath__.size() - strlen(CPPMODULE_PROJECT_ROOT_PATH) - 1); \
    auto __TMP__lstr__ = fmt::format("<info> <{}> [{}:{}] ", time_str, __TMP__fPath__, __LINE__);                                                     \
    auto __TMP__ustr__ = fmt::format(fstr, ##__VA_ARGS__);                                                                                            \
    ___PLOG_COUTI__(fmt::format("{}{}", __TMP__lstr__, __TMP__ustr__));                                                                               \
  } while (0)

#define PrintDebug(fstr, ...)                                                                                                          \
  do {                                                                                                                                 \
    char time_str[32];                                                                                                                 \
    LOCALTIME(time_str);                                                                                                               \
    std::string_view TMP_fPath = __FILE__;                                                                                             \
    TMP_fPath = TMP_fPath.substr(strlen(CPPMODULE_PROJECT_ROOT_PATH) + 1, TMP_fPath.size() - strlen(CPPMODULE_PROJECT_ROOT_PATH) - 1); \
    auto TMP_lstr = fmt::format("<debug> <{}> [{}:{}] ", time_str, TMP_fPath, __LINE__);                                               \
    auto TMP_ustr = fmt::format(fstr, ##__VA_ARGS__);                                                                                  \
    ___PLOG_COUTD__(fmt::format("{}{}", TMP_lstr, TMP_ustr));                                                                          \
  } while (0)

#define PrintError(fstr, ...)                                                                                                                         \
  do {                                                                                                                                                \
    char time_str[32];                                                                                                                                \
    LOCALTIME(time_str);                                                                                                                              \
    std::string_view __TMP__fPath__ = __FILE__;                                                                                                       \
    __TMP__fPath__ = __TMP__fPath__.substr(strlen(CPPMODULE_PROJECT_ROOT_PATH) + 1, __TMP__fPath__.size() - strlen(CPPMODULE_PROJECT_ROOT_PATH) - 1); \
    auto __TMP__lstr__ = fmt::format("<error> <{}> [{}:{}] ", time_str, __TMP__fPath__, __LINE__);                                                    \
    auto __TMP__ustr__ = fmt::format(fstr, ##__VA_ARGS__);                                                                                            \
    ___PLOG_COUTE__(fmt::format("{}{}", __TMP__lstr__, __TMP__ustr__));                                                                               \
  } while (0)


#endif//TORCH_TEST_PLOG_H

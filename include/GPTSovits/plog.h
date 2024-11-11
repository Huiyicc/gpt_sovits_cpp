//
// Created by 19254 on 24-11-6.
//

#ifndef TORCH_TEST_PLOG_H
#define TORCH_TEST_PLOG_H

#include <iostream>
#include <chrono>
#include <format>

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

#define PrintInfo(fstr, ...)                                                                                      \
  do {                                                                                                            \
    char time_str[32];                                                                                            \
    LOCALTIME(time_str);                                                                                          \
    std::string_view __TMP__fPath__ = __FILE__;                                                                   \
    __TMP__fPath__ = __TMP__fPath__.substr(strlen(CPPMODULE_PROJECT_ROOT_PATH)+1, __TMP__fPath__.size() - strlen(CPPMODULE_PROJECT_ROOT_PATH)-1); \
    auto __TMP__lstr__ = std::format("<info> <{}> [{}:{}] ", time_str, __TMP__fPath__, __LINE__);                  \
    auto __TMP__ustr__ = std::format(fstr, ##__VA_ARGS__);                                                        \
    std::cout << std::format("{}{}", __TMP__lstr__, __TMP__ustr__) << std::endl;                                  \
  } while (0)

#define PrintDebug(fstr, ...)                                                                                     \
  do {                                                                                                            \
    char time_str[32];                                                                                            \
    LOCALTIME(time_str);                                                                                          \
    std::string_view TMP_fPath = __FILE__;                                                                   \
    TMP_fPath = TMP_fPath.substr(strlen(CPPMODULE_PROJECT_ROOT_PATH)+1, TMP_fPath.size() - strlen(CPPMODULE_PROJECT_ROOT_PATH)-1); \
    auto TMP_lstr = std::format("<debug> <{}> [{}:{}] ", time_str, TMP_fPath, __LINE__);                  \
    auto TMP_ustr = std::format(fstr, ##__VA_ARGS__);                                                        \
    std::cout << std::format("{}{}", TMP_lstr, TMP_ustr) << std::endl;                                  \
  } while (0)

#define PrintError(fstr, ...)                                                                                              \
  do {                                                                                                            \
    char time_str[32];                                                                                            \
    LOCALTIME(time_str);                                                                                          \
    std::string_view __TMP__fPath__ = __FILE__;                                                                   \
    __TMP__fPath__ = __TMP__fPath__.substr(strlen(CPPMODULE_PROJECT_ROOT_PATH)+1, __TMP__fPath__.size() - strlen(CPPMODULE_PROJECT_ROOT_PATH)-1); \
    auto __TMP__lstr__ = std::format("<error> <{}> [{}:{}] ", time_str, __TMP__fPath__, __LINE__);                  \
    auto __TMP__ustr__ = std::format(fstr, ##__VA_ARGS__);                                                        \
    std::cerr << std::format("{}{}", __TMP__lstr__, __TMP__ustr__) << std::endl;                                  \
  } while (0)


#endif //TORCH_TEST_PLOG_H

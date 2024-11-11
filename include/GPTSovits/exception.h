//
// Created by 19254 on 24-9-24.
//

#ifndef _GPTSOVITS_EXCEPTION_H_
#define _GPTSOVITS_EXCEPTION_H_

#include <exception>
#include <sstream>
#include <string>
#include <utility>
#include <format>

namespace GPTSovits {

class Exception : public std::exception {
public:
  Exception(const std::string &message, int errorCode, const char *file, int line)
    : message_(message), errorCode_(errorCode), file_(file), line_(line) {
    std::ostringstream oss;
    oss << "Error: " << message << " (Code: " << errorCode << ") in " << file << " at line " << line;
    fullMessage_ = oss.str();
  }

  // 支持复制构造函数
  Exception(const Exception &other)
    : message_(other.message_), errorCode_(other.errorCode_), file_(other.file_), line_(other.line_), fullMessage_(other.fullMessage_) {}

  // 支持移动构造函数
  Exception(Exception &&other) noexcept
    : message_(std::move(other.message_)), errorCode_(other.errorCode_), file_(other.file_), line_(other.line_), fullMessage_(std::move(other.fullMessage_)) {}

  // 覆盖what()方法
  const char *what() const noexcept override {
    return fullMessage_.c_str();
  }

  // 获取错误码
  int getErrorCode() const noexcept {
    return errorCode_;
  }

  // 获取文件名
  const char *getFile() const noexcept {
    return file_;
  }

  // 获取行号
  int getLine() const noexcept {
    return line_;
  }

private:
  std::string message_;
  int errorCode_;
  const char *file_;
  int line_;
  std::string fullMessage_;
};

};

// 辅助宏，用于生成包含文件名和行号的异常
#define THROW_ERROR(message,...) throw GPTSovits::Exception(std::format(message,##__VA_ARGS__), 0, __FILE__, __LINE__)
#define THROW_ERROR_CODE(message, errorCode) throw GPTSovits::Exception(std::format(message,##__VA_ARGS__), errorCode, __FILE__, __LINE__)
#define THROW_ERRORN(message,...) throw Exception(std::format(message,##__VA_ARGS__), 0, __FILE__, __LINE__)
#define THROW_ERROR_CODEN(message, errorCode) throw Exception(std::format(message,##__VA_ARGS__), errorCode, __FILE__, __LINE__)


#endif//_GPTSOVITS_EXCEPTION_H_

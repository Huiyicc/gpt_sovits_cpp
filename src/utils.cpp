//
// Created by 19254 on 24-11-17.
//
#include "GPTSovits/utils.h"
#include <cctype>

namespace GPTSovits {
bool safe_isalpha(int c) {
  return (c >= 0 && c <= 255) && std::isalpha(static_cast<unsigned char>(c));
}

bool safe_isdigit(int c) {
  return (c >= 0 && c <= 255) && std::isdigit(static_cast<unsigned char>(c));
}

// Helper function to check if a character is a space
bool safe_isspace(char32_t ch) {
  return ch == U' ' || ch == U'\t' || ch == U'\n' || ch == U'\r' || ch == U'\f' || ch == U'\v';
}

// Function to trim spaces from both ends of a u32string
std::u32string U32trim(const std::u32string& str) {
  size_t first = 0;
  size_t last = str.size();

  // Find the first non-space character from the left
  while (first < last && safe_isspace(str[first])) {
    ++first;
  }

  // Find the first non-space character from the right
  while (last > first && safe_isspace(str[last - 1])) {
    --last;
  }

  // Return the trimmed substring
  return str.substr(first, last - first);
}

}
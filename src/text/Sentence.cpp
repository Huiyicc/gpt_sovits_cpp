//
// Created by 19254 on 24-11-29.
//
#include <GPTSovits/Text/Sentence.h>
#include <GPTSovits/Text/Coding.h>
#include <boost/algorithm/string.hpp>
#include "GPTSovits/plog.h"
#include "GPTSovits/Text/Utils.h"

namespace GPTSovits::Text {

Sentence::Sentence(SentenceSplitMethod cutMethod, int maxLength)
  : m_splitMethod(cutMethod), m_maxLength(maxLength) {
}

Sentence &Sentence::Flush() {
  if (!m_cache.empty()) {
    if (m_appendCallback(U32StringToString(m_cache.back()))) {
      m_cache.pop_back();
    }
  }
  return *this;
};

Sentence &Sentence::Append(const std::string &text) {
  auto u32text = StringToU32String(text);
  // 首先第一个是换行符
  std::vector<std::u32string> u32strList;
  boost::split(u32strList, u32text, boost::is_any_of(U"\n"));
  for (auto &lineStr: u32strList) {
    // 单行
    lineStr = U32trim(lineStr);
    // 略过空
    if (lineStr.empty()) {
      continue;
    }
    // 继续细分
    bool isReg = true;
    std::u32string regs;
    std::vector<char32_t> *checks = nullptr;
    static std::vector<char32_t> checksStrictPunctuation = {U',', U'.', U';', U'?', U'!', U'、', U'，', U'。', U'？', U'！',
                                                            U';', U'：', U'…'};
    static std::vector<char32_t> checksPunctuation = {U'.', U';', U'?', U'!', U'。', U'？', U'！',
                                                      U';', U'：', U'…'};
    switch (m_splitMethod) {
      case SentenceSplitMethod::StrictPunctuation:
        regs = UR"([,.;?!、，。？！;：…])";
        checks = &checksStrictPunctuation;
        break;
      case SentenceSplitMethod::Punctuation:
        regs = UR"([.;?!。？！;：…])";
        checks = &checksPunctuation;
        break;
      case SentenceSplitMethod::Length:
        isReg = false;
        break;
      default:
        THROW_ERROR("未知的切分方式");
    }
    std::vector<std::u32string> sentenceList;
    if (isReg) {
      SplitByRegex(sentenceList, lineStr, regs);
      if (sentenceList.empty()) {
        continue;
      }
      // 判断堆栈最后一个是否以期望符号结尾
      if (!m_cache.empty()) {
        auto appd = FindEnd(m_cache.back(), *checks);
        if (!appd) {
          m_cache.back().append(sentenceList[0]);
          sentenceList.erase(sentenceList.begin());
          // 检查触发
          if (FindEnd(m_cache.back(), *checks) && m_appendCallback) {
            if (m_appendCallback(U32StringToString(m_cache.back()))) {
              m_cache.pop_back();
            }
          }
        }
      }
    } else {
      // 按最大长度切
      m_maxLength = m_maxLength <= 0 ? 50 : m_maxLength;
      int addlen = 0;
      if (!m_cache.empty() && m_cache.back().length() < m_maxLength) {
        addlen = m_maxLength - m_cache.back().length();
      }
      int index = 0;
      addlen = std::min((int) lineStr.size(), addlen);

      if (addlen > 0) {
        // 需要补充到缓存中
        m_cache.back().append(lineStr.substr(0, addlen));
        if (m_cache.back().size() >= m_maxLength) {
          if (m_appendCallback(U32StringToString(m_cache.back()))) {
            m_cache.pop_back();
          }
        }

        index += addlen;
      }

      while (index < lineStr.size()) {
        int subcount = std::min((int) m_maxLength, (int) lineStr.size() - index);
        sentenceList.emplace_back(lineStr.substr(index, subcount));
        index += subcount;
      }
    }
    for (auto &sentence: sentenceList) {
      bool call = false;
      if (m_splitMethod == SentenceSplitMethod::Length) {
        if (!m_cache.empty() && m_cache.back().size() < m_maxLength) {
          m_cache.back() += U32trim(sentence);
        } else {
          m_cache.emplace_back(U32trim(sentence));
        }
        if (m_cache.back().size() >= m_maxLength) {
          call = true;
        }
      } else {
        m_cache.emplace_back(U32trim(sentence));
        call = FindEnd(m_cache.back(), *checks);
      }
      if (call && m_appendCallback) {
        if (m_appendCallback(U32StringToString(m_cache.back()))) {
          m_cache.pop_back();
        }
      }
    }
  }

  return *this;
}

void Sentence::AppendCallBack(const std::function<bool(const std::string &)> &appendCallback) {
  m_appendCallback = appendCallback;
};

std::optional<std::string> Sentence::Pop() {
  if (m_cache.empty()) {
    return std::nullopt;
  }
  auto l = U32StringToString(m_cache.back());
  m_cache.pop_back();
  return std::make_optional<std::string>(l);
};

};

//
// Created by 19254 on 24-11-29.
//

#ifndef GPT_SOVITS_CPP_SENTENCE_H
#define GPT_SOVITS_CPP_SENTENCE_H

#include <cstdint>
#include <string>
#include <GPTSovits/Utils/Stack.h>
#include <functional>
#include <optional>

namespace GPTSovits::Text {

/**
 * @brief 句子处理类
 *
 * 该类用于对文本进行切分和处理，支持多种切分方式，并可以在追加文本时使用回调函数进行处理。
 */
class Sentence {
public:
  /**
  * 插入完整单句的回调函数
  *
  * @brief 该回调函数用于处理追加的文本。
  *
  * @param text 要处理的文本字符串。
  * @return 返回true表示接收本条文本,并且类会将文本从栈顶弹出，返回false表示无操作。
   */
  typedef std::function<bool(const std::string &)> SentenceAppendCallbackFunction;

    /// @brief 切分方式枚举
  enum class SentenceSplitMethod : int32_t {
    /// @brief 按标点符号切分
    Punctuation,
    /// @brief 按标点符号切分(严格模式,逗号等符号也会切分)
    StrictPunctuation,
    /// @brief 按最大长度切分
    Length,
  };

  /** @brief 构造函数
  *
  * @param cutMethod 切分方式，默认为按标点符号切分
  * @param maxLength 当切分方式为Length时有效，表示最大长度，默认为50，不推荐过长
 */
  Sentence(SentenceSplitMethod cutMethod = SentenceSplitMethod::Punctuation, int maxLength = 50);

  /** @brief 追加文本
  *
  * @param text 要追加的文本
  * @return 返回当前对象的引用，以便进行链式调用
 */
  Sentence &Append(const std::string &text);

  /** @brief 结束追加
  *
   * 当Append完成,但是最后一句未满足切分条件时调用,手动触发 appendCallback .
   *
  * @return 返回当前对象的引用，以便进行链式调用
 */
  Sentence &Flush();

  /** @brief 设置追加的回调函数
  *
  * @param appendCallback 处理追加文本的回调函数
 */
  void AppendCallBack(const SentenceAppendCallbackFunction &appendCallback);

  /** @brief 弹出完整句子
   *
   * 弹出后
   *
   * @return 返回切分后的字符串
   * @throws GPTSovits::Exception 当内部为空时抛出此异常
 */
  std::optional<std::string> Pop();

private:
  SentenceSplitMethod m_splitMethod; ///< 切分方式的成员变量
  uint32_t m_maxLength = 50; ///< 最大长度限制
  std::list<std::u32string> m_cache; ///< 存储切分后的字符串缓存
  SentenceAppendCallbackFunction m_appendCallback; ///< 追加文本的回调函数
};

};


#endif //GPT_SOVITS_CPP_SENTENCE_H

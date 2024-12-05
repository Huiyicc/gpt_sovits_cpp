//
// Created by 19254 on 24-12-1.
//

#ifndef GPT_SOVITS_CPP_LANGDETECT_H
#define GPT_SOVITS_CPP_LANGDETECT_H

#include <memory>
#include <string>
#include <vector>

namespace tokenizers {
class Tokenizer;
}

namespace GPTSovits::Text {

class LangDetect {
  static LangDetect *m_instance;

  LangDetect();

  LangDetect(const LangDetect &) = delete;


  // 自行删除前后空格
  std::pair<bool, std::string> detect_word(const std::string &defaultLang, const std::string &word, const std::u32string &uword);

public:

  /**
   * @brief 语言句子结构体，用于存储语言相关的句子信息。
   */
  struct LanguageSentence {
    // 句子
    std::string sentence;
    // 句子u32字符串
    std::u32string u32sentence;
    // 语言
    std::string language;
  };

  LangDetect &operator=(const LangDetect &) = delete;

  static LangDetect *getInstance() {
    if (!m_instance) {
      m_instance = new LangDetect();
    }
    return m_instance;
  }

  std::pair<bool, std::string> Detect(const std::string &input);

  /**
   * @brief 根据输入字符串和默认语言进行语言检测和分句。
   *
   * @param defaultLang 默认语言，如果输入的语言不在配置中，将使用此语言替代。
   * @param input 输入的字符串，需要进行语言检测和分句。
   *
   * @return std::vector<LangDetect::LanguageSentence> 返回检测到的语言句子列表。
   *
   * @exception GPTSovits::Exception 如果提供的默认语言不在配置中，将抛出错误。
   *
   * @details 此函数会对输入字符串进行处理，识别其中的语言并进行分句。
   *          如果遇到特殊字符，将根据其类型进行处理并合并到相应的句子中。
   *          通过分词和语言检测，构建语言句子的结构体并返回。
   *
   * @example
   * @code
   * std::string defaultLang = "zh";
   * std::string input = "我study日本語的时候，もし有汉字，我会很happy。";
   * auto sentences = LangDetect::DetectSplit(defaultLang, input);
   * // 结果如下
   * sentences:
   * {
   *    { "zh" , "我" },
   *    { "en" , "study" },
   *    { "zh" , "日本語的时候，" },
   *    { "jp" , "もし" },
   *    { "zh" , "有汉字，我会很" },
   *    { "en" , "happy。" },
   * }
   * @endcode
   */
  std::vector<LanguageSentence> DetectSplit(const std::string &defaultLang, const std::string &input);

  std::vector<std::string> Tokenize(const std::string &text, bool add_special_tokens);

private:
  std::unique_ptr<tokenizers::Tokenizer> m_tokenizer;
};


};

#endif //GPT_SOVITS_CPP_LANGDETECT_H

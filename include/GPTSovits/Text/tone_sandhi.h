//
// Created by 19254 on 24-11-11.
//

#ifndef GPT_SOVITS_CPP_TONE_SANDHI_H
#define GPT_SOVITS_CPP_TONE_SANDHI_H

#include <string>
#include <vector>
#include <memory>
#include <set>

namespace cppjieba {
class Jieba;
}

namespace GPTSovits {

class ToneSandhi {
  std::shared_ptr<cppjieba::Jieba> m_jieba;
  std::u32string punc = U"：，；。？！“”‘’':,;.?!";
  static std::set<std::u32string> must_not_neural_tone_words;
  static std::set<std::u32string> must_neural_tone_words;

  std::vector<std::string> bu_sandhi(const std::u32string& word, std::vector<std::string>& finals);

  std::vector<std::string> yi_sandhi(const std::u32string& word, std::vector<std::string>& finals);
  std::vector<std::u32string> splitWord(const std::u32string &word);
  std::vector<std::string> neural_sandhi(const std::u32string& word, const std::string& pos, std::vector<std::string>& finals);
  bool all_tone_three(const std::vector<std::string>& finals);
  std::vector<std::string> three_sandhi(const std::u32string& word, std::vector<std::string> finals);

public:
  ToneSandhi(std::shared_ptr<cppjieba::Jieba> jieba);
  std::vector<std::string> modified_tone(const std::u32string&word,const std::string&pos,std::vector<std::string>&finals);

};

}

#endif //GPT_SOVITS_CPP_TONE_SANDHI_H

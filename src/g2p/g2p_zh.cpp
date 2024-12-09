//
// Created by 19254 on 24-12-1.
//
#include <GPTSovits/G2P/g2p.h>
#include <GPTSovits/G2P/g2p_zh.h>
#include "cppjieba/Jieba.hpp"
#include "GPTSovits/Text/tone_sandhi.h"
#include "GPTSovits/Text/Coding.h"
#include "GPTSovits/Text/Utils.h"
#include <boost/algorithm/string.hpp>
#include <GPTSovits/Text/TextNormalizer/zh.h>
#include <GPTSovits/Text/TextNormalizerMap.h>
#include <cpp-pinyin/Pinyin.h>
#include <cpp-pinyin/G2pglobal.h>
#include <numeric>
#include "GPTSovits/G2P/polyphonic.h"
#include "GPTSovits/plog.h"

namespace GPTSovits {
extern std::filesystem::path g_globalResourcesPath;
}

namespace GPTSovits::G2P {

std::vector<std::string> correct_pronunciation(const std::string &word, const std::vector<Pinyin::PinyinRes> &input) {
  std::vector<std::string> out;
  auto pp_dict = get_polyphonic_map();
  auto iter = pp_dict.find(word);
  if (iter != pp_dict.end()) {
    for (auto &p: iter->second) {
      out.emplace_back(p);
    }
    return std::move(out);
  } else {
    for (auto &p: input) {
      out.emplace_back(p.pinyin);
    }
    return std::move(out);
  }
}

std::unordered_set<std::u32string> must_erhua = {
  U"小院儿", U"胡同儿", U"范儿", U"老汉儿", U"撒欢儿", U"寻老礼儿", U"妥妥儿", U"媳妇儿"
};

std::unordered_set<std::u32string> not_erhua = {
  U"虐儿", U"为儿", U"护儿", U"瞒儿", U"救儿", U"替儿", U"有儿", U"一儿", U"我儿", U"俺儿", U"妻儿",
  U"拐儿", U"聋儿", U"乞儿", U"患儿", U"幼儿", U"孤儿", U"婴儿", U"婴幼儿", U"连体儿", U"脑瘫儿",
  U"流浪儿", U"体弱儿", U"混血儿", U"蜜雪儿", U"舫儿", U"祖儿", U"美儿", U"应采儿", U"可儿", U"侄儿",
  U"孙儿", U"侄孙儿", U"女儿", U"男儿", U"红孩儿", U"花儿", U"虫儿", U"马儿", U"鸟儿", U"猪儿", U"猫儿",
  U"狗儿", U"少儿"
};


std::pair<std::vector<std::string>, std::vector<std::string>>
merge_erhua(std::vector<std::string> initials,
            std::vector<std::string> finals,
            const std::u32string &word,
            const std::string &pos) {

  // fix er1
  for (size_t i = 0; i < finals.size(); ++i) {
    if (i == finals.size() - 1 && word[i] == U'儿' && finals[i] == "er1") {
      finals[i] = "er2";
    }
  }

  // 发音
  if (must_erhua.find(word) == must_erhua.end() &&
      (not_erhua.find(word) != not_erhua.end() || pos == "a" || pos == "j" || pos == "nr")) {
    return {initials, finals};
  }

  // "……" 等情况直接返回
  if (finals.size() != word.size()) {
    return {initials, finals};
  }

  assert(finals.size() == word.size());

  // 与前一个字发同音
  std::vector<std::string> new_initials;
  std::vector<std::string> new_finals;
  for (size_t i = 0; i < finals.size(); ++i) {
    std::string phn = finals[i];
    if (i == finals.size() - 1 && word[i] == U'儿' && (phn == "er2" || phn == "er5") &&
        not_erhua.find(word.substr(word.size() - 2)) == not_erhua.end() && !new_finals.empty()) {
      phn = "er" + new_finals.back().substr(new_finals.back().size() - 1);
    }

    new_initials.push_back(initials[i]);
    new_finals.push_back(phn);
  }

  return {new_initials, new_finals};
}


std::tuple<std::vector<std::string>, std::vector<int>>
_g2p_zh(const std::vector<std::u32string> &segments) {
  std::vector<std::string> phones;
  std::vector<int> word2ph;
  std::vector<std::string> initials;
  std::vector<std::string> finals;
  std::vector<std::string> phones_list;
  static auto jieba = []() {
    PrintInfo("Init CPPJieba.");
    return std::make_shared<cppjieba::Jieba>((g_globalResourcesPath / "jieba_dict" / "jieba.dict.utf8").string(),
                                             (g_globalResourcesPath / "jieba_dict" / "hmm_model.utf8").string(),
                                             (g_globalResourcesPath / "jieba_dict" / "user.dict.utf8").string(),
                                             (g_globalResourcesPath / "jieba_dict" / "idf.utf8").string(),
                                             (g_globalResourcesPath / "jieba_dict" / "stop_words.utf8").string());
  }();
  static auto tone_modifier = ToneSandhi(jieba);
  static auto g2p_man = []() {
    PrintInfo("Init cpp-pinyin.");
    Pinyin::setDictionaryPath(g_globalResourcesPath / "dict");
    return std::make_unique<Pinyin::Pinyin>();
  }();
//  const auto g2p_man = ;
  for (auto &segU32: segments) {
    std::vector<std::vector<std::string>> initialss;
    std::vector<std::vector<std::string>> finalss;
    auto seg = Text::U32StringToString(segU32);
    std::vector<std::pair<std::string, std::string>> seg_cut;
    jieba->Tag(seg, seg_cut);
    auto pinyins = g2p_man->hanziToPinyin(seg, Pinyin::ManTone::Style::TONE3, Pinyin::Error::Default, true, false,
                                          true);

    auto pre_word_length = 0;
    for (auto &[word, pos]: seg_cut) {
      auto u32Word = Text::StringToU32String(word);

      auto now_word_length = pre_word_length + u32Word.size();
      if (pos == "eng") {
        pre_word_length = now_word_length;
        continue;
      }

      std::vector<Pinyin::PinyinRes> lpy(pinyins.begin() + pre_word_length,
                                         pinyins.begin() +
                                         now_word_length);
      std::vector<std::string> sub_initials;
      std::vector<std::string> sub_finals;
      // 多音字消歧
      auto word_pinyins = correct_pronunciation(word, lpy);
      for (auto &pinyin: word_pinyins) {
        if (!pinyin.empty() && Text::safe_isalpha(pinyin[0])) {
          auto lp = Pinyin::getInitials(pinyin);
          sub_initials.emplace_back(lp);
          sub_finals.emplace_back(pinyin.substr(lp.size()));
        } else {
          sub_initials.emplace_back(pinyin);
          sub_finals.emplace_back(pinyin);
        }
      }
      pre_word_length = now_word_length;
      sub_finals = tone_modifier.modified_tone(u32Word, pos, sub_finals);
      // 儿化
      auto rs = merge_erhua(sub_initials, sub_finals, u32Word, pos);
      sub_initials = std::move(rs.first);
      sub_finals = std::move(rs.second);
      initialss.emplace_back(std::move(sub_initials));
      finalss.emplace_back(std::move(sub_finals));
    }
    auto _sum_ = [](const std::vector<std::vector<std::string>> &input) -> std::vector<std::string> {
      std::vector<std::string> out;
      for (auto &a: input) {
        for (auto &b: a) {
          out.emplace_back(b);
        }
      }
      return std::move(out);
    };
    initials = _sum_(initialss);
    finals = _sum_(finalss);

    assert(initials.size() == finals.size());
    static std::set<std::string_view> punctuation = {"!", "?", "…", ",", ".", "-"};

    for (int i = 0; i < initials.size(); i++) {
      std::vector<std::string> phone;
      auto &c = initials[i];
      auto &v = finals[i];
      auto raw_pinyin = c + v;
      if (c == v) {
        assert(punctuation.find(c) != punctuation.end());
        phone = {c};
        word2ph.emplace_back(1);
      } else {
        assert(!v.empty());
        auto v_without_tone = v.substr(0, v.size() - 1);
        auto tone = v.back();
        auto pinyin = c + v_without_tone;
        assert(tone == '1' || tone == '2' || tone == '3' || tone == '4' || tone == '5');
        if (!c.empty()) {
          // 多音节
          static std::unordered_map<std::string_view, std::string> v_rep_map = {
            {"uei", "ui"},
            {"iou", "iu"},
            {"uen", "un"},
          };
          auto iter = v_rep_map.find(v_without_tone);
          if (iter != v_rep_map.end()) {
            pinyin = c + iter->second;
          }
        } else {
          // 单音节
          static std::unordered_map<std::string_view, std::string> v_rep_map = {
            {"ing", "ying"},
            {"i",   "yi"},
            {"in",  "yin"},
            {"u",   "wu"},
          };
          auto iter = v_rep_map.find(v_without_tone);
          if (iter != v_rep_map.end()) {
            pinyin = iter->second;
          } else {
            static std::unordered_map<char, std::string> single_rep_map = {
              {'v', "yu"},
              {'e', "e"},
              {'i', "y"},
              {'u', "w"},
            };
            auto iter1 = single_rep_map.find(pinyin[0]);
            if (iter1 != single_rep_map.end()) {
              pinyin = single_rep_map[pinyin[0]] + pinyin.substr(1, pinyin.size() - 1);
            }
          }
        }
        std::vector<std::string> tmvs;
        auto sp = Text::pinyin_to_symbol_map[pinyin];
        boost::split(tmvs, sp, boost::is_space());
        assert(tmvs.size() == 2);
        std::string new_c = tmvs[0];
        std::string new_v = tmvs[1];
        new_v += tone;
        phone = {new_c, new_v};
        word2ph.push_back(phone.size());
      }
      phones_list.insert(phones_list.end(), phone.begin(), phone.end());
    }
  }

  return std::make_tuple(phones_list, word2ph);
}

std::tuple<std::vector<std::string>, std::vector<int>>
g2p_zh(const std::string &text) {
  auto inSentence = Text::StringToU32String(text);
  srell::u32regex re(UR"((?<=[!?…,.-])\s*)");
  srell::u32sregex_token_iterator it(inSentence.begin(), inSentence.end(), re, -1);
  srell::u32sregex_token_iterator reg_end;

  std::vector<std::u32string> sentences;

  for (; it != reg_end; ++it) {
    std::u32string sentence = it->str();
    if (!sentence.empty()) {
      // 去掉首尾空白
      sentence.erase(0, sentence.find_first_not_of(U' ')); // 去掉开头空格
      sentence.erase(sentence.find_last_not_of(U' ') + 1); // 去掉结尾空格
      sentences.push_back(sentence);
    }
  }
  return _g2p_zh(sentences);
}

G2PRes G2PZH::_cleanText(const std::string &text) {
  auto norm_text = Text::text_normalize_zh(text);
  auto [phones, word2ph] = g2p_zh(norm_text);
  assert(phones.size() == std::accumulate(word2ph.begin(), word2ph.end(), 0));
  assert(Text::StringToU32String(norm_text).size() == word2ph.size());
  return {phones, {}, word2ph, norm_text};
}

}
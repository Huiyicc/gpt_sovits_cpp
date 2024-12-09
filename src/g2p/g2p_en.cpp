//
// Created by 19254 on 24-12-2.
//
#include <GPTSovits/G2P/g2p_en.h>
#include <GPTSovits/Utils/exception.h>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include "GPTSovits/Text/TextNormalizer/en.h"
#include "GPTSovits/Text/Coding.h"
#include "GPTSovits/plog.h"
#include "GPTSovits/Text/LangDetect.h"
#include <nlohmann/json.hpp>
#include <set>

namespace GPTSovits {
extern std::filesystem::path g_globalResourcesPath;
}

namespace GPTSovits::G2P {

std::unordered_map<std::string, std::vector<std::string>> g_en_pos_cache;
std::unordered_map<std::string, std::vector<std::vector<std::string>>> g_en_cmu;
struct HomographFeature {
  std::vector<std::string> phoneme1;
  std::vector<std::string> phoneme2;
  std::string partOfSpeech;
};
std::unordered_map<std::string, HomographFeature> g_en_homograph2features;
std::unordered_map<std::string, std::vector<std::vector<std::string>>> g_en_namedict;

void init_en_cmu() {
  auto fpath = g_globalResourcesPath / "g2p" / "en" / "engdict_cache_pickle.json";
  #ifdef _HOST_WINDOWS_
  std::ifstream file(fpath.wstring());
  #else
  std::ifstream file(fpath);
  #endif
  if (!file.is_open()) {
    THROW_ERRORN("Cannot open file: {}", fpath.string());
  }
  nlohmann::json jsonData;
  file >> jsonData;
  // nlohmann/json yyds
  g_en_cmu = jsonData.get<std::unordered_map<std::string, std::vector<std::vector<std::string>>>>();
}

void init_en_namedict() {
  auto fpath = g_globalResourcesPath / "g2p" / "en" / "namedict_en.json";
  #ifdef _HOST_WINDOWS_
  std::ifstream file(fpath.wstring());
  #else
  std::ifstream file(fpath);
  #endif
  if (!file.is_open()) {
    THROW_ERRORN("Cannot open file: {}", fpath.string());
  }
  nlohmann::json jsonData;
  file >> jsonData;
  // nlohmann/json yyds
  g_en_namedict = jsonData.get<std::unordered_map<std::string, std::vector<std::vector<std::string>>>>();
}

void init_en_homograph2features() {
  auto fpath = g_globalResourcesPath / "g2p" / "en" / "homograph2features_en.json";
  #ifdef _HOST_WINDOWS_
  std::ifstream file(fpath.wstring());
  #else
  std::ifstream file(fpath);
  #endif
  if (!file.is_open()) {
    THROW_ERRORN("Cannot open file: {}", fpath.string());
  }
  nlohmann::json jsonData;
  file >> jsonData;
  for (auto &[word, features]: jsonData.items()) {
    auto lword = boost::to_lower_copy(word);
    g_en_homograph2features[lword].phoneme1 = features[0].get<std::vector<std::string>>();
    g_en_homograph2features[lword].phoneme2 = features[1].get<std::vector<std::string>>();
    g_en_homograph2features[lword].partOfSpeech = features[2].get<std::string>();
  }
}

void init_en_pos_cache() {
  auto fpath = g_globalResourcesPath / "g2p" / "en" / "en_pos_tagger.txt";
  #ifdef _HOST_WINDOWS_
  std::ifstream file(fpath.wstring());
  #else
  std::ifstream file(fpath);
  #endif
  if (!file.is_open()) {
    THROW_ERRORN("Cannot open file: {}", fpath.string());
  }
  std::string line;
  while (std::getline(file, line)) {
    int l_idx = line.find('/');
    if (l_idx == -1) {
      continue;
    }
    auto word = line.substr(0, l_idx);
    boost::to_lower(word);
    auto poss = line.substr(l_idx + 1);
    if (g_en_pos_cache.find(word) == g_en_pos_cache.end()) {
      g_en_pos_cache[word] = {};
    }
    boost::split(g_en_pos_cache[word], poss, boost::is_any_of("|"));
  }
}

std::vector<std::pair<std::string, std::string>>
pos_tag(const std::vector<std::string> &words) {

  std::vector<std::pair<std::string, std::string>> result;
  for (auto &word: words) {
    // 默认nn
    std::string tag = "NN";
    auto pword = boost::to_lower_copy(word);
    boost::trim_copy(pword);
    auto iter = g_en_pos_cache.find(pword);
    if (iter != g_en_pos_cache.end()) {
      tag = iter->second.size() > 0 ? iter->second[0] : "NN";
    }
    result.emplace_back(word, tag);
  }
  return result;
}

void init_g2p_en_cache() {
  if (g_en_pos_cache.empty()
      || g_en_cmu.empty()
      || g_en_homograph2features.empty()
      || g_en_namedict.empty()
    ) {
    PrintInfo("Init G2P EN");
    init_en_pos_cache();
    init_en_cmu();
    init_en_homograph2features();
    init_en_namedict();
  }
}

bool isTitle(const std::string &s) {
  bool newWord = true; // 用于跟踪是否是新单词的开始

  for (char ch: s) {
    if (std::isspace(ch)) {
      newWord = true; // 遇到空格，标记为新单词的开始
    } else {
      if (newWord) {
        if (!std::isupper(ch)) {
          return false; // 如果新单词不是以大写字母开头，返回 false
        }
        newWord = false; // 标记为非新单词
      } else {
        if (!std::islower(ch)) {
          return false; // 如果单词中其他字符不是小写字母，返回 false
        }
      }
    }
  }
  return true; // 所有单词符合条件，返回 true
}


std::vector<std::string> qryword(const std::string &input) {
  auto oword = boost::trim_copy(input);
  auto word = boost::to_lower_copy(oword);
  boost::trim(word);
  // 查字典, 单字母除外
  if (auto iter = g_en_cmu.find(word);
    word.size() > 1
    && iter != g_en_cmu.end()) {
    return iter->second[0];
  }
  // 单词仅首字母大写时查找姓名字典
  if (auto iter = g_en_namedict.find(word);
    isTitle(oword) && iter != g_en_namedict.end()) {
    return iter->second[0];
  }
  std::vector<std::string> phones;
  // oov 长度小于等于 3 直接读字母
  if (word.size() < 3) {
    for (auto w: word) {
      // 单读 A 发音修正, 此处不存在大写的情况
      if (w == 'A') {
        phones = {"EY1"};
      } else if (std::isalpha(w)) {
        phones.push_back(std::string(1, w));
      } else {
        std::string sw;
        sw += w;
        auto &ins = g_en_cmu[sw][0];
        phones.insert(phones.end(), ins.begin(), ins.end());
      }
    }
    return phones;
  }
  // 尝试分离所有格
  std::smatch match;
  std::regex re("^([a-z]+)('s)$");
  if (std::regex_match(word, match, re)) {
    phones = qryword(word.substr(0, word.size() - 2));

    if (!phones.empty()) {
      std::string last_phone = phones.back();

      // P T K F TH HH 无声辅音结尾 's 发 ['S']
      if (last_phone == "P" || last_phone == "T" || last_phone == "K" ||
          last_phone == "F" || last_phone == "TH" || last_phone == "HH") {
        phones.push_back("S");
      }
        // S Z SH ZH CH JH 擦声结尾 's 发 ['IH1', 'Z'] 或 ['AH0', 'Z']
      else if (last_phone == "S" || last_phone == "Z" || last_phone == "SH" ||
               last_phone == "ZH" || last_phone == "CH" || last_phone == "JH") {
        phones.push_back("AH0");
        phones.push_back("Z");
      }
        // B D G DH V M N NG L R W Y 有声辅音结尾 's 发 ['Z']
        // AH0 AH1 AH2 EY0 EY1 EY2 AE0 AE1 AE2 EH0 EH1 EH2 OW0 OW1 OW2 UH0 UH1 UH2 IY0 IY1 IY2 AA0 AA1 AA2 AO0 AO1 AO2
        // ER ER0 ER1 ER2 UW0 UW1 UW2 AY0 AY1 AY2 AW0 AW1 AW2 OY0 OY1 OY2 IH IH0 IH1 IH2 元音结尾 's 发 ['Z']
      else {
        phones.push_back("Z");
      }
    }
    return phones;
  }
  std::vector<std::string> comps;
  if (word.find(' ') != -1) {
    comps = Text::LangDetect::getInstance()->Tokenize(boost::trim_copy(word), false);
  } else {
    comps.push_back(word);
  }
  if (comps.size() == 1) {
//    THROW_ERRORN("www? :{}",word);
    return g2p_en::predict(word);
  }
  std::vector<std::string> result;
  for (const auto &comp: comps) {
    phones = qryword(comp);
    result.insert(result.end(), phones.begin(), phones.end());
  }
  return result;
}

bool check_str(const std::string &word) {
  static std::set<std::string> stop_words = {"<bos>", "<eos>", ""};
  auto iter = stop_words.find(word);
  return iter == stop_words.end();
}

std::tuple<std::vector<std::string>, std::vector<int>>
_g2p_en(const std::string &segments) {
  init_g2p_en_cache();
  auto words = Text::LangDetect::getInstance()->Tokenize(segments, true);
  auto tokens = pos_tag(words);
  std::vector<std::string> prons;
  for (auto &[o_word, pos]: tokens) {
    boost::trim(o_word);
    auto word = boost::to_lower_copy(o_word);
    if (!check_str(word)) { continue; }
    std::vector<std::string> pron;
    if (!std::regex_search(word, std::regex("[a-z]"))) {
      pron = {word};
    } else if (word.size() == 1) {
      // 单字母
      if (o_word == "A") {
        pron = {"EY1"};
      } else {
        pron = g_en_cmu[word][0];
      }

    } else if (auto iter = g_en_homograph2features.find(word);iter != g_en_homograph2features.end()) {
      auto &[pron1, pron2, pos1] = iter->second;
      // 多音字
      if (pos1.size() > pos.size()
          && pos.compare(0, pos1.size(), pos1)) {
        pron = pron1;
      } else if (pos.size() < pos1.size()) {
        pron = pron1;
      } else {
        pron = pron2;
      }
    } else {
      pron = qryword(o_word);
    }
    prons.insert(prons.end(), pron.begin(), pron.end());
    prons.emplace_back(" ");
  }
  return {std::vector<std::string>(prons.begin(), prons.end() - 1), {}};
}

std::tuple<std::vector<std::string>, std::vector<int>>
G2P_en(const std::string &text) {
  auto [phone_list, _] = _g2p_en(text);
  std::vector<std::string> phones;
  for (const auto &ph: phone_list) {
    if (ph != " " && ph != "<pad>" && ph != "UW" && ph != "</s>" && ph != "<s>") {
      phones.push_back(ph != "<unk>" ? ph : "UNK");
    }
  }
  return {phones, {}};
}

G2PRes G2PEN::_cleanText(const std::string &text) {
  auto normText = Text::text_normalize_en(text);
  auto [phones, _] = G2P_en(normText);
  if (phones.size() < 4) {
    phones.insert(phones.begin(), ",");
  }
  return {phones, {}, {}, normText};
};

}

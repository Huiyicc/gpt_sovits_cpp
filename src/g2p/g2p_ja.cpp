//
// Created by 19254 on 24-12-12.
//
#include <GPTSovits/G2P/g2p_ja.h>
#include <GPTSovits/Text/Coding.h>
#include <GPTSovits/Text/TextNormalizer/ja.h>
#include <GPTSovits/Text/Utils.h>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <set>
#include <srell.hpp>
#include <vector>

namespace GPTSovits {
extern std::filesystem::path g_globalResourcesPath;
}

namespace GPTSovits::G2P {

std::unique_ptr<OpenJTalk> g_openjtalk_ja = nullptr;

std::u32string symbols_to_japanese(const std::u32string &text) {
  std::u32string result = text;
  static std::vector<std::pair<srell::u32regex, std::u32string>> _symbols_to_japanese = {
    {srell::u32regex(U"％"), U"パーセント"},
  };
  for (auto &[regex, replace]: _symbols_to_japanese) {
    result = srell::regex_replace(result, regex, replace);
  }
  return result;
}

int numeric_feature_by_regex(const std::string &regex_str, const std::string &s) {
  srell::regex regex(regex_str);
  srell::smatch match;

  if (srell::regex_search(s, match, regex)) {
    return std::stoi(match[1].str());
  }
  return -50;
}

std::vector<std::string> pyopenjtalk_g2p_prosody(const std::string &text, bool drop_unvoiced_vowels = false) {
  if (!g_openjtalk_ja) {
    static auto dict_path = g_globalResourcesPath / "g2p" / "ja" / "open_jtalk_dic";
    static auto user_dict = g_globalResourcesPath / "g2p" / "ja" / "ja_userdic" / "user.dict";
    g_openjtalk_ja = OpenJTalk::FromUserDict(dict_path.string(), user_dict.string());
  }
  auto labels = g_openjtalk_ja->make_label(g_openjtalk_ja->run_frontend(text));
  auto N = labels.size();
  std::vector<std::string> phones;
  for (int n = 0; n < N; ++n) {
    auto &lab_curr = labels[n];

    srell::regex pattern(R"(-([^+]+)\+)");
    srell::smatch match;

    if (!srell::regex_search(lab_curr, match, pattern)) {
      continue;
    }
    std::string p3 = match[1];
    if (drop_unvoiced_vowels && (p3 == "A" || p3 == "E" || p3 == "I" || p3 == "O" || p3 == "U")) {
      std::transform(p3.begin(), p3.end(), p3.begin(), ::tolower);// 转换为小写
    }

    if (p3 == "sil") {
      assert(n == 0 || n == N - 1);
      if (n == 0) {
        phones.emplace_back("^");
      } else if (n == N - 1) {
        auto e3 = numeric_feature_by_regex(R"(!(\d+)_)", lab_curr);
        if (e3 == 0) {
          phones.emplace_back("$");
        } else if (e3 == 1) {
          phones.emplace_back("?");
        }
      }
      continue;
    } else if (p3 == "pau") {
      phones.emplace_back("_");
      continue;
    } else {
      phones.emplace_back(p3);
    }
    // accent type and position info (forward or backward)
    auto a1 = numeric_feature_by_regex(R"(/A:([0-9\-]+)\+)", lab_curr);
    auto a2 = numeric_feature_by_regex(R"(\+(\d+)\+)", lab_curr);
    auto a3 = numeric_feature_by_regex(R"(\+(\d+)/)", lab_curr);

    // number of mora in accent phrase
    auto f1 = numeric_feature_by_regex(R"(/F:(\d+)_)", lab_curr);

    auto a2_next = numeric_feature_by_regex(R"(\+(\d+)\+)", labels[n + 1]);
    // accent phrase border
    static std::set<std::string_view> accent_types = {"a", "e", "i", "o", "u", "A", "E", "I", "O", "U", "N", "c", "l"};
    if (auto iter = accent_types.find(p3); a3 == 1 && a2_next == 1 && iter != accent_types.end()) {
      // accent phrase border
      phones.emplace_back("#");
    } else if (a1 == 0 && a2_next == a2 + 1 && a2 != f1) {
      // pitch falling
      phones.emplace_back("]");
    } else if (a2 == 1 && a2_next == 2) {
      phones.emplace_back("[");
    }
  }
  return phones;
};

std::vector<std::string> preprocess_jap(const std::u32string &text, bool with_prosody = false) {
  std::vector<std::string> result;
  auto reText = symbols_to_japanese(text);
  Text::tolower(reText);
  std::vector<std::u32string> sentences;
  std::vector<std::u32string> marks;
  static std::u32string japanese_characters = UR"([A-Za-z\d\u3005\u3040-\u30ff\u4e00-\u9fff\uff11-\uff19\uff21-\uff3a\uff41-\uff5a\uff66-\uff9d])";
  static std::u32string japanese_marks = UR"([^A-Za-z\d\u3005\u3040-\u30ff\u4e00-\u9fff\uff11-\uff19\uff21-\uff3a\uff41-\uff5a\uff66-\uff9d])";
  static srell::u32regex kRegexMarks(japanese_marks);
  static srell::u32regex kRegexCharacters(japanese_characters);
  Text::SplitByRegex1(sentences, reText, japanese_marks);
  Text::FindAll1(marks, reText, japanese_marks);

  for (int i = 0; i < sentences.size(); ++i) {
    auto &sentence = sentences[i];
    if (srell::regex_search(sentence, kRegexCharacters)) {
      if (with_prosody) {
        auto lList = pyopenjtalk_g2p_prosody(Text::U32StringToString(sentence), with_prosody);
        result.insert(result.end(), lList.begin() + 1, lList.end() - 1);
      } else {
        auto [pList, p] = g_openjtalk_ja->g2p(Text::U32StringToString(sentence),false,false);
        result.insert(result.end(), pList.begin(), pList.end());
      }
    }
    if (i < marks.size()) {
      // 防止意外的UNK
      if (marks[i] == U" ") continue;
      result.emplace_back(boost::replace_all_copy(Text::U32StringToString(marks[i]), "　", ""));
    }
  }
  return result;
}

std::string post_replace_ph(const std::string& ph) {
  static std::unordered_map<std::string, std::string> rep_map = {
    {"：", ","},
    {"；", ","},
    {"，", ","},
    {"。", "."},
    {"！", "!"},
    {"？", "?"},
    {"\n", "."},
    {"·", ","},
    {"、", ","},
    {"...", "…"}
  };

  auto it = rep_map.find(ph);
  if (it != rep_map.end()) {
    return it->second;
  }
  return ph;
}

G2PRes G2PJA::_cleanText(const std::string &text) {
  auto reText = Text::text_normalize_ja(Text::StringToU32String(text));
  auto phonesL = preprocess_jap(reText, true);
  std::vector<std::string> phones;
  for (auto&p:phonesL) {
    phones.emplace_back(post_replace_ph(p));
  }
  return {std::move(phones),{},{},Text::U32StringToString(reText)};
}

}// namespace GPTSovits::G2P
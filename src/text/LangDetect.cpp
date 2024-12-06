//
// Created by 19254 on 24-12-1.
//
#include <GPTSovits/Text/LangDetect.h>
#include <CLD2/compact_lang_det.h>
#include <fstream>
#include <set>
#include <map>
#include <GPTSovits/Utils/exception.h>
#include "GPTSovits/plog.h"
#include "GPTSovits/Text/Utils.h"
#include "GPTSovits/Text/Coding.h"
#include <tokenizers_cpp.h>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <srell.hpp>

namespace GPTSovits::Text {

LangDetect *LangDetect::m_instance = nullptr;

// 符号范围
std::vector<std::pair<char32_t, char32_t>> g_symbol_scope = {
  // 中文符号
  {0x3000, 0x303F},
  // 英文符号（全角）
  {0xFF00, 0xFFEF},
  // 日文平假名
  {0x3040, 0x309F},
  // 日文片假名
  {0x30A0, 0x30FF},
  // 韩文音节
  {0xAC00, 0xD7A3},
  // 阿拉伯数字
  {0x0660, 0x0669},
  // 阿拉伯字母
  {0x0621, 0x064A},
  // 数字也算到符号,交给g2p统一处理
  {0x30,   0x39},
};

bool has_symbol(const std::u32string &input) {
  static std::set<char32_t> chars = {U',', U'.', U';', U'?', U'!', U'、', U'，', U'。', U'？', U'！',
                                     U';', U'：', U'…', U'”', U'“', U'‘', U'’', U'"', U'\'', U'·', U'°',
                                     U'(', U')', U'-', U'+', U'*', U'/', U'%', U'&', U'|', U'^', U'~', U'<', U'>', U'=',
                                     U'!', U'@', U'#', U'$', U'%', U'^', U'&', U'*', U'(', U')', U'[', U']', U'{', U'}',
                                     U':'};
  auto res = std::any_of(chars.begin(), chars.end(), [&input](char32_t c) {
    return input.find(c) != std::u32string::npos;
  });
  if (res) return res;
  for (auto &scope: g_symbol_scope) {
    for (auto c: input) {
      if (c >= scope.first && c <= scope.second) {
        return true;
      }
    }
  }
  return false;
}

bool has_lang(const std::vector<std::pair<char32_t, char32_t>> &maps, const std::u32string &input) {
  for (auto &scope: maps) {
    for (auto c: input) {
      if (c >= scope.first && c <= scope.second) {
        return true;
      }
    }
  }
  return false;
};
// 中文范围
std::vector<std::pair<char32_t, char32_t>> g_cn_scope = {
  // CJK 统一汉字
  {0x4E00,  0x9FFF},
  // CJK 扩展 A
  {0x3400,  0x4DBF},
  // CJK 扩展 B
  {0x20000, 0x2A6DF},
  // CJK 扩展 C
  {0x2A700, 0x2B73F},
  // CJK 扩展 D
  {0x2B740, 0x2B81F},
  // CJK 扩展 E
  {0x2B820, 0x2CEAF},
  // CJK 扩展 F
  {0x2CEB0, 0x2EBEF},
  // CJK 扩展 G
  {0x30000, 0x3134F},
  // CJK 扩展 H
  {0x31350, 0x323AF},
  // CJK 兼容汉字
  {0xF900,  0xFAFF},
  // 全角标点符号
//  {0x3000, 0x303F},
};

bool has_cn(const std::u32string &input) {
  return has_lang(g_cn_scope, input);
};


// 英文范围
std::vector<std::pair<char32_t, char32_t>> g_en_scope = {
//  // 数字
//  {0x30, 0x39},
  // 大写英文字母
  {0x41, 0x5A},
  // 小写英文字母
  {0x61, 0x7A},
};

bool has_en(const std::u32string &input) {
  return has_lang(g_en_scope, input);
};


// 日语范围
std::vector<std::pair<char32_t, char32_t>> g_jp_scope = {
  // 平假名
  {0x3040,  0x309F},
  // 片假名
  {0x30A0,  0x30FF},
  // 片假名扩展
  {0x31F0,  0x31FF},
  // 假名补充
  {0x1B000, 0x1B0FF},
  // 假名扩展 A
  {0x1B100, 0x1B12F},
  // 假名扩展 B
  {0x1B130, 0x1B16F},
};

bool has_jp(const std::u32string &input) {
  return has_lang(g_jp_scope, input);
};


// 韩文范围
std::vector<std::pair<char32_t, char32_t>> g_kr_scope = {
  // 韩文音节
  {0x1100, 0x11FF},
  // 韩文兼容字母
  {0x3130, 0x318F},
  // 韩文音节
  {0xAC00, 0xD7A3},
  // 韩文扩展音素A
  {0xA960, 0xA97F},
  // 韩文扩展音素B
  {0xD7B0, 0xD7FF},
};

bool has_kr(const std::u32string &input) {
  return has_lang(g_kr_scope, input);
};

struct SpecialInfo {
  // 文本内容
  std::u32string text;
  // 是否是特殊组合
  bool is_special = false;
};

// 处理特殊组合,将匹配到了的组合划分到单独一句,符合表达式的 is_special 为 true
std::vector<SpecialInfo> handle_special(std::u32string &input) {
  std::vector<std::u32string> regs = {
    // 温度
    UR"(([-+]?\d+(\.\d+)?\s?[°º]?[CFK]))",
    // 货币
    UR"(([￥$€£¥])\s*([0-9,]+(?:\.[0-9]{1,2})?))",
    // 百分比
    UR"((\b\d+(\.\d+)?\s?%))",
    // 重量
    UR"((\d+(\.\d+)?)(\s*(g|kg|lb|oz)))",
    // 面积
    UR"((\d+(\.\d+)?)(\s*(m²|km²|ft²|in²)))",
    // 频率
    UR"((\d+(\.\d+)?)(\s*(Hz|kHz|MHz|GHz)))",
    // 长度
    UR"((\d+(\.\d+)?)(\s*(m|cm|mm|ft|in)))",
  };
  std::vector<SpecialInfo> result;
  std::u32string remaining = input;

  for (const auto &reg: regs) {
    srell::u32regex regex(reg);
    std::u32string::const_iterator search_start(remaining.cbegin());
    srell::u32smatch match;

    while (srell::regex_search(search_start, remaining.cend(), match, regex)) {
      if (match.position() > 0) {
        // Add non-matching prefix as a normal text
        result.push_back(SpecialInfo{std::u32string(search_start, search_start + match.position()), false});
      }

      // Add the matched special text
      result.push_back(SpecialInfo{match.str(), true});

      // Move the search start position
      search_start += match.position() + match.length();
    }

    // Update remaining string
    remaining = std::u32string(search_start, remaining.cend());
  }

  // Add any remaining non-matching text
  if (!remaining.empty()) {
    result.push_back(SpecialInfo{remaining, false});
  }

  return result;

}

struct LangConfig {
  std::string_view lang;
  std::string_view delimiter = "";
  std::u32string_view u32delimiter = U"";
  std::function<bool(const std::u32string &input)> has_func;
};
std::map<std::string_view, LangConfig> g_langConfigs = {
  {
    "en",
    {
      "en",
      " ",
      U" ",
      has_en,
    }
  },
  {
    "zh",
    {
      "zh",
      "",
      U"",
      has_cn,
    }
  },
  {
    "jp",
    {
      "jp",
      "",
      U"",
      has_jp,
    }
  },
  {
    "kr",
    {
       "kr",
      "",
      U"",
      has_kr,
    }
  },
  {
    "symbol",
    {
      "symbol",
      "",
      U"",
      has_symbol,
    }
  }
};

LangDetect::LangDetect() {
  std::filesystem::path tp = std::filesystem::current_path() / "res" / "tokenizer_many_lang.json";
  #ifdef _HOST_WINDOWS_
  std::ifstream file(tp.wstring());
  #else
  std::ifstream file(tp);
  #endif

  if (!file.is_open()) {
    THROW_ERROR("打开Tokenizer失败!\nFrom: {}", tp.string());
  }
  std::ostringstream oss;
  oss << file.rdbuf();
  m_tokenizer = tokenizers::Tokenizer::FromBlobJSON(oss.str());
//
//  file.seekg(0, std::ios::end);
//  std::streamsize size = file.tellg();
//  file.seekg(0, std::ios::beg);
//  std::vector<char> buffer(size);
//  std::unique_ptr<std::string> data;
//  if (file.read(buffer.data(), size)) {
//    data = std::make_unique<std::string>(buffer.data(), size);
//  } else {
//    THROW_ERROR("读取Tokenizer流失败!\nFrom: {}", tp.string());
//  }
//  m_tokenizer = tokenizers::Tokenizer::FromBlobJSON(*data);

};

std::pair<bool, std::string> LangDetect::Detect(const std::string &input) {
  bool is_reliable;
  auto resLang = CLD2::DetectLanguage(input.c_str(), input.size(), true, &is_reliable);

  auto lang = CLD2::LanguageCode(resLang);

  return {is_reliable, lang};
};

std::pair<bool, std::string>
LangDetect::detect_word(const std::string &defaultLang, const std::string &word, const std::u32string &uword) {
  auto [isReliable, lang] = Detect(word);
  if (isReliable) {
    // 可信
//    PrintDebug("[{} ,{}]{}", isReliable, lang, word);
    // 修正语言
    boost::to_lower(lang);
    if (lang == "zh-hant") {
      lang = "zh";
    }
    if (lang == "ja") {
      lang = "jp";
    }
    return {isReliable, lang};
  }
  // 检测失败
  // PrintDebug("[{} ,{}]{}", isReliable, lang, word);
  // 首先判断是否有英文
  if (has_en(uword)) {
    lang = "en";
    isReliable = true;
  }
  // 是否有中文
  if (has_cn(uword)) {
    lang = "zh";
    isReliable = true;
  }
  // 是否有日文
  if (has_jp(uword)) {
    lang = "jp";
    isReliable = true;
    return {isReliable, lang};
  }
  // 是否有韩文
  if (has_kr(uword)) {
    lang = "kr";
    isReliable = true;
    return {isReliable, lang};
  }
  if (!isReliable) {
    // 依旧不可信,判断是否为单个符号
    if (uword.size() == 1 && has_symbol(uword)) {
      // 单个符号
      lang = "symbol";
      isReliable = true;
    }
  }
  return {isReliable, lang};
}


std::vector<std::string> LangDetect::Tokenize(const std::string &text, bool add_special_tokens) {
  auto tokens = m_tokenizer->Encode(text, add_special_tokens);
  std::vector<std::string> res;
  for (auto &token: tokens) {
    res.emplace_back(m_tokenizer->Decode({token}));
  }
  return res;
};

std::vector<LangDetect::LanguageSentence>
LangDetect::DetectSplit(const std::string &defaultLang, const std::string &input) {
  if (g_langConfigs.find(defaultLang) == g_langConfigs.end()) {
    THROW_ERROR("不支持的语言: {}", defaultLang);
  }
  // 处理当前字符串的边界
  std::vector<LanguageSentence> sentences;
  auto u32input = StringToU32String(input);
  auto hsp = handle_special(u32input);
  for (auto &strp: hsp) {
    auto u32handelStr = strp.text;
    auto handelStr = U32StringToString(u32handelStr);
    if (strp.is_special) {
      if (sentences.empty()) {
        sentences.emplace_back(LanguageSentence{handelStr, u32handelStr, defaultLang});
      } else {
        if (sentences.back().language == defaultLang) {
          sentences.back().sentence += handelStr;
          sentences.back().u32sentence += u32handelStr;
        } else {
          sentences.emplace_back(LanguageSentence{handelStr, u32handelStr, defaultLang});
        }
      }
      continue;
    }
    // 分词
    auto tokens = m_tokenizer->Encode(handelStr, true);
    int ti = 0;
    for (auto &token: tokens) {
      auto word = m_tokenizer->Decode({token});
      if (ti == 0 && word == "<bos>") {
        continue;
      }
      ti++;
      auto handStr = boost::trim_copy(word);
      auto uwordRaw = StringToU32String(word);
      auto uword = U32trim(uwordRaw);
      if (uword.empty()) {
        // 空格
        // 保留空格
        if (sentences.empty()) {
          sentences.emplace_back(LanguageSentence{word, uwordRaw, defaultLang});
        } else {
          sentences.back().sentence += word;
          sentences.back().u32sentence += uwordRaw;
        }
        continue;
      }
      auto [isReliable, lang] = detect_word(defaultLang, handStr, uword);
      if (!isReliable) {
        // 不认识
        // 抛出警告
        PrintDebug("WARNING: 语言检测失败.\nFrom: {} | {}", defaultLang, handStr);
        // 警告归警告,还是要添加的
        if (sentences.empty()) {
          sentences.emplace_back(LanguageSentence{handStr, uwordRaw, defaultLang});
        } else {
          sentences.back().sentence += handStr;
          sentences.back().u32sentence += uwordRaw;
        }
        continue;
      }
      // 不支持的语言当做defaultLang
      if (g_langConfigs.find(lang) == g_langConfigs.end()) {
        lang = defaultLang;
      }
      auto &langCfg = g_langConfigs[lang];
      if (sentences.empty()) {
        if (lang == "symbol") {
          // 第一个设置为默认语言
          sentences.emplace_back(LanguageSentence{word, uwordRaw, defaultLang});
          continue;
        }
        // 补上
        sentences.emplace_back(LanguageSentence{word, uwordRaw, lang});
        continue;
      }
      if (sentences.back().language == lang) {
        // 同一个语言
        sentences.back().sentence += word;
        sentences.back().u32sentence += uwordRaw;
      } else {
        // 不同的语言

        // 如果是符号,直接附加值到最后一个句子
        if (lang == "symbol") {
          sentences.back().sentence += word;
          sentences.back().u32sentence += uwordRaw;
          continue;
        }
        sentences.emplace_back(LanguageSentence{word, uwordRaw, lang});
      }
    }
  }
  return sentences;
};

};
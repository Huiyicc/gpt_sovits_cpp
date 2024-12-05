//
// Created by 19254 on 24-11-10.
//
#include "GPTSovits/Text/TextNormalizerMap.h"
#include <regex>
#include <boost/algorithm/string.hpp>
#include <GPTSovits/Text/Coding.h>

namespace GPTSovits::Text {


std::u32string translate(const std::u32string &sentence, const std::unordered_map<char32_t, char32_t> &map) {
  std::u32string translated;
  for (auto ch: sentence) {
    int fullWidthChar = static_cast<int>(ch);
    if (map.find(fullWidthChar) != map.end()) {
      translated += static_cast<char32_t >(map.at(fullWidthChar));
    } else {
      translated += ch;
    }
  }
  return translated;
}

std::vector<std::u32string> _get_value(const std::u32string &value_string, bool use_zero = true) {
  auto stripped = value_string;
  stripped.erase(0, stripped.find_first_not_of(U'0'));
  if (stripped.empty()) {
    return {};
  } else if (stripped.size() == 1) {
    if (use_zero && stripped.size() < value_string.size()) {
      return {DIGITS[U"0"], DIGITS[stripped]};
    } else {
      return {DIGITS[stripped]};
    }
  } else {
    auto largest_unit = std::find_if(UNITS.rbegin(), UNITS.rend(),
                                     [&](const auto &pair) { return pair.first < stripped.size(); })->first;
    std::u32string first_part = value_string.substr(0, value_string.size() - largest_unit);
    std::u32string second_part = value_string.substr(value_string.size() - largest_unit);
    auto result = _get_value(first_part);
    result.push_back(UNITS[largest_unit]);
    auto second_result = _get_value(second_part);
    result.insert(result.end(), second_result.begin(), second_result.end());
    return result;
  }
}

std::u32string verbalize_digit(const std::u32string &value_string, bool alt_one = false) {
  std::u32string result;
  for (auto digit: value_string) {
    result += DIGITS[std::u32string(1, digit)];
  }
  if (alt_one) {
    size_t pos = 0;
    boost::replace_all(result, U"一", U"幺");
  }
  return result;
}

std::u32string verbalize_cardinal(const std::u32string &value_string) {
  if (value_string.empty()) {
    return U"";
  }

  std::u32string stripped = value_string;
  stripped.erase(0, stripped.find_first_not_of(U'0'));
  if (stripped.empty()) {
    return DIGITS[U"0"];
  }

  auto result_symbols = _get_value(stripped);
  if (result_symbols.size() >= 2 && result_symbols[0] == DIGITS[U"1"] && result_symbols[1] == UNITS[1]) {
    result_symbols.erase(result_symbols.begin());
  }

  std::u32string result;
  for (const auto &symbol: result_symbols) {
    result += symbol;
  }
  return result;
}

std::u32string num2str(const std::u32string &value_string) {
  size_t point_pos = value_string.find(U'.');
  std::u32string integer = value_string.substr(0, point_pos);
  std::u32string decimal = point_pos == std::string::npos ? U"" : value_string.substr(point_pos + 1);

  std::u32string result = verbalize_cardinal(integer);

  decimal.erase(decimal.find_last_not_of(U'0') + 1); // 去除末尾的零
  if (!decimal.empty()) {
    result = result.empty() ? U"零" : result;
    result += U"点" + verbalize_digit(decimal);
  }
  return result;
}

std::u32string phone2str(const std::u32string &phone_string, bool mobile = true) {
  std::u32string result;

  if (mobile) {
    // 去掉前面的 '+' 并分割字符串
    std::u32string sanitized = phone_string;
    if (sanitized.front() == U'+') {
      sanitized.erase(0, 1);
    }

    std::vector<std::u32string> sp_parts;
    boost::split(sp_parts, sanitized, boost::is_any_of(U" \t\n"), boost::token_compress_on);

    // 将每个部分转换为口语化字符串
    for (size_t i = 0; i < sp_parts.size(); ++i) {
      if (i > 0) {
        result += U"，"; // 添加分隔符
      }
      result += verbalize_digit(sp_parts[i], true);
    }
  } else {
    // 分割以 '-' 为分隔符的字符串
    std::vector<std::u32string> sil_parts;
    boost::split(sil_parts, phone_string, boost::is_any_of(U"-"), boost::token_compress_on);

    // 将每个部分转换为口语化字符串
    for (size_t i = 0; i < sil_parts.size(); ++i) {
      if (i > 0) {
        result += U"，"; // 添加分隔符
      }
      result += verbalize_digit(sil_parts[i], true);
    }
  }

  return result;
}

std::u32string _time_num2str(const std::u32string &num_string) {
  std::u32string result = num2str(num_string);
  if (num_string.front() == U'0') {
    result = DIGITS[U"0"] + result;
  }
  return result;
}

std::u32string replace_date(const srell::u32smatch &match) {
  std::u32string year = match.str(1);
  std::u32string month = match.str(3);
  std::u32string day = match.str(5);
  std::u32string result;
  if (!year.empty()) {
    result += verbalize_digit(year) + U"年";
  }
  if (!month.empty()) {
    result += verbalize_cardinal(month) + U"月";
  }
  if (!day.empty()) {
    result += verbalize_cardinal(day) + match.str(9);
  }
  return result;
}

std::u32string replace_date2(const srell::u32smatch &match) {
  std::u32string year = match[1].str();
  std::u32string month = match[3].str();
  std::u32string day = match[4].str();
  std::u32string result;

  if (!year.empty()) {
    result += verbalize_digit(year) + U"年";
  }
  if (!month.empty()) {
    result += verbalize_cardinal(month) + U"月";
  }
  if (!day.empty()) {
    result += verbalize_cardinal(day) + U"日";
  }

  return result;
}

std::u32string replace_time(const srell::u32smatch &match) {
  bool is_range = match.size() > 5;

  std::u32string hour = match[1];
  std::u32string minute = match[2];
  std::u32string second = match[4];

  std::u32string hour_2, minute_2, second_2;
  if (is_range) {
    hour_2 = match[6];
    minute_2 = match[7];
    second_2 = match[9];
  }

  std::u32string result = num2str(hour) + U"点";
  if (!minute.empty() && minute != U"0") {
    if (minute == U"30") {
      result += U"半";
    } else {
      result += _time_num2str(minute) + U"分";
    }
  }
  if (!second.empty() && second != U"0") {
    result += _time_num2str(second) + U"秒";
  }

  if (is_range) {
    result += U"至" + num2str(hour_2) + U"点";
    if (!minute_2.empty() && minute_2 != U"0") {
      if (minute_2 == U"30") {
        result += U"半";
      } else {
        result += _time_num2str(minute_2) + U"分";
      }
    }
    if (!second_2.empty() && second_2 != U"0") {
      result += _time_num2str(second_2) + U"秒";
    }
  }

  return result;
}

std::u32string replace_to_range(const srell::u32smatch &match) {
  // 获取匹配的整个字符串
  std::u32string result = match.str(0);

  // 替换 '~' 为 '至'
  std::size_t pos = result.find(U'~');
  if (pos != std::u32string::npos) {
    result.replace(pos, 1, U"至");
  }

  return result;
}

std::u32string replace_temperature(const srell::u32smatch &match) {
  std::u32string sign = match[1].str(); // 取出符号
  std::u32string temperature = match[2].str(); // 取出温度值
  std::u32string unit = match[3].str(); // 取出单位

  // 处理符号
  if (sign == U"-") {
    sign = U"零下";
  } else {
    sign.clear(); // 如果不是负号，符号清空
  }

  // 将温度值转换为中文形式
  temperature = num2str(temperature);

  // 处理单位
  if (unit == U"摄氏度") {
    unit = U"摄氏度";
  } else {
    unit = U"度"; // 默认单位
  }

  // 组装最终结果
  return sign + temperature + unit;
}

std::u32string replace_frac(const srell::u32smatch &match) {
  std::u32string sign = match[1].str();
  std::u32string nominator = match[2].str();
  std::u32string denominator = match[3].str();

  // 处理符号
  if (!sign.empty()) {
    sign = U"负";
  } else {
    sign = U"";
  }

  // 将分子和分母转换为中文
  nominator = num2str(nominator);
  denominator = num2str(denominator);

  // 构建结果字符串
  return sign + denominator + U"分之" + nominator;
}

std::u32string replace_percentage(const srell::u32smatch &match) {
  // 获取符号和百分比部分
  std::u32string sign = match[1].str();
  std::u32string percent = match[2].str();

  // 如果 sign 不为空，则设置为 "负"
  if (!sign.empty()) {
    sign = U"负";
  } else {
    sign = U"";
  }

  // 将 percent 转换为中文
  percent = num2str(percent);

  // 构建结果字符串
  return sign + U"百分之" + percent;
}

std::u32string replace_measure(const std::u32string &sentence) {
  std::u32string result = sentence;
  for (const auto &pair: measure_dict) {
    result = srell::regex_replace(result, srell::u32regex(pair.first), pair.second);
  }
  return result;
}

std::u32string replace_asmd(const srell::u32smatch &match) {
  return match.str(1) + asmd_map[match.str(8)] + match.str(9);;
}

std::u32string replace_mobile(const srell::u32smatch &match) {
  return phone2str(match.str(0)); // 使用匹配的整个字符串
}

std::u32string replace_phone(const srell::u32smatch &match) {
  return phone2str(match.str(0), false);
}

std::u32string replace_power(const srell::u32smatch &match) {
  std::u32string power_num;

  // 遍历匹配的整个字符串
  for (auto m: match.str(0)) {
    power_num += power_map[m];  // 将每个字符映射到对应的幂次
  }

  return U"的" + power_num + U"次方";
}

std::u32string replace_number(const srell::u32smatch &match) {
  std::u32string sign = match[1].str();
  std::u32string number = match[2].str();
  std::u32string pure_decimal = match[5].str();

  std::u32string result;
  if (!pure_decimal.empty()) {
    result = num2str(pure_decimal);
  } else {
    sign = sign.empty() ? U"" : U"负"; // 如果 sign 不为空，则标记为“负”
    number = num2str(number);
    result = sign + number;
  }
  return result;
}

std::u32string replace_range(const srell::u32smatch &match) {
  std::u32string first = match[1].str();
  std::u32string second = match[6].str();

  // 假设 RE_NUMBER 是一个正则表达式，用于匹配数字
  srell::u32regex RE_NUMBER(UR"(\d+)"); // 示例正则表达式，匹配数字
  first = process_with_regex(first, RE_NUMBER, replace_number);
  second = process_with_regex(second, RE_NUMBER, replace_number);

  return first + U"到" + second;
}

std::u32string replace_positive_quantifier(const srell::u32smatch &match) {
  std::u32string number = match[1].str();
  std::u32string match_2 = match[2].str();

  if (match_2 == U"+") {
    match_2 = U"多";
  }

  std::u32string quantifiers = match[3].str();
  number = num2str(number);
  if (number == U"二") {
    number = U"两";
  }

  return number + match_2 + quantifiers;
}

std::u32string replace_default_num(const srell::u32smatch &match) {
  return verbalize_digit(match[0].str(), true); // 传递 alt_one 为 true
}

std::u32string replace_negative_num(const srell::u32smatch &match) {
  std::u32string sign = match[1].str();
  std::u32string number = match[2].str();

  // 如果 sign 不为空，则标记为“负”
  sign = sign.empty() ? U"" : U"负";
  number = num2str(number);

  // 拼接结果
  return sign + number;
}

std::u32string replace_punctuation(const std::u32string &text) {
  std::u32string replaced_text = text;

  // 替换"嗯"为"恩"和"呣"为"母"
  size_t pos = replaced_text.find(U'嗯');
  while (pos != std::u32string::npos) {
    replaced_text.replace(pos, 2, U"恩");
    pos = replaced_text.find(U'嗯', pos + 2);
  }

  pos = replaced_text.find(U'呣');
  while (pos != std::u32string::npos) {
    replaced_text.replace(pos, 2, U"母");
    pos = replaced_text.find(U'呣', pos + 2);
  }

  // 构建正则表达式
  std::u32string pattern = U"(";
  for (const auto &entry: rep_map) {
    pattern += srell::regex_replace(entry.first, srell::u32regex(UR"([.*+?^${}()|\[\]\\])"), UR"(\$&)") + U"|";
  }
  pattern.pop_back(); // 去掉最后的'|'
  pattern += U")";

  srell::u32regex re(pattern);

  // 使用 lambda 函数进行替换
  replaced_text = process_with_regex(replaced_text, re, [](const srell::u32smatch &match) {
    return rep_map[match.str()];
  });

  // 移除不在指定范围内的字符
  replaced_text = srell::regex_replace(replaced_text, srell::u32regex(UR"([^\u4e00-\u9fa5,.!?\n]+)"), U"");

  return replaced_text;
}

void tranditional_to_simplified(std::u32string &text) {
  for (auto &c: text) {
    auto iter = t2s_dict.find(c);
    if (iter != t2s_dict.end()) {
      c = iter->second;
    }
  }
}

void simplified_to_traditional(std::u32string &text) {
  for (auto &c: text) {
    auto iter = s2t_dict.find(c);
    if (iter != s2t_dict.end()) {
      c = iter->second;
    }
  }
}


}
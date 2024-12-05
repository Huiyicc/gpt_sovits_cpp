//
// Created by 19254 on 24-11-10.
//

#ifndef GPT_SOVITS_CPP_TEXTNORMALIZERMAP_H
#define GPT_SOVITS_CPP_TEXTNORMALIZERMAP_H

#include <map>
#include <unordered_map>
#include <string>
#include <srell.hpp>
#include <functional>

namespace GPTSovits::Text {

extern const std::unordered_map<char32_t, char32_t> F2H_ASCII_LETTERS;
extern const std::unordered_map<char32_t, char32_t> F2H_DIGITS;
extern const std::unordered_map<char32_t, char32_t> F2H_SPACE;
extern std::map<std::u32string, std::u32string> DIGITS;
extern std::map<char, std::u32string> UNITS;
extern std::unordered_map<char32_t, char32_t> t2s_dict;
extern std::unordered_map<char32_t, char32_t> s2t_dict;
extern std::unordered_map<std::u32string, std::u32string> measure_dict;
extern std::unordered_map<std::u32string, std::u32string> asmd_map;
extern std::unordered_map<char32_t, std::u32string> power_map ;
extern std::unordered_map<std::u32string, std::u32string> rep_map;
extern std::unordered_map<std::string_view,std::string_view> pinyin_to_symbol_map;

std::u32string process_with_regex(const std::u32string &text, const srell::u32regex &re,
                                  const std::function<std::u32string(const srell::u32smatch &)> &replace_func);

std::u32string translate(const std::u32string& sentence, const std::unordered_map<char32_t, char32_t>& map);

std::u32string replace_date(const srell::u32smatch& match);
std::u32string replace_date2(const srell::u32smatch& match);
std::u32string replace_time(const srell::u32smatch& match);
std::u32string replace_to_range(const srell::u32smatch& match);
std::u32string replace_temperature(const srell::u32smatch& match);
std::u32string replace_measure(const std::u32string& sentence);
std::u32string replace_power(const srell::u32smatch &match);
std::u32string replace_frac(const srell::u32smatch& match);
std::u32string replace_percentage(const srell::u32smatch& match);
std::u32string replace_mobile(const srell::u32smatch& match);
std::u32string replace_phone(const srell::u32smatch& match);
std::u32string replace_asmd(const srell::u32smatch& match);
std::u32string replace_range(const srell::u32smatch& match);
std::u32string replace_negative_num(const srell::u32smatch& match);
std::u32string replace_number(const srell::u32smatch& match);
std::u32string replace_positive_quantifier(const srell::u32smatch& match);
std::u32string replace_default_num(const srell::u32smatch& match);
std::u32string replace_punctuation(const std::u32string &text);
std::u32string replace_consecutive_punctuation(const std::u32string& text);

std::u32string remove_commas(const srell::u32smatch &match);
std::u32string expand_dollars(const srell::u32smatch &m);
std::u32string expand_decimal_point(const srell::u32smatch &m);
std::u32string expand_ordinal(const srell::u32smatch &m);
std::u32string expand_number(const srell::u32smatch &m);

void tranditional_to_simplified(std::u32string &text);
void simplified_to_traditional(std::u32string &text);


}

#endif //GPT_SOVITS_CPP_TEXTNORMALIZERMAP_H

//
// Created by 19254 on 24-11-10.
//
#include "GPTSovits/TextNormalizer.h"
#include "GPTSovits/plog.h"
#include <iostream>
#include <map>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include <GPTSovits/TextNormalizerMap.h>
#include "utf8.h"
#include "GPTSovits/utils.h"


namespace GPTSovits {

std::u32string StringToU32String(const std::string &text) {
  std::u32string out;
  utf8::utf8to32(text.begin(), text.end(), std::back_inserter(out));
  return out;
};

std::string U32StringToString(const std::u32string &text) {
  std::string out;
  utf8::utf32to8(text.begin(), text.end(), std::back_inserter(out));
  return out;
}

std::string text_normalize(const std::string &text) {
  auto tx = TextNormalizer();
  auto sentences = tx.Normalize(StringToU32String(text));
  std::u32string dest_text;
  for (auto &sentence: sentences) {
    dest_text+=replace_punctuation(sentence);
  }
  // 避免重复标点引起的参考泄露
  dest_text = replace_consecutive_punctuation(dest_text);
  return U32StringToString(dest_text);
}

std::vector<std::u32string> TextNormalizer::split(const std::u32string &text, const std::string &lang) {
//  auto processed_text = StringToU32String(text);
  auto processed_text = text;
  // 仅针对纯中文文本
  if (lang == "zh") {
    // 移除空格
    processed_text.erase(std::remove(processed_text.begin(), processed_text.end(), U' '), processed_text.end());
    // 过滤掉特殊字符

    processed_text = srell::regex_replace(processed_text, srell::u32regex(UR"([——《》【】<>{}()（）#&@“”^_|\\])"), U"");
  }

  // 使用正则表达式进行分割
  processed_text = srell::regex_replace(processed_text, SENTENCE_SPLITOR, U"$&\n");
  processed_text = U32trim(processed_text);

  std::vector<std::u32string> sentences;
  boost::split(sentences, processed_text, boost::is_any_of(U"\n"));

  // 去掉每个句子的前后空格
  for (auto &sentence: sentences) {
    sentence = U32trim(sentence);
  }

  return sentences;
}

std::u32string TextNormalizer::postReplace(const std::u32string &sentence) {
  auto modifiedSentence = sentence;
  boost::replace_all(modifiedSentence, U"/", U"每");
  boost::replace_all(modifiedSentence, U"/", U"每");
  boost::replace_all(modifiedSentence, U"①", U"一");
  boost::replace_all(modifiedSentence, U"②", U"二");
  boost::replace_all(modifiedSentence, U"③", U"三");
  boost::replace_all(modifiedSentence, U"④", U"四");
  boost::replace_all(modifiedSentence, U"⑤", U"五");
  boost::replace_all(modifiedSentence, U"⑥", U"六");
  boost::replace_all(modifiedSentence, U"⑦", U"七");
  boost::replace_all(modifiedSentence, U"⑧", U"八");
  boost::replace_all(modifiedSentence, U"⑨", U"九");
  boost::replace_all(modifiedSentence, U"⑩", U"十");
  boost::replace_all(modifiedSentence, U"α", U"阿尔法");
  boost::replace_all(modifiedSentence, U"β", U"贝塔");
  boost::replace_all(modifiedSentence, U"γ", U"伽玛");
  boost::replace_all(modifiedSentence, U"Γ", U"伽玛");
  boost::replace_all(modifiedSentence, U"δ", U"德尔塔");
  boost::replace_all(modifiedSentence, U"Δ", U"德尔塔");
  boost::replace_all(modifiedSentence, U"ε", U"艾普西龙");
  boost::replace_all(modifiedSentence, U"ζ", U"捷塔");
  boost::replace_all(modifiedSentence, U"η", U"依塔");
  boost::replace_all(modifiedSentence, U"θ", U"西塔");
  boost::replace_all(modifiedSentence, U"Θ", U"西塔");
  boost::replace_all(modifiedSentence, U"ι", U"艾欧塔");
  boost::replace_all(modifiedSentence, U"κ", U"喀帕");
  boost::replace_all(modifiedSentence, U"λ", U"拉姆达");
  boost::replace_all(modifiedSentence, U"Λ", U"拉姆达");
  boost::replace_all(modifiedSentence, U"μ", U"缪");
  boost::replace_all(modifiedSentence, U"ν", U"拗");
  boost::replace_all(modifiedSentence, U"ξ", U"克西");
  boost::replace_all(modifiedSentence, U"Ξ", U"克西");
  boost::replace_all(modifiedSentence, U"ο", U"欧米克伦");
  boost::replace_all(modifiedSentence, U"π", U"派");
  boost::replace_all(modifiedSentence, U"Π", U"派");
  boost::replace_all(modifiedSentence, U"ρ", U"肉");
  boost::replace_all(modifiedSentence, U"ς", U"西格玛");
  boost::replace_all(modifiedSentence, U"Σ", U"西格玛");
  boost::replace_all(modifiedSentence, U"σ", U"西格玛");
  boost::replace_all(modifiedSentence, U"τ", U"套");
  boost::replace_all(modifiedSentence, U"υ", U"宇普西龙");
  boost::replace_all(modifiedSentence, U"φ", U"服艾");
  boost::replace_all(modifiedSentence, U"Φ", U"服艾");
  boost::replace_all(modifiedSentence, U"χ", U"器");
  boost::replace_all(modifiedSentence, U"ψ", U"普赛");
  boost::replace_all(modifiedSentence, U"Ψ", U"普赛");
  boost::replace_all(modifiedSentence, U"ω", U"欧米伽");
  boost::replace_all(modifiedSentence, U"Ω", U"欧米伽");
  boost::replace_all(modifiedSentence, U"+", U"加");
  boost::replace_all(modifiedSentence, U"-", U"减");
  boost::replace_all(modifiedSentence, U"×", U"乘");
  boost::replace_all(modifiedSentence, U"÷", U"除");
  boost::replace_all(modifiedSentence, U"=", U"等");
  modifiedSentence = srell::regex_replace(modifiedSentence, srell::u32regex(UR"([-——《》【】<=>{}()（）#&@“”^_|\\])"), U"");
  return modifiedSentence;
}

std::vector<std::u32string> TextNormalizer::Normalize(const std::u32string &text) {
  auto sentences = split(text);
  std::vector<std::u32string> out;
  for (auto &sentence: sentences) {
    out.emplace_back(NormalizeSentence(sentence));
  }
  return out;
}


std::u32string TextNormalizer::NormalizeSentence(const std::u32string &sentence) {
  auto modifiedSentence = sentence;
  tranditional_to_simplified(modifiedSentence);

  modifiedSentence = translate(modifiedSentence, F2H_ASCII_LETTERS);
  modifiedSentence = translate(modifiedSentence, F2H_DIGITS);
  modifiedSentence = translate(modifiedSentence, F2H_SPACE);

  // number related NSW verbalization
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(
    UR"((\d{4}|\d{2})年((0?[1-9]|1[0-2])月)?(((0?[1-9])|((1|2)[0-9])|30|31)([日号]))?)"), replace_date);
  modifiedSentence = process_with_regex(modifiedSentence,
                                        srell::u32regex(UR"((\d{4})([- /.])(0[1-9]|1[012])\2(0[1-9]|[12][0-9]|3[01]))"),
                                        replace_date2);

  // range first
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(
                                          UR"(([0-1]?[0-9]|2[0-3]):([0-5][0-9])(:([0-5][0-9]))?(~|-)([0-1]?[0-9]|2[0-3]):([0-5][0-9])(:([0-5][0-9]))?)"),
                                        replace_time);
  modifiedSentence = process_with_regex(modifiedSentence,
                                        srell::u32regex(UR"(([0-1]?[0-9]|2[0-3]):([0-5][0-9])(:([0-5][0-9]))?)"),
                                        replace_time);

  // 处理~波浪号作为至的替换
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(
                                          UR"(((-?)((\d+)(\.\d+)?)|(\.(\d+)))(%|°C|℃|度|摄氏度|cm2|cm²|cm3|cm³|cm|db|ds|kg|km|m2|m²|m³|m3|ml|m|mm|s)[~]((-?)((\d+)(\.\d+)?)|(\.(\d+)))(%|°C|℃|度|摄氏度|cm2|cm²|cm3|cm³|cm|db|ds|kg|km|m2|m²|m³|m3|ml|m|mm|s))"),
                                        replace_to_range);
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(UR"((-?)(\d+(\.\d+)?)(°C|℃|度|摄氏度))"),
                                        replace_temperature);
  modifiedSentence = replace_measure(modifiedSentence);

  // 处理数学运算
  srell::u32regex re_asmd(
    UR"(((-?)((\d+)(\.\d+)?[⁰¹²³⁴⁵⁶⁷⁸⁹ˣʸⁿ]*)|(\.\d+[⁰¹²³⁴⁵⁶⁷⁸⁹ˣʸⁿ]*)|([A-Za-z][⁰¹²³⁴⁵⁶⁷⁸⁹ˣʸⁿ]*))([\+\-×÷=])((-?)((\d+)(\.\d+)?[⁰¹²³⁴⁵⁶⁷⁸⁹ˣʸⁿ]*)|(\.\d+[⁰¹²³⁴⁵⁶⁷⁸⁹ˣʸⁿ]*)|([A-Za-z][⁰¹²³⁴⁵⁶⁷⁸⁹ˣʸⁿ]*)))",
    srell::u32regex::flag_type::ECMAScript);
  std::u32string asmd_result;
  srell::u32smatch asmd_match;
  while (srell::regex_search(modifiedSentence, asmd_match, re_asmd)) {
    asmd_result += asmd_match.prefix().str();
    asmd_result += replace_asmd(asmd_match);
    modifiedSentence = asmd_match.suffix().str();
  }
  asmd_result += modifiedSentence;
  modifiedSentence = asmd_result;
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(UR"([⁰¹²³⁴⁵⁶⁷⁸⁹ˣʸⁿ]+)"), replace_power);

  // 分数表达式
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(UR"((-?)(\d+)/(\d+))"), replace_frac);
  // 百分数表达式
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(UR"((-?)(\d+(\.\d+)?)%)"),
                                        replace_percentage);

  /*
    规范化固话/手机号码
    手机
    http://www.jihaoba.com/news/show/13680
    移动：139、138、137、136、135、134、159、158、157、150、151、152、188、187、182、183、184、178、198
    联通：130、131、132、156、155、186、185、176
    电信：133、153、189、180、181、177
   */
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(
    UR"((?<!\d)((\+?86 ?)?1([38]\d|5[0-35-9]|7[678]|9[89])\d{8})(?!\d))"), replace_mobile);
  modifiedSentence = process_with_regex(modifiedSentence,
                                        srell::u32regex(UR"((?<!\d)((0(10|2[1-3]|[3-9]\d{2})-?)?[1-9]\d{6,7})(?!\d))"),
                                        replace_phone);
  // 全国统一的号码400开头
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(UR"((400)(-)?\d{3}(-)?\d{4})"),
                                        replace_phone);
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(
                                          UR"(((?<![\d\+\-×÷=])((-?)((\d+)(\.\d+)?))[-~]((-?)((\d+)(\.\d+)?))(?![\d\+\-×÷=])))", srell::regex::extended),
                                        replace_range);

  // 整数表达式
  // 带负号的整数 -10
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(UR"((-)(\d+))"), replace_negative_num);
  // 纯小数
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(UR"((-?)((\d+)(\.\d+))|(\.(\d+)))"),
                                        replace_number);
  // 正整数 + 量词
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(
                                          UR"((\d+)([多余几\+])?(处|台|架|枚|趟|幅|平|方|堵|间|床|株|批|项|例|列|篇|栋|注|亩|封|艘|把|目|套|段|人|所|朵|匹|张|座|回|场|尾|条|个|首|阙|阵|网|炮|顶|丘|棵|只|支|袭|辆|挑|担|颗|壳|窠|曲|墙|群|腔|砣|座|客|贯|扎|捆|刀|令|打|手|罗|坡|山|岭|江|溪|钟|队|单|双|对|出|口|头|脚|板|跳|枝|件|贴|针|线|管|名|位|身|堂|课|本|页|家|户|层|丝|毫|厘|分|钱|两|斤|担|铢|石|钧|锱|忽|(千|毫|微)克|毫|厘|(公)分|分|寸|尺|丈|里|寻|常|铺|程|(千|分|厘|毫|微)米|米|撮|勺|合|升|斗|石|盘|碗|碟|叠|桶|笼|盆|盒|杯|钟|斛|锅|簋|篮|盘|桶|罐|瓶|壶|卮|盏|箩|箱|煲|啖|袋|钵|年|月|日|季|刻|时|周|天|秒|分|小时|旬|纪|岁|世|更|夜|春|夏|秋|冬|代|伏|辈|丸|泡|粒|颗|幢|堆|条|根|支|道|面|片|张|颗|块|元|(亿|千万|百万|万|千|百)|(亿|千万|百万|万|千|百|美|)元|(亿|千万|百万|万|千|百|十|)吨|(亿|千万|百万|万|千|百|)块|角|毛|分))"),
                                        replace_positive_quantifier);
  // 编号-无符号整形
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(UR"(\d{3}\d*)"), replace_default_num);
  modifiedSentence = process_with_regex(modifiedSentence, srell::u32regex(UR"((-?)((\d+)(\.\d+)?)|(\.(\d+)))"),
                                        replace_number);

  modifiedSentence = postReplace(modifiedSentence);
  return modifiedSentence;
}


};



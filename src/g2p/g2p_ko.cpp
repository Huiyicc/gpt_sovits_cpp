//
// Created by 19254 on 24-12-26.
//
#include <GPTSovits/G2P/g2p.h>
#include <GPTSovits/G2P/g2p_ko.h>
#include <GPTSovits/Text/Coding.h>
#include <filesystem>
#include <srell.hpp>

namespace GPTSovits {
extern std::filesystem::path g_globalResourcesPath;
}

namespace GPTSovits::G2P {

std::u32string ko_latin_to_hangul(const std::u32string &text) {
  struct LatinToHangul {
    srell::u32regex pattern;
    std::u32string hangul;
  };
  static std::vector<std::tuple<srell::u32regex, std::u32string>> latin_to_hangul = {
    {srell::u32regex(U"a", srell::regex_constants::icase), U"에이"},
    {srell::u32regex(U"b", srell::regex_constants::icase), U"비"},
    {srell::u32regex(U"c", srell::regex_constants::icase), U"시"},
    {srell::u32regex(U"d", srell::regex_constants::icase), U"디"},
    {srell::u32regex(U"e", srell::regex_constants::icase), U"이"},
    {srell::u32regex(U"f", srell::regex_constants::icase), U"에프"},
    {srell::u32regex(U"g", srell::regex_constants::icase), U"지"},
    {srell::u32regex(U"h", srell::regex_constants::icase), U"에이치"},
    {srell::u32regex(U"i", srell::regex_constants::icase), U"아이"},
    {srell::u32regex(U"j", srell::regex_constants::icase), U"제이"},
    {srell::u32regex(U"k", srell::regex_constants::icase), U"케이"},
    {srell::u32regex(U"l", srell::regex_constants::icase), U"엘"},
    {srell::u32regex(U"m", srell::regex_constants::icase), U"엠"},
    {srell::u32regex(U"n", srell::regex_constants::icase), U"엔"},
    {srell::u32regex(U"o", srell::regex_constants::icase), U"오"},
    {srell::u32regex(U"p", srell::regex_constants::icase), U"피"},
    {srell::u32regex(U"q", srell::regex_constants::icase), U"큐"},
    {srell::u32regex(U"r", srell::regex_constants::icase), U"아르"},
    {srell::u32regex(U"s", srell::regex_constants::icase), U"에스"},
    {srell::u32regex(U"t", srell::regex_constants::icase), U"티"},
    {srell::u32regex(U"u", srell::regex_constants::icase), U"유"},
    {srell::u32regex(U"v", srell::regex_constants::icase), U"브이"},
    {srell::u32regex(U"w", srell::regex_constants::icase), U"더블유"},
    {srell::u32regex(U"x", srell::regex_constants::icase), U"엑스"},
    {srell::u32regex(U"y", srell::regex_constants::icase), U"와이"},
    {srell::u32regex(U"z", srell::regex_constants::icase), U"제"},
  };
  std::u32string result = text;
  for (auto &[regex, replacement]: latin_to_hangul) {
    result = srell::regex_replace(result, regex, replacement);
  }

  return result;
}

G2PRes G2PKO::_cleanText(const std::string &text) {
  auto str = Text::StringToU32String(text);
  str = ko_latin_to_hangul(str);

  return {};
};

}// namespace GPTSovits::G2P
//
// Created by 19254 on 24-11-11.
//
#include <GPTSovits/tone_sandhi.h>
#include <algorithm>
#include <utility>
#include "cppjieba/Jieba.hpp"
#include <GPTSovits/Text/TextNormalizer/zh.h>
#include "GPTSovits/Text/Utils.h"
#include "GPTSovits/Text/Coding.h"

namespace GPTSovits {

ToneSandhi::ToneSandhi(std::shared_ptr<cppjieba::Jieba> jieba) : m_jieba(std::move(jieba)) {

}

std::vector<std::string> ToneSandhi::bu_sandhi(const std::u32string &word, std::vector<std::string> &finals) {
  // e.g. 看不懂
  if (word.length() == 3 && word[1] == U'不') {
    finals[1] = finals[1].substr(0, finals[1].length() - 1) + "5";
  } else {
    for (size_t i = 0; i < word.length(); ++i) {
      // "不" before tone4 should be bu2, e.g. 不怕
      if (word[i] == U'不' && i + 1 < word.length() && finals[i + 1].back() == '4') {
        finals[i] = finals[i].substr(0, finals[i].length() - 1) + "2";
      }
    }
  }
  return finals;
}

std::vector<std::u32string> ToneSandhi::splitWord(const std::u32string &word) {
  std::vector<std::string> wordList;
  auto uWord = Text::U32StringToString(word);
  m_jieba->CutForSearch(uWord, wordList);

  // Sort wordList by length in ascending order
  sort(wordList.begin(), wordList.end(), [](const std::string &a, const std::string &b) {
    return a.size() < b.size();
  });

  std::string firstSubword = wordList[0];
  size_t firstBeginIdx = uWord.find(firstSubword);

  std::vector<std::u32string> newWordList;
  if (firstBeginIdx == 0) {
    std::string secondSubword = uWord.substr(firstSubword.size());
    newWordList = {Text::StringToU32String(firstSubword), Text::StringToU32String(secondSubword)};
  } else {
    std::string secondSubword = uWord.substr(0, word.size() - firstSubword.size());
    newWordList = {Text::StringToU32String(secondSubword), Text::StringToU32String(firstSubword)};
  }

  return newWordList;
}


std::vector<std::string>
ToneSandhi::neural_sandhi(const std::u32string &word, const std::string &pos, std::vector<std::string> &finals) {
  // Reduplication words for n. and v. e.g. 奶奶, 试试, 旺旺
  for (int j = 0; j < word.size(); ++j) {
    auto item = word[j];
    if (j - 1 >= 0
        && item == word[j - 1]
        && (pos[0] == 'n' || pos[0] == 'v' || pos[0] == 'a')
        && (must_not_neural_tone_words.find(word) == must_not_neural_tone_words.end())
      ) {
      finals[j] = finals[j].substr(0, finals[j].size() - 1) + "5";
    }

  }
  auto safeSlice = [](const std::u32string& str, int start, int end)->std::u32string {
    // 如果起始索引小于 0，则从 0 开始
    if (start < 0) {
      start = 0;
    }
    // 如果结束索引超过字符串长度，则设为字符串长度
    if (end > str.length()) {
      end = str.length();
    }
    // 如果起始索引超过结束索引，返回空字符串
    if (start >= end) {
      return U"";
    }
    // 返回切片
    return str.substr(start, end - start);
  };

  int ge_idx = word.find(U'个');
  if (word.size() >= 1 &&
      std::u32string_view(U"吧呢哈啊呐噻嘛吖嗨呐哦哒额滴哩哟喽啰耶喔诶").find(word.back()) != std::u32string::npos) {
    finals.back() = finals.back().substr(0, finals.back().size() - 1) + "5";
  } else if (word.size() >= 1 && std::u32string_view(U"的地得").find(word.back()) != std::u32string::npos) {
    finals.back() = finals.back().substr(0, finals.back().size() - 1) + "5";
  } else if (word.size() == 1 && std::u32string_view(U"了着过").find(word) != std::u32string::npos &&
             (pos == "ul" || pos == "uz" || pos == "ug")) {
    finals.back() = finals.back().substr(0, finals.back().size() - 1) + "5";
  } else if (word.size() > 1 && std::u32string_view(U"们子").find(word.back()) != std::u32string::npos &&
             (pos == "r" || pos == "n") &&
             std::find(must_not_neural_tone_words.begin(), must_not_neural_tone_words.end(), word) ==
             must_not_neural_tone_words.end()) {
    finals.back() = finals.back().substr(0, finals.back().size() - 1) + "5";
  } else if (word.size() > 1 && std::u32string_view(U"上下里").find(word.back()) != std::u32string::npos &&
             (pos == "s" || pos == "l" || pos == "f")) {
    finals.back() = finals.back().substr(0, finals.back().size() - 1) + "5";
  } else if (word.size() > 1 && std::u32string_view(U"来去").find(word.back()) != std::u32string::npos &&
             std::u32string_view(U"上下进出回过起开").find(word[word.size() - 2]) != std::u32string::npos) {
    finals.back() = finals.back().substr(0, finals.back().size() - 1) + "5";
  } else if ((ge_idx >= 1 && (Text::safe_isdigit(word[ge_idx - 1]) ||
                              std::u32string_view(U"几有两半多各整每做是").find(word[ge_idx - 1]) !=
                              std::u32string::npos)) || word == U"个") {
    finals[ge_idx] = finals[ge_idx].substr(0, finals[ge_idx].size() - 1) + "5";
  } else {
    if (must_neural_tone_words.find(word) != must_neural_tone_words.end() ||
        must_neural_tone_words.find(safeSlice(word,word.size() - 2,word.size())) != must_neural_tone_words.end()) {
      finals.back() = finals.back().substr(0, finals.back().size() - 1) + "5";
    }
  }

  auto word_list = splitWord(word);
  std::vector<std::vector<std::string>> finals_list = {
    std::vector<std::string>(finals.begin(), finals.begin() + word_list[0].size()),
    std::vector<std::string>(finals.begin() + word_list[0].size(), finals.end())};

  for (int i = 0; i < word_list.size(); ++i) {
    if (must_neural_tone_words.find(word_list[i]) != must_neural_tone_words.end() ||
        must_neural_tone_words.find(safeSlice(word_list[i],word_list[i].size()-2,word_list[i].size())) !=
        must_neural_tone_words.end()) {
      finals_list[i].back() = finals_list[i].back().substr(0, finals_list[i].back().size() - 1) + "5";
    }
  }

  finals.clear();
  for (const auto &fl: finals_list) {
    finals.insert(finals.end(), fl.begin(), fl.end());
  }

  return finals;
}


std::vector<std::string> ToneSandhi::yi_sandhi(const std::u32string &word, std::vector<std::string> &finals) {
  // "一" in number sequences, e.g. 一零零, 二一零
  if (word.find(U'一') != std::u32string::npos && std::all_of(word.begin(), word.end(), [](char32_t c) {
    return c == U'一' || Text::safe_isdigit(char(c));
  })) {
    return finals;
  }
    // "一" between reduplication words should be yi5, e.g. 看一看
  else if (word.length() == 3 && word[1] == U'一' && word[0] == word[2]) {
    finals[1] = finals[1].substr(0, finals[1].length() - 1) + "5";
  }
    // when "一" is ordinal word, it should be yi1
  else if (word.substr(0, 2) == U"第一") {
    finals[1] = finals[1].substr(0, finals[1].length() - 1) + "1";
  } else {
    for (size_t i = 0; i < word.length(); ++i) {
      if (word[i] == U'一' && i + 1 < word.length()) {
        // "一" before tone4 should be yi2, e.g. 一段
        if (finals[i + 1].back() == '4') {
          finals[i] = finals[i].substr(0, finals[i].length() - 1) + "2";
        }
          // "一" before non-tone4 should be yi4, e.g. 一天
        else {
          // "一" 后面如果是标点，还读一声
          if (punc.find(word[i + 1]) == std::u32string::npos) {
            finals[i] = finals[i].substr(0, finals[i].length() - 1) + "4";
          }
        }
      }
    }
  }
  return finals;
}

std::vector<std::string> ToneSandhi::three_sandhi(const std::u32string& word, std::vector<std::string> finals) {
  if (word.length() == 2 && all_tone_three(finals)) {
    finals[0].back() = '2';
  } else if (word.length() == 3) {
    auto word_list = splitWord(word);
    if (all_tone_three(finals)) {
      if (word_list[0].length() == 2) {
        finals[0].back() = '2';
        finals[1].back() = '2';
      } else if (word_list[0].length() == 1) {
        finals[1].back() = '2';
      }
    } else {
      std::vector<std::vector<std::string>> finals_list = {
        std::vector<std::string>(finals.begin(), finals.begin() + word_list[0].length()),
        std::vector<std::string>(finals.begin() + word_list[0].length(), finals.end())
      };
      if (finals_list.size() == 2) {
        for (size_t i = 0; i < finals_list.size(); ++i) {
          auto& sub = finals_list[i];
          if (all_tone_three(sub) && sub.size() == 2) {
            sub[0].back() = '2';
          } else if (i == 1 && !all_tone_three(sub) && sub[0].back() == '3' && finals_list[0].back().back() == '3') {
            finals_list[0].back().back() = '2';
          }
        }
        finals.clear();
        for (const auto& sub : finals_list) {
          finals.insert(finals.end(), sub.begin(), sub.end());
        }
      }
    }
  } else if (word.length() == 4) {
    std::vector<std::vector<std::string>> finals_list = {
      std::vector<std::string>(finals.begin(), finals.begin() + 2),
      std::vector<std::string>(finals.begin() + 2, finals.end())
    };
    finals.clear();
    for (auto& sub : finals_list) {
      if (all_tone_three(sub)) {
        sub[0].back() = '2';
      }
      finals.insert(finals.end(), sub.begin(), sub.end());
    }
  }
  return finals;
}

bool ToneSandhi::all_tone_three(const std::vector<std::string>& finals) {
  return std::all_of(finals.begin(), finals.end(), [](const std::string& x) { return x.back() == '3'; });
}

std::vector<std::string>
ToneSandhi::modified_tone(const std::u32string &word, const std::string &pos,
                          std::vector<std::string> &finals) {
  auto r = bu_sandhi(word, finals);
  r = yi_sandhi(word, r);
  r = neural_sandhi(word, pos, r);
  r = three_sandhi(word, r);
  return std::move(r);
}


std::set<std::u32string> ToneSandhi::must_not_neural_tone_words = {
  U"男子",
  U"女子",
  U"分子",
  U"原子",
  U"量子",
  U"莲子",
  U"石子",
  U"瓜子",
  U"电子",
  U"人人",
  U"虎虎",
  U"幺幺",
  U"干嘛",
  U"学子",
  U"哈哈",
  U"数数",
  U"袅袅",
  U"局地",
  U"以下",
  U"娃哈哈",
  U"花花草草",
  U"留得",
  U"耕地",
  U"想想",
  U"熙熙",
  U"攘攘",
  U"卵子",
  U"死死",
  U"冉冉",
  U"恳恳",
  U"佼佼",
  U"吵吵",
  U"打打",
  U"考考",
  U"整整",
  U"莘莘",
  U"落地",
  U"算子",
  U"家家户户",
  U"青青",
};

std::set<std::u32string> ToneSandhi::must_neural_tone_words = {
  U"麻烦",
  U"麻利",
  U"鸳鸯",
  U"高粱",
  U"骨头",
  U"骆驼",
  U"马虎",
  U"首饰",
  U"馒头",
  U"馄饨",
  U"风筝",
  U"难为",
  U"队伍",
  U"阔气",
  U"闺女",
  U"门道",
  U"锄头",
  U"铺盖",
  U"铃铛",
  U"铁匠",
  U"钥匙",
  U"里脊",
  U"里头",
  U"部分",
  U"那么",
  U"道士",
  U"造化",
  U"迷糊",
  U"连累",
  U"这么",
  U"这个",
  U"运气",
  U"过去",
  U"软和",
  U"转悠",
  U"踏实",
  U"跳蚤",
  U"跟头",
  U"趔趄",
  U"财主",
  U"豆腐",
  U"讲究",
  U"记性",
  U"记号",
  U"认识",
  U"规矩",
  U"见识",
  U"裁缝",
  U"补丁",
  U"衣裳",
  U"衣服",
  U"衙门",
  U"街坊",
  U"行李",
  U"行当",
  U"蛤蟆",
  U"蘑菇",
  U"薄荷",
  U"葫芦",
  U"葡萄",
  U"萝卜",
  U"荸荠",
  U"苗条",
  U"苗头",
  U"苍蝇",
  U"芝麻",
  U"舒服",
  U"舒坦",
  U"舌头",
  U"自在",
  U"膏药",
  U"脾气",
  U"脑袋",
  U"脊梁",
  U"能耐",
  U"胳膊",
  U"胭脂",
  U"胡萝",
  U"胡琴",
  U"胡同",
  U"聪明",
  U"耽误",
  U"耽搁",
  U"耷拉",
  U"耳朵",
  U"老爷",
  U"老实",
  U"老婆",
  U"老头",
  U"老太",
  U"翻腾",
  U"罗嗦",
  U"罐头",
  U"编辑",
  U"结实",
  U"红火",
  U"累赘",
  U"糨糊",
  U"糊涂",
  U"精神",
  U"粮食",
  U"簸箕",
  U"篱笆",
  U"算计",
  U"算盘",
  U"答应",
  U"笤帚",
  U"笑语",
  U"笑话",
  U"窟窿",
  U"窝囊",
  U"窗户",
  U"稳当",
  U"稀罕",
  U"称呼",
  U"秧歌",
  U"秀气",
  U"秀才",
  U"福气",
  U"祖宗",
  U"砚台",
  U"码头",
  U"石榴",
  U"石头",
  U"石匠",
  U"知识",
  U"眼睛",
  U"眯缝",
  U"眨巴",
  U"眉毛",
  U"相声",
  U"盘算",
  U"白净",
  U"痢疾",
  U"痛快",
  U"疟疾",
  U"疙瘩",
  U"疏忽",
  U"畜生",
  U"生意",
  U"甘蔗",
  U"琵琶",
  U"琢磨",
  U"琉璃",
  U"玻璃",
  U"玫瑰",
  U"玄乎",
  U"狐狸",
  U"状元",
  U"特务",
  U"牲口",
  U"牙碜",
  U"牌楼",
  U"爽快",
  U"爱人",
  U"热闹",
  U"烧饼",
  U"烟筒",
  U"烂糊",
  U"点心",
  U"炊帚",
  U"灯笼",
  U"火候",
  U"漂亮",
  U"滑溜",
  U"溜达",
  U"温和",
  U"清楚",
  U"消息",
  U"浪头",
  U"活泼",
  U"比方",
  U"正经",
  U"欺负",
  U"模糊",
  U"槟榔",
  U"棺材",
  U"棒槌",
  U"棉花",
  U"核桃",
  U"栅栏",
  U"柴火",
  U"架势",
  U"枕头",
  U"枇杷",
  U"机灵",
  U"本事",
  U"木头",
  U"木匠",
  U"朋友",
  U"月饼",
  U"月亮",
  U"暖和",
  U"明白",
  U"时候",
  U"新鲜",
  U"故事",
  U"收拾",
  U"收成",
  U"提防",
  U"挖苦",
  U"挑剔",
  U"指甲",
  U"指头",
  U"拾掇",
  U"拳头",
  U"拨弄",
  U"招牌",
  U"招呼",
  U"抬举",
  U"护士",
  U"折腾",
  U"扫帚",
  U"打量",
  U"打算",
  U"打点",
  U"打扮",
  U"打听",
  U"打发",
  U"扎实",
  U"扁担",
  U"戒指",
  U"懒得",
  U"意识",
  U"意思",
  U"情形",
  U"悟性",
  U"怪物",
  U"思量",
  U"怎么",
  U"念头",
  U"念叨",
  U"快活",
  U"忙活",
  U"志气",
  U"心思",
  U"得罪",
  U"张罗",
  U"弟兄",
  U"开通",
  U"应酬",
  U"庄稼",
  U"干事",
  U"帮手",
  U"帐篷",
  U"希罕",
  U"师父",
  U"师傅",
  U"巴结",
  U"巴掌",
  U"差事",
  U"工夫",
  U"岁数",
  U"屁股",
  U"尾巴",
  U"少爷",
  U"小气",
  U"小伙",
  U"将就",
  U"对头",
  U"对付",
  U"寡妇",
  U"家伙",
  U"客气",
  U"实在",
  U"官司",
  U"学问",
  U"学生",
  U"字号",
  U"嫁妆",
  U"媳妇",
  U"媒人",
  U"婆家",
  U"娘家",
  U"委屈",
  U"姑娘",
  U"姐夫",
  U"妯娌",
  U"妥当",
  U"妖精",
  U"奴才",
  U"女婿",
  U"头发",
  U"太阳",
  U"大爷",
  U"大方",
  U"大意",
  U"大夫",
  U"多少",
  U"多么",
  U"外甥",
  U"壮实",
  U"地道",
  U"地方",
  U"在乎",
  U"困难",
  U"嘴巴",
  U"嘱咐",
  U"嘟囔",
  U"嘀咕",
  U"喜欢",
  U"喇嘛",
  U"喇叭",
  U"商量",
  U"唾沫",
  U"哑巴",
  U"哈欠",
  U"哆嗦",
  U"咳嗽",
  U"和尚",
  U"告诉",
  U"告示",
  U"含糊",
  U"吓唬",
  U"后头",
  U"名字",
  U"名堂",
  U"合同",
  U"吆喝",
  U"叫唤",
  U"口袋",
  U"厚道",
  U"厉害",
  U"千斤",
  U"包袱",
  U"包涵",
  U"匀称",
  U"勤快",
  U"动静",
  U"动弹",
  U"功夫",
  U"力气",
  U"前头",
  U"刺猬",
  U"刺激",
  U"别扭",
  U"利落",
  U"利索",
  U"利害",
  U"分析",
  U"出息",
  U"凑合",
  U"凉快",
  U"冷战",
  U"冤枉",
  U"冒失",
  U"养活",
  U"关系",
  U"先生",
  U"兄弟",
  U"便宜",
  U"使唤",
  U"佩服",
  U"作坊",
  U"体面",
  U"位置",
  U"似的",
  U"伙计",
  U"休息",
  U"什么",
  U"人家",
  U"亲戚",
  U"亲家",
  U"交情",
  U"云彩",
  U"事情",
  U"买卖",
  U"主意",
  U"丫头",
  U"丧气",
  U"两口",
  U"东西",
  U"东家",
  U"世故",
  U"不由",
  U"不在",
  U"下水",
  U"下巴",
  U"上头",
  U"上司",
  U"丈夫",
  U"丈人",
  U"一辈",
  U"那个",
  U"菩萨",
  U"父亲",
  U"母亲",
  U"咕噜",
  U"邋遢",
  U"费用",
  U"冤家",
  U"甜头",
  U"介绍",
  U"荒唐",
  U"大人",
  U"泥鳅",
  U"幸福",
  U"熟悉",
  U"计划",
  U"扑腾",
  U"蜡烛",
  U"姥爷",
  U"照顾",
  U"喉咙",
  U"吉他",
  U"弄堂",
  U"蚂蚱",
  U"凤凰",
  U"拖沓",
  U"寒碜",
  U"糟蹋",
  U"倒腾",
  U"报复",
  U"逻辑",
  U"盘缠",
  U"喽啰",
  U"牢骚",
  U"咖喱",
  U"扫把",
  U"惦记",
};

}
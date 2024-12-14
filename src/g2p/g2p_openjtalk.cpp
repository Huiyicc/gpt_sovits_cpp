//
// Created by 19254 on 24-12-13.
//
#include "jpcommon/jpcommon.h"
#include "mecab/src/mecab.h"
#include "njd/njd.h"
#include "text2mecab/text2mecab.h"
#include <GPTSovits/G2P/g2p_ja.h>
#include <GPTSovits/Text/Coding.h>
#include <GPTSovits/Utils/exception.h>
#include <GPTSovits/plog.h>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <mecab2njd/mecab2njd.h>
#include <njd2jpcommon/njd2jpcommon.h>
#include <njd_set_accent_phrase/njd_set_accent_phrase.h>
#include <njd_set_accent_type/njd_set_accent_type.h>
#include <njd_set_digit/njd_set_digit.h>
#include <njd_set_long_vowel/njd_set_long_vowel.h>
#include <njd_set_pronunciation/njd_set_pronunciation.h>
#include <njd_set_unvoiced_vowel/njd_set_unvoiced_vowel.h>

namespace GPTSovits {
extern std::filesystem::path g_globalResourcesPath;
}

namespace GPTSovits::G2P {

OpenJTalk::Feature node2feature(NJDNode *node) {
  return {
    node->string,
    node->pos,
    node->pos_group1,
    node->pos_group2,
    node->pos_group3,
    node->ctype,
    node->cform,
    node->orig,
    node->read,
    node->pron,
    node->acc,
    node->mora_size,
    node->chain_rule,
    node->chain_flag,
  };
}

void feature2njd(NJD *njd, const std::vector<OpenJTalk::Feature> &features) {
  NJDNode *node;
  for (auto &feature_node: features) {
    node = (NJDNode *) calloc(1, sizeof(NJDNode));
    NJDNode_initialize(node);
    NJDNode_set_string(node, feature_node.string.c_str());
    NJDNode_set_pos(node, feature_node.pos.c_str());
    NJDNode_set_pos_group1(node, feature_node.pos_group1.c_str());
    NJDNode_set_pos_group2(node, feature_node.pos_group2.c_str());
    NJDNode_set_pos_group3(node, feature_node.pos_group3.c_str());
    NJDNode_set_ctype(node, feature_node.ctype.c_str());
    NJDNode_set_cform(node, feature_node.cform.c_str());
    NJDNode_set_orig(node, feature_node.orig.c_str());
    NJDNode_set_read(node, feature_node.read.c_str());
    NJDNode_set_pron(node, feature_node.pron.c_str());
    NJDNode_set_acc(node, feature_node.acc);
    NJDNode_set_mora_size(node, feature_node.mora_size);
    NJDNode_set_chain_rule(node, feature_node.chain_rule.c_str());
    NJDNode_set_chain_flag(node, feature_node.chain_flag);
    NJD_push_node(njd, node);
  }
}

std::vector<OpenJTalk::Feature> njd2feature(NJD *njd) {
  NJDNode *node = njd->head;
  std::vector<OpenJTalk::Feature> result;
  while (node) {
    result.push_back(node2feature(node));
    node = node->next;
  }
  return result;
}

int Mecab_load_with_userdic(Mecab *m, const std::string &dict_path, const std::string &user_dict) {
  if (user_dict.empty()) {
    return Mecab_load(m, dict_path.c_str());
  }
  if (!m || dict_path.empty()) {
    return NULL;
  }
  Mecab_clear(m);
  char *argv[] = {(char *) "mecab", (char *) "-d", (char *) dict_path.c_str(), (char *) "-u",
                  (char *) user_dict.c_str()};

  auto *model = MeCab::createModel(5, argv);
  if (!model) {
    return NULL;
  }
  m->model = model;
  auto tagger = model->createTagger();
  if (!tagger) {
    Mecab_clear(m);
    return NULL;
  }
  m->tagger = tagger;

  auto lattice = model->createLattice();
  if (!lattice) {
    Mecab_clear(m);
    return NULL;
  }
  m->lattice = lattice;
  return 1;
}

OpenJTalk::OpenJTalk(const std::string &dict_path, const std::string &user_dict) {
  m_mecab = new Mecab{};
  m_njd = new NJD{};
  m_jpcommon = new JPCommon{};

  Mecab_initialize(m_mecab);
  NJD_initialize(m_njd);
  JPCommon_initialize(m_jpcommon);

  auto r = Mecab_load_with_userdic(m_mecab, dict_path, user_dict);
  if (!r) {
    clear();
  }
}

void OpenJTalk::clear() {
  if (m_mecab) {
    Mecab_clear(m_mecab);
  }
  if (m_njd) {
    NJD_clear(m_njd);
  }
  if (m_jpcommon) {
    JPCommon_clear(m_jpcommon);
  }
}

OpenJTalk::~OpenJTalk() {
  //  if (m_mecab) {
  //    mecab_destroy(m_mecab);
  //  }
}

std::unique_ptr<OpenJTalk>
OpenJTalk::FromUserDict(const std::string &dict_path, const std::string &user_dict) {
  //  std::string options = "-d " + dict_path + " -u " + user_dict;
  //  auto m_mecab = mecab_new2(options.c_str());
  //
  //  //  auto mecab = mecab_new2(options.c_str());
  ////  auto tagger = MeCab::createTagger(options.c_str());
  //  if (m_mecab == nullptr) {
  //    THROW_ERRORN("create new mecab object fail!");
  //  }
  return std::make_unique<OpenJTalk>(dict_path, user_dict);
};

std::vector<OpenJTalk::Feature> OpenJTalk::run_frontend(const std::string &text) {
  char buff[8192];
  text2mecab(buff, text.c_str());
  Mecab_analysis(m_mecab, buff);
  mecab2njd(m_njd, Mecab_get_feature(m_mecab), Mecab_get_size(m_mecab));
  njd_set_pronunciation(m_njd);
  njd_set_digit(m_njd);
  njd_set_accent_phrase(m_njd);
  njd_set_accent_type(m_njd);
  njd_set_unvoiced_vowel(m_njd);
  njd_set_long_vowel(m_njd);
  auto features = njd2feature(m_njd);
  NJD_refresh(m_njd);
  Mecab_refresh(m_mecab);

  return features;
};

std::vector<std::string>
OpenJTalk::make_label(const std::vector<OpenJTalk::Feature> &features) {
  feature2njd(m_njd, features);
  njd2jpcommon(m_jpcommon, m_njd);
  JPCommon_make_label(m_jpcommon);
  int label_size = JPCommon_get_label_size(m_jpcommon);
  auto label_feature = JPCommon_get_label_feature(m_jpcommon);
  std::vector<std::string> labels;
  for (int i = 0; i < label_size; ++i) {
    labels.emplace_back(label_feature[i]);
  }
  JPCommon_refresh(m_jpcommon);
  NJD_refresh(m_njd);
  return labels;
};

std::pair<std::vector<std::string>,std::string> OpenJTalk::g2p(const std::string &text, bool kana, bool join) {
  auto njd_features = run_frontend(text);
  if (!kana) {
    auto labels = make_label(njd_features);
    std::vector<std::string> prons;
    for (size_t i = 1; i < labels.size() - 1; ++i) {
      std::string s = labels[i];
      size_t dashPos = s.find('-');
      if (dashPos != std::string::npos) {

        size_t plusPos = s.find('+', dashPos);
        std::string pron = s.substr(dashPos + 1, (plusPos != std::string::npos ? plusPos - dashPos - 1 : std::string::npos));
        prons.push_back(pron);
      }
    }
    if (join) {
      return {{},boost::algorithm::join(prons, " ")};
    }
    return {prons,""};
  }
  // kana
  std::vector<std::string> prons;
  for (auto &n: njd_features) {
    std::string p;
    if (n.pos == "記号") {
      p = n.string;
    } else {
      p = n.pron;
    }
    // remove special chars
    boost::replace_all(p, "’", "");
    prons.emplace_back(p);
  }
  if (join) {
    return {{},boost::algorithm::join(prons, "")};
  }
  return {prons,""};
}

}// namespace GPTSovits::G2P

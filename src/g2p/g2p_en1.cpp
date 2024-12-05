//
// Created by 19254 on 24-12-2.
//
#include <GPTSovits/G2P/g2p_en.h>
#include <GPTSovits/Utils/exception.h>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <memory>
#include <filesystem>
#include "GPTSovits/plog.h"

#include <xtensor/xarray.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xnpy.hpp>
#include <xtensor-blas/xlinalg.hpp>
#include <xtensor/xindex_view.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xindex_view.hpp>

namespace GPTSovits::G2P::g2p_en {

extern std::map<std::string, int> g_en_g2idx;
extern std::map<int, std::string> g_en_idx2p;

struct NpData {
  std::shared_ptr<xt::xarray<float>> enc_emb;
  std::shared_ptr<xt::xarray<float>> dec_b_hh;
  std::shared_ptr<xt::xarray<float>> dec_b_ih;
  std::shared_ptr<xt::xarray<float>> dec_emb;
  std::shared_ptr<xt::xarray<float>> dec_w_hh;
  std::shared_ptr<xt::xarray<float>> dec_w_ih;
  std::shared_ptr<xt::xarray<float>> enc_b_hh;
  std::shared_ptr<xt::xarray<float>> enc_b_ih;
  std::shared_ptr<xt::xarray<float>> enc_w_hh;
  std::shared_ptr<xt::xarray<float>> enc_w_ih;
  std::shared_ptr<xt::xarray<float>> fc_b;
  std::shared_ptr<xt::xarray<float>> fc_w;
};
NpData g_np_data;

void init_np_array() {
  static bool isInit = false;
  if (isInit) {
    return;
  }
  isInit = true;
  auto loadnp =
    [](std::filesystem::path path, auto &target) {
      target = std::make_shared<xt::xarray<float>>(xt::load_npy<float>(path.string()));
    };
  auto path = std::filesystem::current_path() / "res" / "g2p" / "en";
  loadnp(path / "enc_emb_en.bin", g_np_data.enc_emb);
  loadnp(path / "dec_b_hh_en.bin", g_np_data.dec_b_hh);
  loadnp(path / "dec_b_ih_en.bin", g_np_data.dec_b_ih);
  loadnp(path / "dec_emb_en.bin", g_np_data.dec_emb);
  loadnp(path / "dec_w_hh_en.bin", g_np_data.dec_w_hh);
  loadnp(path / "dec_w_ih_en.bin", g_np_data.dec_w_ih);
  loadnp(path / "enc_b_hh_en.bin", g_np_data.enc_b_hh);
  loadnp(path / "enc_b_ih_en.bin", g_np_data.enc_b_ih);
  loadnp(path / "enc_w_hh_en.bin", g_np_data.enc_w_hh);
  loadnp(path / "enc_w_ih_en.bin", g_np_data.enc_w_ih);
  loadnp(path / "fc_b_en.bin", g_np_data.fc_b);
  loadnp(path / "fc_w_en.bin", g_np_data.fc_w);
}

xt::xarray<float> sigmoid(const xt::xarray<float> &x) {
  return 1.0f / (1.0f + xt::exp(-x));
}

xt::xarray<float> grucell(const xt::xarray<float> &x, const xt::xarray<float> &h,
                          const xt::xarray<float> &w_ih, const xt::xarray<float> &w_hh,
                          const xt::xarray<float> &b_ih, const xt::xarray<float> &b_hh) {
  // Matrix multiplication
  auto rzn_ih = xt::linalg::dot(x, xt::transpose(w_ih)) + b_ih;
  auto rzn_hh = xt::linalg::dot(h, xt::transpose(w_hh)) + b_hh;

  // Split operations
  int split_size = rzn_ih.shape(1) * 2 / 3;
  auto rz_ih = xt::view(rzn_ih, xt::all(), xt::range(0, split_size));
  auto n_ih = xt::view(rzn_ih, xt::all(), xt::range(split_size, rzn_ih.shape(1)));
  auto rz_hh = xt::view(rzn_hh, xt::all(), xt::range(0, split_size));
  auto n_hh = xt::view(rzn_hh, xt::all(), xt::range(split_size, rzn_hh.shape(1)));

  auto rz = sigmoid(rz_ih + rz_hh);

  // Split rz into r and z
  auto r = xt::view(rz, xt::all(), xt::range(0, rz.shape(1) / 2));
  auto z = xt::view(rz, xt::all(), xt::range(rz.shape(1) / 2, rz.shape(1)));

  auto n = xt::tanh(n_ih + r * n_hh);
  auto new_h = (1.0f - z) * n + z * h;

  return new_h;
}

xt::xarray<float> gru(const xt::xarray<float> &x, int steps,
                      const xt::xarray<float> &w_ih, const xt::xarray<float> &w_hh,
                      const xt::xarray<float> &b_ih, const xt::xarray<float> &b_hh,
                      const xt::xarray<float> *h0 = nullptr) {

  int batch_size = x.shape(0);
  int hidden_size = w_hh.shape(1);

  // 初始化隐藏状态
  xt::xarray<float> h;
  if (h0 == nullptr) {
    h = xt::zeros<float>({batch_size, hidden_size});
  } else {
    h = *h0;
  }

  // 初始化输出tensor
  xt::xarray<float> outputs = xt::zeros<float>({batch_size, steps, hidden_size});

  // 按时间步长进行循环
  for (int t = 0; t < steps; ++t) {
    auto xt = xt::view(x, xt::all(), t, xt::all());
    h = grucell(xt, h, w_ih, w_hh, b_ih, b_hh);
    xt::view(outputs, xt::all(), t, xt::all()) = h;
  }

  return outputs;
}

xt::xarray<float> encode(const std::string &word) {
  std::vector<int> x;
  for (char c: word) {
    std::string char_str(1, c);
    auto it = g_en_g2idx.find(char_str);
    x.push_back(it != g_en_g2idx.end() ? it->second : g_en_g2idx["<unk>"]);
  }
  // Add </s> token
  x.push_back(g_en_g2idx["</s>"]);

  // 修改: 直接构建结果数组
  xt::xarray<float> result = xt::zeros<float>({(size_t) 1, x.size(), g_np_data.enc_emb->shape(1)});
  for (size_t i = 0; i < x.size(); i++) {
    for (size_t j = 0; j < g_np_data.enc_emb->shape(1); j++) {
      result(0, i, j) = (*(g_np_data.enc_emb))(x[i], j);
    }
  }

  return result;
}


std::vector<std::string> predict(const std::string &text) {
  init_np_array();
  auto enc = encode(text);

  auto enc_out = gru(enc, text.length() + 1, *g_np_data.enc_w_ih, *g_np_data.enc_w_hh,
                     *g_np_data.enc_b_ih, *g_np_data.enc_b_hh,
                     nullptr);

  // get last hidden state - shape (1, hidden_size)
  auto last_hidden = xt::view(enc_out, xt::all(), -1, xt::all());

  // decoder initial state
  xt::xarray<float> dec = xt::zeros<float>({static_cast<size_t>(1), g_np_data.dec_emb->shape(1)});
  for(size_t j = 0; j < g_np_data.dec_emb->shape(1); j++) {
    dec(0, j) = (*g_np_data.dec_emb)(2, j); // 2 is <s> token
  }

  auto h = last_hidden;

  std::vector<std::string> preds;
  for(int i = 0; i < 20; i++) {
    h = grucell(dec, h, *g_np_data.dec_w_ih, *g_np_data.dec_w_hh,
                *g_np_data.dec_b_ih, *g_np_data.dec_b_hh);

    // Calculate logits - shape should be (1, vocab_size)
    auto logits = xt::linalg::dot(h, xt::transpose(*g_np_data.fc_w)) + *g_np_data.fc_b;

    // Get predicted index
    size_t pred;
    float max_val = std::numeric_limits<float>::lowest();

    // Manually find the maximum value and its index
    for(size_t j = 0; j < logits.shape(1); ++j) {
      if(logits(0, j) > max_val) {
        max_val = logits(0, j);
        pred = j;
      }
    }

    if(pred == 3) break; // </s> token

    auto it = g_en_idx2p.find(pred);
    if(it != g_en_idx2p.end()) {
      preds.push_back(it->second);
    } else {
      preds.push_back("<unk>");
    }

    // Update dec for next iteration
    dec = xt::zeros<float>({static_cast<size_t>(1), g_np_data.dec_emb->shape(1)});
    for(size_t j = 0; j < g_np_data.dec_emb->shape(1); j++) {
      dec(0, j) = (*g_np_data.dec_emb)(pred, j);
    }
  }

  return preds;
}


std::map<std::string, int> g_en_g2idx = {
  {"</s>",  2},
  {"<pad>", 0},
  {"<unk>", 1},
  {"a",     3},
  {"b",     4},
  {"c",     5},
  {"d",     6},
  {"e",     7},
  {"f",     8},
  {"g",     9},
  {"h",     10},
  {"i",     11},
  {"j",     12},
  {"k",     13},
  {"l",     14},
  {"m",     15},
  {"n",     16},
  {"o",     17},
  {"p",     18},
  {"q",     19},
  {"r",     20},
  {"s",     21},
  {"t",     22},
  {"u",     23},
  {"v",     24},
  {"w",     25},
  {"x",     26},
  {"y",     27},
  {"z",     28},
};

std::map<int, std::string> g_en_idx2p = {
  {0,  "<pad>"},
  {1,  "<unk>"},
  {2,  "<s>"},
  {3,  "</s>"},
  {4,  "AA0"},
  {5,  "AA1"},
  {6,  "AA2"},
  {7,  "AE0"},
  {8,  "AE1"},
  {9,  "AE2"},
  {10, "AH0"},
  {11, "AH1"},
  {12, "AH2"},
  {13, "AO0"},
  {14, "AO1"},
  {15, "AO2"},
  {16, "AW0"},
  {17, "AW1"},
  {18, "AW2"},
  {19, "AY0"},
  {20, "AY1"},
  {21, "AY2"},
  {22, "B"},
  {23, "CH"},
  {24, "D"},
  {25, "DH"},
  {26, "EH0"},
  {27, "EH1"},
  {28, "EH2"},
  {29, "ER0"},
  {30, "ER1"},
  {31, "ER2"},
  {32, "EY0"},
  {33, "EY1"},
  {34, "EY2"},
  {35, "F"},
  {36, "G"},
  {37, "HH"},
  {38, "IH0"},
  {39, "IH1"},
  {40, "IH2"},
  {41, "IY0"},
  {42, "IY1"},
  {43, "IY2"},
  {44, "JH"},
  {45, "K"},
  {46, "L"},
  {47, "M"},
  {48, "N"},
  {49, "NG"},
  {50, "OW0"},
  {51, "OW1"},
  {52, "OW2"},
  {53, "OY0"},
  {54, "OY1"},
  {55, "OY2"},
  {56, "P"},
  {57, "R"},
  {58, "S"},
  {59, "SH"},
  {60, "T"},
  {61, "TH"},
  {62, "UH0"},
  {63, "UH1"},
  {64, "UH2"},
  {65, "UW"},
  {66, "UW0"},
  {67, "UW1"},
  {68, "UW2"},
  {69, "V"},
  {70, "W"},
  {71, "Y"},
  {72, "Z"},
  {73, "ZH"},
};

}
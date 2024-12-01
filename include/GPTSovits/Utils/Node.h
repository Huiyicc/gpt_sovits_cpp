//
// Created by 19254 on 24-11-30.
//

#ifndef GPT_SOVITS_CPP_NODE_H
#define GPT_SOVITS_CPP_NODE_H

namespace GPTSovits::Utils {

template<typename T>
class Node {
public:
  T data;
  Node* next = nullptr;

  Node(const T& value) : data(value), next(nullptr) {}
};

};

#endif //GPT_SOVITS_CPP_NODE_H

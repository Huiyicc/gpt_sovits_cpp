//
// Created by 19254 on 24-11-30.
//

#ifndef GPT_SOVITS_CPP_STACK_H
#define GPT_SOVITS_CPP_STACK_H

#include <GPTSovits/Utils/exception.h>
#include <GPTSovits/Utils/Node.h>

namespace GPTSovits::Utils {

/**
 * @brief 栈类模板
 *
 * @tparam T 栈中存储的数据类型
 */
template<typename T>
class Stack {
private:
  Node<T> *top = nullptr; ///< 栈顶指针
public:
  /**
   * @brief 构造函数，初始化栈为空
   */
  Stack() : top(nullptr) {}

  /**
   * @brief 析构函数，释放栈中所有节点
   *
   * @note 使用noexcept表示此函数不会抛出异常
   */
  ~Stack() noexcept {
    while (!isEmpty()) {
      pop();
    }
  }

  /**
   * @brief 将一个元素压入栈中
   *
   * @param value 要压入栈的元素值
   */
  void push(const T& value) {
    auto *newNode = new Node<T>(value); ///< 创建新节点
    newNode->next = top; ///< 将新节点的下一个指针指向当前栈顶
    top = newNode; ///< 更新栈顶为新节点
  }
  void push(T&& value) {
    auto *newNode = new Node<T>(std::forward<T>(value));
    newNode->next = top;
    top = newNode;
  }

  /**
   * @brief 从栈中弹出栈顶元素
   *
   * @return true 弹出成功
   * @return false 栈为空，无法弹出
   */
  bool pop() {
    if (isEmpty()) {
      return false; ///< 栈为空，无法弹出
    }
    Node<T> *temp = top; ///< 保存当前栈顶节点
    top = top->next; ///< 更新栈顶为下一个节点
    delete temp; ///< 释放原栈顶节点的内存
    return true; ///< 返回弹出成功
  }

  /**
   * @brief 查看栈顶元素但不弹出
   *
   * @return T 栈顶元素的值
   * @throws GPTSovits::Exception 当栈为空时抛出此异常
   */
  T& peek() {
    if (isEmpty()) {
      THROW_ERRORN("Stack Empty!"); ///< 抛出栈空异常
    }
    return top->data; ///< 返回栈顶元素的值
  }

  /**
   * @brief 检查栈是否为空
   *
   * @return true 栈为空
   * @return false 栈不为空
   */
  bool isEmpty() {
    return top == nullptr; ///< 返回栈顶指针是否为nullptr
  }
};

};

#endif //GPT_SOVITS_CPP_STACK_H

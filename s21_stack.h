#ifndef _SRC_S21_STACK_H_
#define _SRC_S21_STACK_H_

#include "s21_list.h"

namespace s21 {
template <class T>
class stack : public list<T> {
 public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using size_type = size_t;

  stack() : list<T>(){};
  stack(std::size_t n) : list<T>(n) {}
  stack(std::initializer_list<T> const &items) : list<T>(items) {}
  stack(const stack &s) : list<T>(s) {}
  stack(stack &&s) : list<T>(s) { s.list<T>::clear(); }
  stack &operator=(stack &&s) {
    list<T>::operator=(s);
    s.list<T>::clear();
    return *this;
  }

  const_reference top() { return *(--list<T>::end()); }
  void push(const_reference value) { list<T>::push_back(value); }
  void pop() {
    if (list<T>::size() != 0) list<T>::pop_back();
  }

  template <typename... Args>
  void emplace_front(Args &&...args) {
    list<T>::emplace_back(args...);
  }
};

}  // namespace s21

#endif  // _SRC_S21_STACK_H_
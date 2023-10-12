#ifndef _SRC_S21_QUEUE_H_
#define _SRC_S21_QUEUE_H_

#include "s21_list.h"

namespace s21 {

template <class T>
class queue : public list<T> {
 public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using size_type = std::size_t;

  queue() : list<T>() {}
  queue(std::size_t n) : list<T>(n) {}
  queue(std::initializer_list<value_type> const &items) : list<T>(items) {}
  queue(const queue &q) : list<T>(q) {}
  queue(queue &&q) : list<T>(q) { q.list<T>::clear(); }
  queue &operator=(queue &&q) {
    list<T>::operator=(q);
    q.list<T>::clear();
    return *this;
  }
  bool operator==(queue other) const { return list<T>::operator==(other); }
  queue &operator=(const queue &q) {
    list<T>::operator=(q);
    return *this;
  }

  void push(const_reference value) { list<T>::push_back(value); }
  void pop() { list<T>::pop_front(); }
};
}  // namespace s21

#endif  // _SRC_S21_QUEUE_H_

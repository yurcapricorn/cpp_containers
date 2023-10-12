
#pragma once
#include "s21_set.h"

namespace s21 {

template <class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>>
class multiset : public set_base<false, Key, Compare, Allocator> {
 public:
  using value_type = Key;
  using iterator = CTree_iterator<value_type, Compare, Allocator>;
  using base_class = set_base<false, Key, Compare, Allocator>;

  multiset() : base_class() {}

  multiset(std::initializer_list<Key> const &items) : base_class(items) {}

  iterator insert(const value_type &value) {
    return base_class::insert(value).first;
  }
};

};  // namespace s21
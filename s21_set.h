
#pragma once

#include <stdexcept>

#include "s21_tree.h"
#include "s21_vector.h"

namespace s21 {

template <bool unique, class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>>
class set_base {
 public:
  using key_type = Key;
  using value_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using NodeT = Node<value_type>;
  using iterator = CTree_iterator<value_type, Compare, Allocator>;
  using const_iterator = const CTree_iterator<value_type, Compare, Allocator>;
  using size_type = size_t;

  set_base() {}
  set_base(std::initializer_list<value_type> const &items) {
    for (auto item : items) {
      insert(item);
    }
  }
  set_base(const set_base &s) {
    for (auto it = s.cbegin(); it != s.cend(); it++) {
      insert(*it);
    }
  }
  set_base(set_base &&s) {
    if (this != &s) {
      std::swap(tree, s.tree);
    }
  }
  ~set_base() {}

  set_base &operator=(set_base &&s) {
    if (this == &s) return *this;
    std::swap(tree, s.tree);
    return *this;
  }

  iterator begin() { return iterator(tree, tree.getFirstNode()); }
  iterator end() { return iterator(tree, tree.getSentinelNode()); }

  const_iterator cbegin() const {
    return const_iterator(tree, tree.getFirstNode());
  }
  const_iterator cend() const {
    return const_iterator(tree, tree.getSentinelNode());
  }

  bool empty() { return tree.getNumberOfNodes() == 0; }
  size_type size() { return tree.getNumberOfNodes(); }
  size_type max_size() { return std::numeric_limits<size_type>::max(); }

  void clear() { tree.removeAllNodes(); }

  std::pair<iterator, bool> insert(const value_type &value) {
    if (unique) {
      auto node = tree.findNode(value);
      if (!tree.isNil(node)) {
        return std::pair<iterator, bool>{iterator(tree, node), false};
      } else {
        return std::pair<iterator, bool>{iterator(tree, tree.insertNode(value)),
                                         true};
      }
    } else {
      return std::pair<iterator, bool>{iterator(tree, tree.insertNode(value)),
                                       true};
    }
  }

  void erase(iterator pos) { tree.removeNode(*pos); }
  void swap(set_base &other) { std::swap(*this, other); }

  void merge(set_base &other) {
    auto it = other.begin();
    while (it != other.end()) {
      auto node = *it;
      it++;
      auto res = insert(node);
      if (!unique || res.second) {
        auto nextNode = *it;
        auto isNil = it == other.end();
        other.tree.removeNode(node);
        if (isNil) {
          break;
        }
        it = other.find(nextNode);
      }
    }
  }

  iterator find(const Key &key) { return iterator(tree, tree.findNode(key)); }

  iterator lower_bound(const Key &key) {
    auto it = begin();
    auto comp = Compare();
    while (it != end()) {
      if (!comp(*it, key)) {
        break;
      }
      it++;
    }
    return it;
  }

  iterator upper_bound(const Key &key) {
    auto it = begin();
    auto comp = Compare();
    while (it != end()) {
      if (comp(key, *it)) {
        break;
      }
      it++;
    }
    return it;
  }

  size_type count(const Key &key) { return tree.getNumberOfNodes(key); }

  bool contains(const Key &key) {
    auto node = tree.findNode(value_type{key});
    return tree.isNil(node) ? false : true;
  }

  template <class... Args>
  s21::vector<std::pair<iterator, bool>> emplace(Args &&...args) {
    s21::vector<std::pair<iterator, bool>> out;
    out.push_back(insert(value_type(std::forward<Args>(args)...)));
    return out;
  }

 protected:
  CTree<value_type, Compare, Allocator> tree;
};

template <class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key>>
class set : public set_base<true, Key, Compare, Allocator> {
 public:
  using base_class = set_base<true, Key, Compare, Allocator>;

  set() : base_class() {}

  set(std::initializer_list<Key> const &items) : base_class(items) {}
};

};  // namespace s21
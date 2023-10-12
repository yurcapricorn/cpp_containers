
#pragma once

#include <stdexcept>

#include "s21_tree.h"
#include "s21_vector.h"

namespace s21 {

template <class value_type, class Compare>
class value_compare {
 public:
  value_compare() : comp(Compare()) {}

  bool operator()(const value_type &lhs, const value_type &rhs) {
    return comp(lhs.first, rhs.first);
  }

 private:
  Compare comp;
};

template <class Key, class T, class Compare = std::less<Key>,
          class Allocator = std::allocator<std::pair<const Key, T>>>
class map {
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<key_type, mapped_type>;
  using reference = value_type &;
  using const_reference = const value_type &;
  using NodeT = Node<value_type>;
  using iterator =
      CTree_iterator<value_type, value_compare<value_type, Compare>, Allocator>;
  using const_iterator =
      const CTree_iterator<value_type, value_compare<value_type, Compare>,
                           Allocator>;
  using size_type = size_t;

 public:
  map() {}
  map(std::initializer_list<value_type> const &items) {
    for (auto item : items) {
      insert(item);
    }
  }
  map(const map &m) {
    for (auto it = m.cbegin(); it != m.cend(); it++) {
      insert(*it);
    }
  }
  map(map &&m) noexcept {
    if (this != &m) {
      std::swap(tree, m.tree);
    }
  }
  ~map() {}
  map &operator=(map &&m) {
    if (this == &m) return *this;
    std::swap(tree, m.tree);
    return *this;
  }

  T &at(const Key &key) {
    auto node = tree.findNode(value_type{key, T()});
    if (tree.isNil(node)) {
      throw std::out_of_range("out_of_range");
    }
    return node->item.second;
  }

  T &operator[](const Key &key) {
    value_type value = {key, T()};
    auto node = tree.findNode(value);
    if (!tree.isNil(node)) {
      return node->item.second;
    } else {
      return tree.insertNode(value)->item.second;
    }
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
    auto node = tree.findNode(value);
    if (!tree.isNil(node)) {
      return std::pair<iterator, bool>{iterator(tree, node), false};
    } else {
      return std::pair<iterator, bool>{iterator(tree, tree.insertNode(value)),
                                       true};
    }
  }

  std::pair<iterator, bool> insert(const Key &key, const T &obj) {
    return insert(value_type{key, obj});
  }

  std::pair<iterator, bool> insert_or_assign(const Key &key, const T &obj) {
    value_type value = {key, obj};
    auto node = tree.findNode(value);
    if (!tree.isNil(node)) {
      node->item.second = obj;
      return std::pair<iterator, bool>{iterator(tree, node), false};
    } else {
      return std::pair<iterator, bool>{iterator(tree, tree.insertNode(value)),
                                       true};
    }
  }

  void erase(iterator pos) { tree.removeNode(*pos); }
  void swap(map &other) { std::swap(*this, other); }
  void merge(map &other) {
    auto it = other.begin();
    while (it != other.end()) {
      auto node = *it;
      it++;
      auto res = insert(node);
      if (res.second) {
        auto nextNode = *it;
        auto isNil = it == other.end();
        other.tree.removeNode(node);
        if (isNil) {
          break;
        }
        it = other.find(nextNode.first);
      }
    }
  }
  iterator find(const Key &key) {
    value_type value = {key, T()};
    return iterator(tree, tree.findNode(value));
  }
  bool contains(const Key &key) {
    auto node = tree.findNode(value_type{key, T()});
    return tree.isNil(node) ? false : true;
  }

  template <class... Args>
  s21::vector<std::pair<iterator, bool>> emplace(Args &&...args) {
    s21::vector<std::pair<iterator, bool>> out;
    out.push_back(insert(value_type(std::forward<Args>(args)...)));
    return out;
  }

 private:
  CTree<value_type, value_compare<value_type, Compare>, Allocator> tree;
};

};  // namespace s21
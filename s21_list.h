#ifndef _SRC_S21_LIST_H_
#define _SRC_S21_LIST_H_

#include <initializer_list>
#include <limits>

namespace s21 {

template <class T>
class Node {
 public:
  T value;
  Node<T> *prev = nullptr, *next = nullptr;
  Node(const T val) : value(val) {}
};

template <class T>
class list {
 public:
  class ListIterator {
    friend list;

   public:
    ListIterator(Node<T> *node) : ptr(node){};
    T &operator*() { return ptr->value; }
    ListIterator operator++(int) {
      ptr = ptr->next;
      return *this;
    }
    ListIterator operator--(int) {
      ptr = ptr->prev;
      return *this;
    }
    ListIterator operator++() {
      ptr = ptr->next;
      return *this;
    }
    ListIterator operator--() {
      ptr = ptr->prev;
      return *this;
    }
    bool operator!=(const ListIterator &other) { return ptr != other.ptr; }
    bool operator==(const ListIterator &other) { return ptr == other.ptr; }

   protected:
    Node<T> *ptr = nullptr;
  };

  class ListConstIterator : ListIterator {
    friend list;

   public:
    const T &operator*() { return ListIterator::operator*(); }
    ListConstIterator(ListIterator other) : ListIterator(other) {}
    ListConstIterator operator++(int) {
      ptr = ptr->next;
      return *this;
    }

   protected:
    Node<T> *ptr = nullptr;
  };

  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using iterator = ListIterator;
  using const_iterator = ListConstIterator;
  using size_type = std::size_t;

 private:
  using NodeT = Node<value_type>;

 public:
  const_reference front() const { return head->value; }
  const_reference back() const { return tail->value; }

  iterator begin() { return iterator(head); }
  iterator end() { return iterator(not_a_node); }

  const_iterator begin() const { return const_iterator(head); }
  const_iterator end() const { return const_iterator(not_a_node); }

  void pop_front() { pop(head); }
  void pop_back() { pop(tail); }
  list() {
    not_a_node = new NodeT(value_type{});
    head = tail = not_a_node;
    head->next = head;
    head->prev = head;
  };

  list(size_type n) {
    not_a_node = new NodeT(value_type{});
    for (size_type k = 0; k < n; k++) push_back(0);
  }

  list(std::initializer_list<value_type> const &items) {
    not_a_node = new NodeT(value_type{});
    for (const auto &add : items) push_back(add);
  }

  list(const list &other) : list() {
    NodeT *tmp = other.head;
    for (std::size_t i = 0; i < other.size_; i++) {
      this->push_back(tmp->value);
      tmp = tmp->next;
    }
  }

  list(list<value_type> &&l) {
    not_a_node = l.not_a_node;
    head = l.head;
    tail = l.tail;
    size_ = l.size_;
    l.not_a_node = l.head = l.tail = nullptr;
    l.size_ = 0;
  }

  list &operator=(list<value_type> &&l) {
    clear();
    delete not_a_node;
    not_a_node = l.not_a_node;
    size_ = l.size_;
    head = l.head;
    tail = l.tail;
    l.not_a_node = l.head = l.tail = nullptr;
    l.size_ = 0;
    return *this;
  }

  list &operator=(const list<value_type> &l) {
    if (size() > l.size()) {
      while (size() != l.size()) {
        pop_back();
      }
    } else {
      while (size() != l.size()) {
        push_back(0);
      }
    }

    iterator it = begin();
    for (auto i : l) *it = i->value;
    return *this;
  }

  ~list() {
    clear();
    delete not_a_node;
  };

  void push_back(const value_type &value) {
    NodeT *node = new Node(value);
    if (size_ == 0) {
      head = tail = node;
      tail->next = not_a_node;
      head->prev = not_a_node;
      not_a_node->prev = tail;
      not_a_node->next = head;
    } else {
      NodeT *tmp = head;
      tmp = tail;
      tmp->next = tail = node;
      tail->next = not_a_node;
      tail->prev = tmp;
      head->prev = not_a_node;
      not_a_node->prev = tail;
      not_a_node->next = head;
    }
    size_++;
    not_a_node->value = size_;
  }

  void push_front(const value_type &value) {
    NodeT *node = new Node(value);
    if (size_ == 0) {
      head = tail = node;
      tail->next = not_a_node;
      head->prev = not_a_node;
      not_a_node->prev = tail;
      not_a_node->next = head;
    } else {
      NodeT *tmp = head;
      head = node;
      head->next = tmp;
      tmp->prev = head;
      head->prev = not_a_node;
      not_a_node->next = head;
      not_a_node->prev = tail;
      tail->next = not_a_node;
    }
    size_++;
    not_a_node->value = size_;
  }

  void pop(NodeT *del) {
    if (size_ < 1) return;
    if (size_ == 1) {
      head = tail = not_a_node;
      not_a_node->next = not_a_node;
      not_a_node->prev = not_a_node;
    } else if (size_ == 2) {
      if (del == head) {
        head = tail;
        head->prev = not_a_node;
        not_a_node->next = head;
      } else {
        tail = head;
        tail->next = not_a_node;
        not_a_node->prev = tail;
      }
    } else if (del == head) {
      head = head->next;
      head->prev = not_a_node;
      not_a_node->next = head;
    } else if (del == tail) {
      tail = tail->prev;
      tail->next = not_a_node;
      not_a_node->prev = tail;
    } else {
      NodeT *n2 = del->next;
      NodeT *n1 = del->prev;
      n2->prev = n1;
      n1->next = n2;
    }
    delete del;
    size_--;
    not_a_node->value = size_;
  }

  size_type size() { return size_; }

  void swap(list<value_type> &other) {
    swap<NodeT *>(&head, &other.head);
    swap<NodeT *>(&tail, &other.tail);
    swap<NodeT *>(&not_a_node, &other.not_a_node);
    swap<size_type>(&size_, &other.size_);
  }

  bool operator==(list<value_type> other) const {
    if (size_ != other.size_) {
      return false;
    }
    for (NodeT *i = head, *j = other.head; i != not_a_node;
         i = i->next, j = j->next) {
      if (i->value != j->value) return false;
    }
    return true;
  }

  bool empty() const { return size_ == 0 ? true : false; }
  size_type max_size() const {
    return std::numeric_limits<std::ptrdiff_t>().max() / sizeof(NodeT);
  }

  void clear() {
    while (head != not_a_node) {
      NodeT *remove = head;
      head = head->next;
      delete remove;
    }
    size_ = 0;
  }

  iterator insert(iterator pos, value_type value) {
    auto node = new Node(value);
    auto left = pos.ptr->prev;
    auto right = pos.ptr;
    node->next = right;
    node->prev = left;
    left->next = node;
    right->prev = node;
    size_++;
    not_a_node->value = size_;
    return iterator(node);
  }

  void erase(iterator pos) { pop(pos.ptr); }

  void splice(const_iterator pos, list &other) {
    iterator it = pos;
    NodeT *left = it.ptr->prev;
    NodeT *right = it.ptr;

    left->next = other.head;
    other.head->prev = left;
    right->prev = other.tail;
    other.tail->next = right;
    size_ += other.size_;
    not_a_node->value = size_;
    other.size_ = 0;
    other.head = other.tail = other.not_a_node;
    other.not_a_node->next = other.not_a_node;
    other.not_a_node->prev = other.not_a_node;
  }

  void merge(list &other) {
    NodeT *tmp = other.head;
    while (tmp != other.not_a_node) {
      push_back(tmp->value);
      tmp = tmp->next;
    }
    other.clear();
  }

  void reverse() {
    NodeT *this_ = head;
    swap<NodeT *>(&head, &tail);
    for (int k = 0; k <= (int)size_; k++) {
      swap<NodeT *>(&this_->next, &this_->prev);
      this_ = this_->prev;
    }
  }
  void unique() {
    NodeT *tmp = head;
    while (tmp != not_a_node) {
      const_reference val = tmp->value;
      tmp = tmp->next;
      while (tmp != not_a_node && val == tmp->value) {
        NodeT *tmp2 = tmp->next;
        pop(tmp);
        tmp = tmp2;
      }
    }
  }
  void sort() {
    NodeT *tmp = head;
    bool flag = 0;

    while (true) {
      while (tmp->next != not_a_node) {
        if (tmp->value > tmp->next->value) {
          swap(&(tmp->value), &(tmp->next->value));
          flag = 1;
        } else {
          tmp = tmp->next;
        }
      }
      tmp = head;

      if (!flag) {
        break;
      }
      flag = false;
    }
  }

  iterator emplace(iterator pos, value_type arg) { return insert(pos, arg); }

  template <class... Args>
  iterator emplace(const_iterator pos, value_type first, Args &&...args) {
    return emplace(insert(pos, first)++, args...);
  }

  void emplace_back(value_type first) { push_back(first); }

  template <class... Args>
  void emplace_back(value_type first, Args &&...args) {
    push_back(first);
    emplace_back(args...);
  }

  void emplace_front(value_type first) { push_front(first); }

  template <class... Args>
  void emplace_front(value_type first, Args &&...args) {
    push_front(first);
    emplace_front(args...);
  }

 private:
  friend ListIterator;
  NodeT *not_a_node = nullptr, *head = nullptr, *tail = nullptr;
  size_type size_ = 0;

  template <class S>
  void swap(S *a, S *b) {
    S buff = *a;
    *a = *b;
    *b = buff;
  }
};

}  // namespace s21

#endif  // _SRC_S21_LIST_H_
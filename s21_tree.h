
#pragma once

#include <functional>

enum NodeColor : uint32_t { RED, BLACK };
template <class value_type>
struct Node {
  Node *left;
  Node *right;
  Node *parent;
  NodeColor color;
  value_type item;

  Node()
      : left(nullptr),
        right(nullptr),
        parent(nullptr),
        color(BLACK),
        item(value_type()) {}

  Node(const value_type &v, Node *p)
      : left(nullptr), right(nullptr), parent(p), color(RED), item(v) {}

  bool on_left() { return parent && this == parent->left; }

  bool on_right() { return parent && this == parent->right; }
};

template <class value_type, class Compare, class Allocator>
class CTree {
 private:
  using size_type = size_t;
  using NodeT = Node<value_type>;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using allocator_traits = std::allocator_traits<Allocator>;

  static inline NodeT sentinel = NodeT();
  NodeT *root;
  size_type numberOfNodes;
  Compare comp;
  Allocator allocator;

  NodeT *findNode(NodeT *root, const value_type &item) {
    auto node = root;
    while (!isNil(node)) {
      if (!comp(item, node->item) && !comp(node->item, item)) {
        return node;
      } else {
        node = comp(item, node->item) ? node->left : node->right;
      }
    }
    return &sentinel;
  }

  NodeT *node_min(NodeT *root) const {
    auto node = root;
    while (!isNil(node->left)) {
      node = node->left;
    }
    return node;
  }

  NodeT *node_max(NodeT *root) const {
    auto node = root;
    while (!isNil(node->right)) {
      node = node->right;
    }
    return node;
  }

  void removeAllNodes(NodeT *root) {
    if (!isNil(root)) {
      removeAllNodes(root->left);
      removeAllNodes(root->right);
      allocator_traits::deallocate(allocator, reinterpret_cast<pointer>(root),
                                   sizeof(NodeT));
    }
  }

  void rotate_left(NodeT *x) {
    auto y = x->right;
    x->right = y->left;
    if (y->left != &sentinel) y->left->parent = x;
    if (y != &sentinel) y->parent = x->parent;
    if (x->parent) {
      if (x == x->parent->left)
        x->parent->left = y;
      else
        x->parent->right = y;
    } else {
      root = y;
    }
    y->left = x;
    if (!isNil(x)) x->parent = y;
  }

  void rotate_right(NodeT *x) {
    auto y = x->left;
    x->left = y->right;
    if (!isNil(y->right)) y->right->parent = x;
    if (!isNil(y)) y->parent = x->parent;
    if (x->parent) {
      if (x == x->parent->right)
        x->parent->right = y;
      else
        x->parent->left = y;
    } else {
      root = y;
    }
    y->right = x;
    if (!isNil(x)) x->parent = y;
  }

  void insertFixupNode(NodeT *x) {
    while (x != root && x->parent->color == RED) {
      if (x->parent == x->parent->parent->left) {
        auto y = x->parent->parent->right;
        if (y->color == RED) {
          x->parent->color = BLACK;
          y->color = BLACK;
          x->parent->parent->color = RED;
          x = x->parent->parent;
        } else {
          if (x == x->parent->right) {
            x = x->parent;
            rotate_left(x);
          }
          x->parent->color = BLACK;
          x->parent->parent->color = RED;
          rotate_right(x->parent->parent);
        }
      } else {
        auto y = x->parent->parent->left;
        if (y->color == RED) {
          x->parent->color = BLACK;
          y->color = BLACK;
          x->parent->parent->color = RED;
          x = x->parent->parent;
        } else {
          if (x == x->parent->left) {
            x = x->parent;
            rotate_right(x);
          }
          x->parent->color = BLACK;
          x->parent->parent->color = RED;
          rotate_left(x->parent->parent);
        }
      }
    }
    root->color = BLACK;
  }

  void deleteFixupNode(NodeT *x) {
    while (x != root && x->color == BLACK) {
      if (x == x->parent->left) {
        auto w = x->parent->right;
        if (w->color == RED) {
          w->color = BLACK;
          x->parent->color = RED;
          rotate_left(x->parent);
          w = x->parent->right;
        }
        if (w->left->color == BLACK && w->right->color == BLACK) {
          w->color = RED;
          x = x->parent;
        } else {
          if (w->right->color == BLACK) {
            w->left->color = BLACK;
            w->color = RED;
            rotate_right(w);
            w = x->parent->right;
          }
          w->color = x->parent->color;
          x->parent->color = BLACK;
          w->right->color = BLACK;
          rotate_left(x->parent);
          x = root;
        }
      } else {
        auto w = x->parent->left;
        if (w->color == RED) {
          w->color = BLACK;
          x->parent->color = RED;
          rotate_right(x->parent);
          w = x->parent->left;
        }
        if (w->right->color == BLACK && w->left->color == BLACK) {
          w->color = RED;
          x = x->parent;
        } else {
          if (w->left->color == BLACK) {
            w->right->color = BLACK;
            w->color = RED;
            rotate_left(w);
            w = x->parent->left;
          }
          w->color = x->parent->color;
          x->parent->color = BLACK;
          w->left->color = BLACK;
          rotate_right(x->parent);
          x = root;
        }
      }
    }
    x->color = BLACK;
  }

 public:
  CTree()
      : root(&sentinel),
        numberOfNodes(0),
        comp(Compare()),
        allocator(Allocator()) {}

  ~CTree() { removeAllNodes(); }

  NodeT *insertNode(const value_type &item) {
    NodeT *node = root;
    NodeT *parent = nullptr;
    while (!isNil(node)) {
      parent = node;
      node = comp(item, node->item) ? node->left : node->right;
    }
    node = reinterpret_cast<NodeT *>(allocator_traits::allocate(
        allocator, sizeof(NodeT)));  // NodeT(item, parent);
    allocator_traits::construct(allocator, node, item, parent);
    node->left = node->right = &sentinel;

    if (nullptr == parent) {
      root = node;
    } else {
      if (comp(item, parent->item)) {
        parent->left = node;
      } else {
        parent->right = node;
      }
    }
    insertFixupNode(node);
    numberOfNodes++;
    return node;
  }

  void removeNode(const value_type &item) {
    NodeT *x, *y;

    auto node = findNode(item);
    if (isNil(node)) {
      return;
    }

    if (isNil(node->left) || isNil(node->right)) {
      y = node;
    } else {
      y = node->right;
      while (!isNil(y->left)) {
        y = y->left;
      }
    }

    if (!isNil(y->left)) {
      x = y->left;
    } else {
      x = y->right;
    }

    x->parent = y->parent;
    if (y->parent) {
      if (y == y->parent->left) {
        y->parent->left = x;
      } else {
        y->parent->right = x;
      }
    } else {
      root = x;
    }
    if (y != node) {
      node->item = y->item;
    }

    if (y->color == BLACK) {
      deleteFixupNode(x);
    }
    allocator_traits::deallocate(allocator, reinterpret_cast<pointer>(y),
                                 sizeof(NodeT));
    numberOfNodes--;
  }

  NodeT *getFirstNode() const { return node_min(root); }

  NodeT *getSentinelNode() const { return const_cast<NodeT *>(&sentinel); }

  NodeT *nextNode(NodeT *node) const {
    if (isNil(node)) {
      return const_cast<NodeT *>(&sentinel);
    }
    if (!isNil(node->right)) {
      return node_min(node->right);
    } else {
      while (node->on_right()) {
        node = node->parent;
      }
      return node != root ? node->parent : const_cast<NodeT *>(&sentinel);
    }
  }

  NodeT *prevNode(NodeT *node) const {
    if (isNil(node)) {
      return const_cast<NodeT *>(&sentinel);
    }
    if (!isNil(node->left)) {
      return node_max(node->left);
    } else {
      while (node->on_left()) {
        node = node->parent;
      }
      return node != root ? node->parent : const_cast<NodeT *>(&sentinel);
    }
  }

  NodeT *findNode(const value_type &item) { return findNode(root, item); }

  void removeAllNodes() {
    removeAllNodes(root);
    numberOfNodes = 0;
  }

  size_type getNumberOfNodes() { return numberOfNodes; }
  size_type getNumberOfNodes(const value_type &item) {
    size_type count = 0;
    auto node = findNode(root, item);

    while (!isNil(node)) {
      count++;
      node = nextNode(node);
      if (!comp(item, node->item) && !comp(node->item, item)) {
        continue;
      }
      break;
    }

    return count;
  }

  bool isNil(NodeT *node) const { return node == &sentinel; }
};

template <class value_type, class Compare, class Allocator>
class CTree_iterator {
  using reference = value_type &;
  using NodeT = Node<value_type>;
  using Tree = CTree<value_type, Compare, Allocator>;

 private:
  const Tree *tree;
  NodeT *node;

 public:
  CTree_iterator(const Tree &t, NodeT *in)
      : tree(std::addressof(t)), node(in) {}

  CTree_iterator &operator++() {
    node = tree->nextNode(node);
    return *this;
  }

  CTree_iterator operator++(int) {
    auto iter = *this;
    ++(*this);
    return iter;
  }

  CTree_iterator &operator--() {
    node = tree->prevNode(node);
    return *this;
  }

  CTree_iterator operator--(int) {
    auto iter = *this;
    --(*this);
    return iter;
  }

  bool operator!=(const CTree_iterator &it) const noexcept {
    return tree != it.tree || node != it.node;
  }

  bool operator==(const CTree_iterator &it) const noexcept {
    return tree == it.tree && node == it.node;
  }

  reference operator*() { return node->item; }
};

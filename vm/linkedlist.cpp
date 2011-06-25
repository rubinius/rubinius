#include "linkedlist.hpp"
#include <assert.h>
#include <iostream>

LinkedList::LinkedList() : head_(NULL), count_(0) { }
LinkedList::Node::Node() : next_(NULL), prev_(NULL) { }

size_t LinkedList::size() const {
  return count_;
}

void LinkedList::add(LinkedList::Node* node) {
  assert(!node->next() && !node->prev());
  // std::cout << "add: " << node << "\n";

  if(head_) {
    assert(!head_->prev());
    head_->set_prev(node);
  }

  node->set_next(head_);
  head_ = node;

  count_++;
}

void LinkedList::remove(LinkedList::Node* node) {
  // std::cout << "remove: " << node << "\n";

  if(node == head_) {
    head_ = node->next();
  }

  node->remove_linkage();
  count_--;

  if(head_) {
    assert(!head_->prev());
  }
}

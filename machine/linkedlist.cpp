#include "linkedlist.hpp"
#include <assert.h>
#include <iostream>

namespace rubinius {
  void LinkedList::add(LinkedList::Node* node) {
    std::lock_guard<locks::spinlock_mutex> guard(lock_);

    assert(!node->next() && !node->prev());

    if(head_) {
      assert(!head_->prev());
      head_->set_prev(node);
    }

    node->set_next(head_);
    head_ = node;

    count_++;
  }

  void LinkedList::remove(LinkedList::Node* node) {
    std::lock_guard<locks::spinlock_mutex> guard(lock_);

    if(node == head_) {
      head_ = node->next();
    }

    node->remove_linkage();
    count_--;

    if(head_) {
      assert(!head_->prev());
    }
  }
}

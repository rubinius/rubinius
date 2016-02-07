#ifndef RBX_LINKEDLIST_HPP
#define RBX_LINKEDLIST_HPP

#include <stdio.h>
#include <assert.h>

class LinkedList {
public:

  class Node {
  private:
    Node* next_;
    Node* prev_;

  public:
    Node();

    Node* next() const {
      return next_;
    }

    Node* prev() const {
      return prev_;
    }

    void set_next(Node* n) {
      next_ = n;
    }

    void set_prev(Node* n) {
      prev_ = n;
    }

    void remove_linkage() {
      if(next_) {
        assert(next_->prev() == this);
        next_->set_prev(prev_);
      }

      if(prev_) {
        assert(prev_->next() == this);
        prev_->set_next(next_);
      }

      next_ = NULL;
      prev_ = NULL;
    }
  };

private:
  Node* head_;
  size_t count_;

public:
  LinkedList();
  Node* head() const {
    return head_;
  }

  size_t size() const;
  void   add(Node*);
  void   remove(Node*);

  // Utility templates
  template <typename Roots, typename Root>
  class Iterator {
    Roots& roots_;
    Root* current_;

  public:
    Iterator(Roots& roots)
      : roots_(roots)
      , current_(roots.front())
    {}

    bool more() const {
      return current_ != 0;
    }

    void advance() {
      current_ = static_cast<Root*>(current_->next());
    }

    Root* operator->() const {
      return current_;
    }

    Root* current() const {
      return current_;
    }

    Root* next() const {
      Root* ret = current_;
      if(current_) {
        current_ = static_cast<Root*>(current_->next());
      }

      return ret;
    }
  };


};

#endif

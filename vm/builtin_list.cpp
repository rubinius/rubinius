#include "builtin.hpp"

namespace rubinius {

  /* Register the List and List::Node classes as globals */
  void List::init(STATE) {
    Class* cls;
    cls = state->new_class("List", List::fields);
    GO(list).set(cls);
    cls->set_object_type(ListType);

    GO(list_node).set(state->new_class("Node", G(object),
                                                List::Node::fields, cls));
  }

  /* Create a new List object, containing no elements. */
  List* List::create(STATE) {
    List* list = (List*)state->new_object(G(list));
    list->count = Object::i2n(0);

    return list;
  }

  /* Append +obj+ to the current List. */
  void List::append(STATE, OBJECT obj) {
    List::Node* node = (List::Node*)state->new_object(G(list_node));
    SET(node, object, obj);
    List::Node* cur_last = last;

    if(!cur_last->nil_p()) {
      SET(cur_last, next, node);
    }

    SET(this, last, node);

    if(first->nil_p()) {
      SET(this, first, node);
    }

    count = Object::i2n(state, count->n2i() + 1);
  }

  /* Return the +index+ numbered element from the beginning. */
  OBJECT List::locate(STATE, size_t index) {
    Node* cur = first;

    while(index > 0) {
      if(cur->nil_p()) return Qnil;

      cur = cur->next;
      index--;
    }

    if(cur->nil_p()) return Qnil;
    return cur->object;
  }

  /* Return the first element in the list and remove it, moving all
   * other elements forward. */
  OBJECT List::shift(STATE) {
    if(empty_p()) return Qnil;

    count = Object::i2n(state, count->n2i() - 1);
    List::Node* n = first;
    SET(this, first, first->next);

    if(last == n) {
      last = (Node*)Qnil;
    }

    return n->object;
  }

  /* Search the List for +obj+ and remove all instances of it.
   *
   * Returns the number of elements removed. */
  size_t List::remove(STATE, OBJECT obj) {
    if(empty_p()) return 0;

    size_t deleted = 0, counted = 0;

    Node* node = first;
    Node* lst =  (Node*)Qnil;
    Node* nxt =  (Node*)Qnil;

    while(!node->nil_p()) {
      nxt = node->next;

      if(node->object == obj) {
        deleted++;
        if(lst->nil_p()) {
          SET(this, first, nxt);
        } else {
          SET(lst, next, nxt);
        }

        if(last == node) {
          SET(this, last, lst);
        }

        lst = (Node*)Qnil;
      } else {
        counted++;
        lst = node;
      }

      node = nxt;
    }

    count = Object::i2n(state, counted);

    return deleted;
  }
};


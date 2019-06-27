#include "memory.hpp"
#include "object_utils.hpp"

#include "class/class.hpp"
#include "class/fixnum.hpp"
#include "class/list.hpp"

namespace rubinius {

  /* Returns true if the List is empty, contains no elements. */
  bool List::empty_p() {
    return count()->to_native() == 0;
  }

  /* Returns the number of elements in the List. */
  size_t List::size() {
    return count()->to_native();
  }

  /* Register the List and List::Node classes as globals */
  void List::bootstrap(STATE) {
    GO(list).set(state->memory()->new_class<Class, List>(state, G(rubinius), "List"));

    GO(list_node).set(state->memory()->new_class<Class, ListNode>(
          state, G(object), G(list), "Node"));
  }

  /* Create a new List object, containing no elements. */
  List* List::create(STATE) {
    List* list = state->memory()->new_object<List>(state, G(list));
    list->count(state, Fixnum::from(0));

    return list;
  }

  List* List::clear(STATE) {
    first(nil<ListNode>());
    last(nil<ListNode>());
    count(state, Fixnum::from(0));

    return this;
  }

  /* Append +obj+ to the current List. */
  void List::append(STATE, Object* obj) {
    ListNode* node = state->memory()->new_object<ListNode>(state, G(list_node));
    node->object(state, obj);
    ListNode* cur_last = last();

    if(!cur_last->nil_p()) {
      cur_last->next(state, node);
    }

    last(state, node);

    if(first()->nil_p()) {
      first(state, node);
    }

    count(state, Fixnum::from(count()->to_native() + 1));
  }

  /* Return the +index+ numbered element from the beginning. */
  Object* List::locate(STATE, size_t index) {
    ListNode* cur = first();

    while(index > 0) {
      if(cur->nil_p()) return cNil;

      cur = cur->next();
      index--;
    }

    if(cur->nil_p()) return cNil;
    return cur->object();
  }

  /* Return the first element in the list and remove it, moving all
   * other elements forward. */
  Object* List::shift(STATE) {
    if(empty_p()) return cNil;

    count(state, Fixnum::from(count()->to_native() - 1));
    ListNode* n = first();
    first(state, first()->next());

    if(last()->equal_p(n)) {
      last(state, nil<ListNode>());
    }

    return n->object();
  }

  /* Search the List for +obj+ and remove all instances of it.
   *
   * Returns the number of elements removed. */
  size_t List::remove(STATE, const Object* obj) {
    if(empty_p()) return 0;

    size_t deleted = 0, counted = 0;

    ListNode* node = first();
    ListNode* lst =  nil<ListNode>();
    ListNode* nxt;

    while(!node->nil_p()) {
      nxt = node->next();

      if(node->object()->equal_p(obj)) {
        deleted++;
        if(lst->nil_p()) {
          first(state, nxt);
        } else {
          lst->next(state, nxt);
        }

        if(last()->equal_p(node)) {
          last(state, lst);
        }

        lst = nil<ListNode>();
      } else {
        counted++;
        lst = node;
      }

      node = nxt;
    }

    count(state, Fixnum::from(counted));

    return deleted;
  }

}


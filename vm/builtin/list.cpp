#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/list.hpp"
#include "object_utils.hpp"
#include "ontology.hpp"

namespace rubinius {

  /* Returns true if the List is empty, contains no elements. */
  bool List::empty_p() {
    return count_->to_native() == 0;
  }

  /* Returns the number of elements in the List. */
  size_t List::size() {
    return count_->to_native();
  }

  /* Register the List and List::Node classes as globals */
  void List::init(STATE) {
    Class* cls;
    cls = ontology::new_class_under(state, "List", G(rubinius));

    GO(list).set(cls);
    cls->set_object_type(state, ListType);

    GO(list_node).set(ontology::new_class_under(state,
          "Node", cls));

    G(list_node)->set_object_type(state, ListNodeType);
  }

  /* Create a new List object, containing no elements. */
  List* List::create(STATE) {
    List* list = state->new_object<List>(G(list));
    list->count(state, Fixnum::from(0));

    return list;
  }

  List* List::clear(STATE) {
    first_ = nil<ListNode>();
    last_ = nil<ListNode>();
    count(state, Fixnum::from(0));

    return this;
  }

  /* Append +obj+ to the current List. */
  void List::append(STATE, Object* obj) {
    ListNode* node = state->new_object<ListNode>(G(list_node));
    node->object(state, obj);
    ListNode* cur_last = last_;

    if(!cur_last->nil_p()) {
      cur_last->next(state, node);
    }

    last(state, node);

    if(first_->nil_p()) {
      first(state, node);
    }

    count(state, Integer::from(state, count_->to_native() + 1));
  }

  /* Return the +index+ numbered element from the beginning. */
  Object* List::locate(STATE, size_t index) {
    ListNode* cur = first_;

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

    count(state, Integer::from(state, count_->to_native() - 1));
    ListNode* n = first_;
    first(state, first_->next());

    if(last_ == n) {
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

    ListNode* node = first_;
    ListNode* lst =  nil<ListNode>();
    ListNode* nxt;

    while(!node->nil_p()) {
      nxt = node->next();

      if(node->object() == obj) {
        deleted++;
        if(lst->nil_p()) {
          first(state, nxt);
        } else {
          lst->next(state, nxt);
        }

        if(last_ == node) {
          last(state, lst);
        }

        lst = nil<ListNode>();
      } else {
        counted++;
        lst = node;
      }

      node = nxt;
    }

    count(state, Integer::from(state, counted));

    return deleted;
  }

}


#ifndef RBX_BUILTIN_LIST_HPP
#define RBX_BUILTIN_LIST_HPP

#include "objects.hpp"

namespace rubinius {

  class List : public BuiltinType {
    public:

    class Node : public BuiltinType {
      public:

      const static size_t fields = 2;

      OBJECT object;
      Node* next;
    };
      
    const static size_t fields = 3;
    const static object_type type = ListType;

    INTEGER count;
    Node* first;
    Node* last;

    /* Returns true if the List is empty, contains no elements. */
    bool empty_p() {
      return count->n2i() == 0;
    }

    /* Returns the number of elements in the List. */
    size_t size() {
      return count->n2i();
    }

    static void init(STATE);
    static List* create(STATE);
    void append(STATE, OBJECT obj);
    OBJECT shift(STATE);
    OBJECT locate(STATE, size_t index);
    size_t remove(STATE, OBJECT obj);
  };

};
#endif

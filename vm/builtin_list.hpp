#ifndef RBX_BUILTIN_LIST_HPP
#define RBX_BUILTIN_LIST_HPP

#include "prelude.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
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

    OBJECT count;
    Node* first;
    Node* last;

    bool empty_p() {
      return count->n2i() == 0;
    }

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

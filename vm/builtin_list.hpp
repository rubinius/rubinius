#ifndef RBX_BUILTIN_LIST_HPP
#define RBX_BUILTIN_LIST_HPP

#include "objects.hpp"

namespace rubinius {

  class ListNode : public BuiltinType {
  public:

    const static size_t fields = 2;
    const static object_type type = ListNodeType;

    OBJECT object; // slot
    ListNode* next; // slot

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

  class List : public BuiltinType {
  public:

    const static size_t fields = 4;
    const static object_type type = ListType;

    OBJECT __ivars__; // slot
    INTEGER count; // slot
    ListNode* first; // slot
    ListNode* last; // slot

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

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

  };

};
#endif

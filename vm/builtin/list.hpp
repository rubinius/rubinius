#ifndef RBX_BUILTIN_LIST_HPP
#define RBX_BUILTIN_LIST_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {

  class ListNode : public Object {
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

  class List : public Object {
  public:

    const static size_t fields = 3;
    const static object_type type = ListType;

    INTEGER count; // slot
    ListNode* first; // slot
    ListNode* last; // slot

    bool empty_p();
    size_t size();
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

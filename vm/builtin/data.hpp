#ifndef RBX_BUILTIN_DATA_HPP
#define RBX_BUILTIN_DATA_HPP

#include "builtin/object.hpp"
#include "builtin/nativemethodcontext.hpp"
#include "type_info.hpp"

namespace rubinius {
  class Data : public Object {
  public:
    const static size_t fields = 0;
    const static object_type type = DataType;

    void (*mark_) (void*);
    void (*free_) (void*);
    void *data_;

    NativeMethodContext* mark_context; // slot

    static void init(STATE);

    static Data* create(STATE, void* data,
                        void (*mark) (void*),
                        void (*free) (void*));

    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(Object* t, ObjectMark& mark);
    };
  };
};

#endif

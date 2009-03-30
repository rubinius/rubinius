#ifndef RBX_BUILTIN_DATA_HPP
#define RBX_BUILTIN_DATA_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {

  class Data : public Object {
  public:
    const static object_type type = DataType;

  public:   /* Types */

    /** The signature for the mark function. */
    typedef   void (*MarkFunctor)(void*);

    /** The signature for the free function. */
    typedef   void (*FreeFunctor)(void*);

  private:
    FreeFunctor free_;
    MarkFunctor mark_;
    void*       data_;

  public:   /* Interface */

    /**  Register class with the VM. */
    static void   init(STATE);

    /** New Data instance. */
    static Data*  create(STATE, void* data, MarkFunctor mark, FreeFunctor free);

    void* data() {
      return data_;
    }

    FreeFunctor free() {
      return free_;
    }

    MarkFunctor mark() {
      return mark_;
    }

    void** data_address() {
      return &data_;
    }

    void data(STATE, void* data) {
      data_ = data;
    }

    void free(STATE, FreeFunctor free) {
      free_ = free;
    }

    void mark(STATE, MarkFunctor mark) {
      mark_ = mark;
    }

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };

}

#endif

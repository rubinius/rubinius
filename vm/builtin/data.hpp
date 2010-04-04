#ifndef RBX_BUILTIN_DATA_HPP
#define RBX_BUILTIN_DATA_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {

  // HACK manually copied here from ruby.h
  struct RDataExposed {
    void (*dmark)(void*);
    void (*dfree)(void*);
    void *data;
  };

  class Data : public Object {
  public:
    const static object_type type = DataType;

  public:   /* Types */
    /** The signature for the mark function. */
    typedef   void (*MarkFunctor)(void*);

    /** The signature for the free function. */
    typedef   void (*FreeFunctor)(void*);

  private:
    RDataExposed exposed_;

  public:   /* Interface */

    /**  Register class with the VM. */
    static void   init(STATE);

    /** New Data instance. */
    static Data*  create(STATE, void* data, MarkFunctor mark, FreeFunctor free);

    static void finalize(STATE, Data* data);

    RDataExposed* exposed() {
      return &exposed_;
    }

    void* data() {
      return exposed_.data;
    }

    FreeFunctor free() {
      return exposed_.dfree;
    }

    MarkFunctor mark() {
      return exposed_.dmark;
    }

    void** data_address() {
      return &exposed_.data;
    }

    void data(STATE, void* data) {
      exposed_.data = data;
    }

    void free(STATE, FreeFunctor free) {
      exposed_.dfree = free;
    }

    void mark(STATE, MarkFunctor mark) {
      exposed_.dmark = mark;
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

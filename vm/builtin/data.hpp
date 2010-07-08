#ifndef RBX_BUILTIN_DATA_HPP
#define RBX_BUILTIN_DATA_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"

namespace rubinius {
  // Copied from here because you can't include capi/include/ruby.h into
  // our C++ files.
  struct RDataShadow {
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
    void (*dmark_)(void*);
    void (*dfree_)(void*);
    void *data_;

  public:   /* Interface */

    /**  Register class with the VM. */
    static void   init(STATE);

    /** New Data instance. */
    static Data*  create(STATE, void* data, MarkFunctor mark, FreeFunctor free);

    static void finalize(STATE, Data* data);

    RDataShadow* rdata(STATE);
    void* data(STATE);
    FreeFunctor free(STATE);
    MarkFunctor mark(STATE);

  public:   /* TypeInfo */

    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void auto_mark(Object* obj, ObjectMark& mark) {}
    };
  };

}

#endif

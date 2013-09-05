#ifndef RBX_BUILTIN_DATA_HPP
#define RBX_BUILTIN_DATA_HPP

#include "builtin/object.hpp"

namespace rubinius {
  // Copied from here because you can't include capi/include/ruby.h into
  // our C++ files.

  struct rb_data_type_struct_shadow {
    const char *wrap_struct_name;
    struct {
      void (*dmark)(void*);
      void (*dfree)(void*);
      size_t (*dsize)(const void *);
      void *reserved[2]; /* For future extension.
                            This array *must* be filled with ZERO. */
    } function;
    const struct rb_data_type_struct_shadow* parent;
    void *data;        /* This area can be used for any purpose
                          by a programmer who define the type. */
  };

  // This union crazyness is needed because these should be compatible
  // for the data pointer. It is valid behavior to use DATA_PTR on both
  // typed and untyped and get a valid result back.
  // MRI constructs this differently, but this approach allows us to
  // use this in a slightly more type safe and explicit way.
  struct RUntypedDataShadow {
    void (*dmark)(void*);
    void (*dfree)(void*);
  };

  struct RTypedDataShadow {
    const struct rb_data_type_struct_shadow* type;
    native_int typed;
  };

  struct RDataShadow {
    union {
      RUntypedDataShadow untyped;
      RTypedDataShadow   typed;
    } d;
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
    RDataShadow* internal_;
    bool freed_;

  public:   /* Interface */

    /**  Register class with the VM. */
    static void   init(STATE);

    /** New Data instance. */
    static Data*  create(STATE, void* data, MarkFunctor mark, FreeFunctor free);

    /** New typed Data instance. */
    static Data*  create_typed(STATE, void* data, const struct rb_data_type_struct_shadow* type);

    static void finalize(STATE, Data* data);

    bool freed_p() const {
      return freed_;
    }

    void set_freed() {
      freed_ = true;
    }

    RDataShadow* rdata(STATE) {
      return internal_;
    }

    RDataShadow* rdata() {
      return internal_;
    }

    bool typed() {
      return rdata()->d.typed.typed == 1;
    }

    const struct rb_data_type_struct_shadow* data_type() {
      if(typed()) {
        return rdata()->d.typed.type;
      }
      return NULL;
    }

    void* data();
    FreeFunctor free();
    MarkFunctor mark();

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

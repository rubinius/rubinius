#ifndef RBX_BUILTIN_DATA_HPP
#define RBX_BUILTIN_DATA_HPP

#include "class/object.hpp"

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
    intptr_t typed;
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
    attr_field(internal, RDataShadow*);
    attr_field(freed, bool);

  public:   /* Interface */

    /**  Register class with the VM. */
    static void bootstrap(STATE);
    static void initialize(STATE, Data* obj) {
      obj->internal(NULL);
      obj->freed(false);
    }

    /** New Data instance. */
    static Data*  create(STATE, void* data, MarkFunctor mark, FreeFunctor free);

    /** New typed Data instance. */
    static Data*  create_typed(STATE, void* data, const struct rb_data_type_struct_shadow* type);

    static void finalize(STATE, Data* data);

    bool freed_p() const {
      return freed();
    }

    void set_freed() {
      freed(true);
    }

    RDataShadow* rdata(STATE) {
      return internal();
    }

    RDataShadow* rdata() {
      return internal();
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
      virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {}
    };
  };

}

#endif

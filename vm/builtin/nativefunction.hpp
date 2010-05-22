#ifndef RBX_BUILTIN_NATIVEFUNCTION_HPP
#define RBX_BUILTIN_NATIVEFUNCTION_HPP

#include "builtin/executable.hpp"

#include <ffi.h>

namespace rubinius {
  class NativeFunction : public Executable {
  public:
    static const object_type type = NativeFunctionType;

  private:
    Symbol* name_;         // slot
    Symbol* file_;         // slot
    Fixnum* required_;     // slot

  public:
    ffi_cif cif;
    size_t arg_count;
    int *arg_types;
    int ret_type;
    void *ep;

    /* accessors */

    attr_accessor(name, Symbol);
    attr_accessor(file, Symbol);
    attr_accessor(required, Fixnum);

    /* interface */

    static size_t type_size(size_t type);

    // Ruby.primitive :nativefunction_type_size
    static Fixnum* type_size_prim(STATE, Fixnum* type);

    static void init(STATE);

    static NativeFunction* create(STATE, Symbol* name, int args);
    static Object* execute(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args);

    // Ruby.primitive :nativefunction_bind
    static NativeFunction* bind(STATE, Object* library, String* name, Array* args, Object* ret);

    void bind(STATE, int arg_count, int *arg_types, int ret_type, void* func);
    Object* call(STATE, Arguments& args, CallFrame* call_frame);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
    };

  };

}

#endif

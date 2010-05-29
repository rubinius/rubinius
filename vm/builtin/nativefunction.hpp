#ifndef RBX_BUILTIN_NATIVEFUNCTION_HPP
#define RBX_BUILTIN_NATIVEFUNCTION_HPP

#include "builtin/executable.hpp"
#include "gc/code_resource.hpp"

#include <ffi.h>

namespace rubinius {
  class FFIData;

  class NativeFunction : public Executable {
  public:
    static const object_type type = NativeFunctionType;

  private:
    Symbol* name_;         // slot
    Symbol* file_;         // slot
    Fixnum* required_;     // slot
    NativeFunction* callback_info_; // slot

  public:
    FFIData* ffi_data;

    /* accessors */

    attr_accessor(name, Symbol);
    attr_accessor(file, Symbol);
    attr_accessor(required, Fixnum);
    attr_accessor(callback_info, NativeFunction);

    /* interface */

    static size_t type_size(size_t type);

    // Ruby.primitive :nativefunction_type_size
    static Fixnum* type_size_prim(STATE, Fixnum* type);

    static void init(STATE);

    static NativeFunction* create(STATE, Symbol* name, int args);
    static Object* execute(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args);

    // Ruby.primitive :nativefunction_generate
    static NativeFunction* generate(STATE, Pointer* ptr, Symbol* name, Array* args, Object* ret);

    // Ruby.primitive :nativefunction_generate_tramp
    static Array* generate_tramp(STATE, Object* obj, Symbol* name, Array* args, Object* ret);

    static Pointer* adjust_tramp(STATE, Object* obj, NativeFunction* orig);

    void prep(STATE, int arg_count, int *arg_types, int ret_type);
    Object* call(STATE, Arguments& args, CallFrame* call_frame);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
      virtual void mark(Object* t, ObjectMark& mark);
    };

  };

  class FFIData: public CodeResource {
  public:
    ffi_cif cif;
    ffi_closure* closure;
    Object* callable;
    NativeFunction* function;

    size_t arg_count;
    int *arg_types;
    int ret_type;
    void *ep;

    FFIData(NativeFunction* func,  int count, int* types, int ret);

    virtual ~FFIData();

    static FFIData* create(NativeFunction* func, int count, int* types, int ret);
  };

}

#endif

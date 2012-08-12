#ifndef RBX_BUILTIN_NATIVEFUNCTION_HPP
#define RBX_BUILTIN_NATIVEFUNCTION_HPP

#include "builtin/executable.hpp"
#include "gc/code_resource.hpp"

#include <ffi.h>

namespace rubinius {
  class FFIData;
  struct FFIArgInfo;

  class NativeFunction : public Executable {
  public:
    static const object_type type = NativeFunctionType;

  private:
    Symbol* name_;         // slot
    Symbol* file_;         // slot
    Fixnum* required_;     // slot
    Object* varargs_;      // slot

  public:
    FFIData* ffi_data;

    /* accessors */

    attr_accessor(name, Symbol);
    attr_accessor(file, Symbol);
    attr_accessor(required, Fixnum);
    attr_accessor(varargs, Object);

    /* interface */

    static size_t type_size(size_t type);

    // Rubinius.primitive :nativefunction_type_size
    static Fixnum* type_size_prim(STATE, Fixnum* type);

    static void init(STATE);

    static NativeFunction* create(STATE, Symbol* name, int args);
    static Object* execute(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

    // Rubinius.primitive :nativefunction_generate
    static NativeFunction* generate(STATE, Pointer* ptr, Symbol* name, Array* args, Object* ret);

    // Rubinius.primitive :nativefunction_generate_tramp
    static Array* generate_tramp(STATE, Object* obj, Symbol* name, Array* args, Object* ret);

    static Pointer* adjust_tramp(STATE, Object* obj, NativeFunction* orig);

    static bool ffi_arg_info(STATE, Object* type, FFIArgInfo* info);
    static ffi_type* ffi_type_info(FFIArgInfo* info);

    void prep(STATE, int arg_count, FFIArgInfo* args, FFIArgInfo* ret);
    Object* call(STATE, Arguments& args, CallFrame* call_frame);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
      virtual void mark(Object* t, ObjectMark& mark);
    };

  };

  struct FFIArgInfo {
    int type;
    Object* enum_obj;
    NativeFunction* callback;
  };

  class FFIData: public CodeResource {
  public:
    ffi_cif cif;
    ffi_closure* closure;
    SharedState* shared;
    Object* callable;
    NativeFunction* function;
    FFIArgInfo* args_info;
    FFIArgInfo ret_info;

    size_t arg_count;
    void *ep;

    FFIData(STATE, NativeFunction* func,  int count, FFIArgInfo* args, FFIArgInfo* ret);

    virtual ~FFIData();
    void cleanup(State* state, CodeManager* cm) { }

    static FFIData* create(STATE, NativeFunction* func, int count, FFIArgInfo* args, FFIArgInfo* ret);
  };

}

#endif

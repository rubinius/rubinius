#ifndef RBX_BUILTIN_NATIVEFUNCTION_HPP
#define RBX_BUILTIN_NATIVEFUNCTION_HPP

#include "object_utils.hpp"

#include "class/executable.hpp"
#include "class/fixnum.hpp"
#include "class/symbol.hpp"

#include "memory/code_resource.hpp"

#include <ffi.h>

namespace rubinius {
  class FFIData;
  struct FFIArgInfo;

  class NativeFunction : public Executable {
  public:
    static const object_type type = NativeFunctionType;

    attr_accessor(name, Symbol);
    attr_accessor(file, Symbol);
    attr_accessor(required, Fixnum);
    attr_accessor(varargs, Object);

    attr_field(ffi_data, FFIData*);

    static size_t type_size(size_t type);

    // Rubinius.primitive :nativefunction_type_size
    static Fixnum* type_size_prim(STATE, Fixnum* type);

    static void bootstrap(STATE);
    static void initialize(STATE, NativeFunction* obj) {
      Executable::initialize(state, obj);

      obj->name(nil<Symbol>());
      obj->file(nil<Symbol>());
      obj->required(Fixnum::from(0));
      obj->varargs(cFalse);
      obj->ffi_data(NULL);
    }

    static NativeFunction* create(STATE, Symbol* name, int args);
    static Object* execute(STATE, Executable* exec, Module* mod, Arguments& args);

    // Rubinius.primitive :nativefunction_generate
    static NativeFunction* generate(STATE, Pointer* ptr, Symbol* name, Array* args, Object* ret);

    // Rubinius.primitive :nativefunction_generate_tramp
    static Array* generate_tramp(STATE, Object* obj, Symbol* name, Array* args, Object* ret);

    static Pointer* adjust_tramp(STATE, Object* obj, NativeFunction* orig);

    static bool ffi_arg_info(STATE, Object* type, FFIArgInfo* info);
    static ffi_type* ffi_type_info(FFIArgInfo* info);

    void prep(STATE, int arg_count, FFIArgInfo* args, FFIArgInfo* ret);
    Object* call(STATE, Arguments& args);

    class Info : public Executable::Info {
    public:
      BASIC_TYPEINFO(Executable::Info)
      virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
    };

  };

  struct FFIArgInfo {
    int type;
    Object* enum_obj;
    NativeFunction* callback;
  };

  class FFIData: public memory::CodeResource {
  public:
    ffi_cif cif;
    ffi_closure* closure;
    Object* callable;
    NativeFunction* function;
    FFIArgInfo* args_info;
    FFIArgInfo ret_info;

    size_t arg_count;
    void *ep;

    FFIData(STATE, NativeFunction* func,  int count, FFIArgInfo* args, FFIArgInfo* ret);

    virtual ~FFIData();
    void cleanup(ThreadState* state, memory::CodeManager* cm) { }

    static FFIData* create(STATE, NativeFunction* func, int count, FFIArgInfo* args, FFIArgInfo* ret);
  };

}

#endif

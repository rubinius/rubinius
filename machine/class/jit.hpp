#ifndef RBX_BUILTIN_JIT_HPP
#define RBX_BUILTIN_JIT_HPP

#include "object_utils.hpp"

#include "class/array.hpp"
#include "class/block_environment.hpp"
#include "class/compiled_code.hpp"
#include "class/class.hpp"
#include "class/list.hpp"
#include "class/module.hpp"
#include "class/object.hpp"

namespace rubinius {
  class JITCompileRequest : public Object {
  public:
    const static object_type type = JITCompileRequestType;

    attr_accessor(method, CompiledCode);
    attr_accessor(receiver_class, Class);
    attr_accessor(block_env, BlockEnvironment);

  private:
    attr_field(waiter, std::condition_variable*);
    attr_field(hits, int);
    attr_field(is_block, bool);

  public:
    static void initialize(STATE, JITCompileRequest* obj) {
      obj->method(nil<CompiledCode>());
      obj->receiver_class(nil<Class>());
      obj->block_env(nil<BlockEnvironment>());
      obj->waiter(NULL);
      obj->hits(0);
      obj->is_block(false);
    }

    static JITCompileRequest* create(STATE, CompiledCode* code, Class* receiver_class,
        int hits, BlockEnvironment* block_env = NULL, bool is_block = false);

    MachineCode* machine_code() {
      return method()->machine_code();
    }

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class JIT : public Module {
  public:
    const static object_type type = JITType;

    attr_accessor(compile_class, Class);
    attr_accessor(compile_list, List);

    static void bootstrap(STATE);
    static void initialize(STATE, JIT* obj) {
      obj->compile_class(nil<Class>());
      obj->compile_list(nil<List>());
    }

    static void initialize(STATE, JIT* obj, Module* under, const char* name);

    Object* enable(STATE);

    Object* compile_soon(STATE, CompiledCode* code,
        Class* receiver_class, BlockEnvironment* block_env=NULL, bool is_block=false);
    Object* compile_callframe(STATE, CompiledCode* code,
        int primitive=-1);
    Object* start_method_update(STATE);
    Object* end_method_update(STATE);

    // Rubinius.primitive :jit_compile
    Object* compile(STATE, Object* object, CompiledCode* code, Object* block_environment);

    // Rubinius.primitive :jit_enabled_p
    Object* enabled_p(STATE);

    // Rubinius.primitive :jit_compile_threshold
    Object* compile_threshold(STATE);

    // Rubinius.primitive :jit_sync_set
    Object* sync_set(STATE, Object* flag);

    // Rubinius.primitive :jit_sync_get
    Object* sync_get(STATE);

    class Info : public Module::Info {
    public:
      BASIC_TYPEINFO(Module::Info)
    };
  };
}

#endif

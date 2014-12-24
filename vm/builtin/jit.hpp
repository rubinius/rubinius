#ifndef RBX_BUILTIN_JIT_HPP
#define RBX_BUILTIN_JIT_HPP

#include "object_utils.hpp"

#include "builtin/compiled_code.hpp"
#include "builtin/class.hpp"
#include "builtin/module.hpp"
#include "builtin/object.hpp"

#include "util/thread.hpp"

namespace rubinius {
  class Array;
  class BlockEnvironment;
  class CompiledCode;
  class Class;
  class List;

  class JITCompileRequest : public Object {
  public:
    const static object_type type = JITCompileRequestType;

  private:
    CompiledCode* method_;        // slot
    Class* receiver_class_;       // slot
    BlockEnvironment* block_env_; // slot

    utilities::thread::Condition* waiter_;
    int hits_;
    bool is_block_;

  public:
    /* accessors */

    attr_accessor(method, CompiledCode);
    attr_accessor(receiver_class, Class);
    attr_accessor(block_env, BlockEnvironment);

  public:

    static JITCompileRequest* create(STATE, CompiledCode* code, Class* receiver_class,
        int hits, BlockEnvironment* block_env = NULL, bool is_block = false);

    MachineCode* machine_code() {
      return method_->machine_code();
    }

    CompiledCode* method() {
      return method_;
    }

    BlockEnvironment* block_env() {
      return block_env_;
    }

    Class* receiver_class() {
      return receiver_class_;
    }

    bool is_block() {
      return is_block_;
    }

    int hits() {
      return hits_;
    }

    void set_waiter(utilities::thread::Condition* cond) {
      waiter_ = cond;
    }

    utilities::thread::Condition* waiter() {
      return waiter_;
    }

    void set_method(CompiledCode* meth) {
      method_ = meth;
    }

    void set_receiver_class(Class* receiver_class) {
      receiver_class_ = receiver_class;
    }

    void set_block_env(BlockEnvironment* block_env) {
      block_env_ = block_env;
    }
    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };
  };

  class JIT : public Module {
  public:
    const static object_type type = JITType;

  private:
    Class* compile_class_;        // slot
    List* compile_list_;          // slot
    Object* available_;           // slot
    Object* enabled_;             // slot
    Array* properties_;           // slot

  public:
    /* accessors */
    attr_accessor(compile_class, Class);
    attr_accessor(compile_list, List);
    attr_accessor(available, Object);
    attr_accessor(enabled, Object);
    attr_accessor(properties, Array);

  public:
    static void init(STATE);

    Object* enable(STATE);

    Object* compile_soon(STATE, CompiledCode* code, CallFrame* call_frame,
        Class* receiver_class, BlockEnvironment* block_env=NULL, bool is_block=false);
    Object* compile_callframe(STATE, CompiledCode* code, CallFrame* call_frame,
        int primitive=-1);
    Object* start_method_update(STATE);
    Object* end_method_update(STATE);

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

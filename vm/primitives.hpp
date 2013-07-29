#ifndef RBX_PRIMITIVES_HPP
#define RBX_PRIMITIVES_HPP

#include "executor.hpp"
#include "prelude.hpp"

#include <stdexcept>

namespace rubinius {

  /* Forwards */
  class Object;
  class Primitives;
  class Symbol;


  enum PrimitiveRuntimeCode {
    // A weird value, but works well. It looks like a reference to the highest
    // address in memory.
    kPrimitiveFailed = ((unsigned int)-1) & ~TAG_REF_MASK
  };

#ifdef ENABLE_LLVM
  class JITStubResults {
    int arg_count_;
    const char* name_;
    bool pass_callframe_;
    bool can_fail_;

  public:
    JITStubResults()
      : arg_count_(0)
      , name_(0)
      , pass_callframe_(true)
      , can_fail_(false)
    {}

    void set_arg_count(int count) {
      arg_count_ = count;
    }

    int arg_count() const {
      return arg_count_;
    }

    void set_name(const char* name) {
      name_ = name;
    }

    const char* name() const {
      return name_;
    }

    void set_pass_callframe(bool val) {
      pass_callframe_ = val;
    }

    bool pass_callframe() const {
      return pass_callframe_;
    }

    void set_can_fail(bool val) {
      can_fail_ = val;
    }

    bool can_fail() const {
      return can_fail_;
    }

  };
#endif


  class Primitives {
  public:
    static Object* failure() {
      return reinterpret_cast<Object*>(kPrimitiveFailed);
    }

    /*
     * The primitive generator emits one 'executor' function per
     * primitive. This simply checks the argument types and then
     * calls the C++ code that implements the primitive.
     * See MachineCode::execute for the version that handles 'regular'
     * Ruby code.
     */
    static executor resolve_primitive(STATE, Symbol* name, int* index = 0);
    static Object* unknown_primitive(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);
#ifdef ENABLE_LLVM
    static bool get_jit_stub(int index, JITStubResults& res);
#endif
    static InvokePrimitive get_invoke_stub(STATE, Symbol* name);

#include "gen/primitives_declare.hpp"

  };
}

#endif

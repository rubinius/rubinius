#ifndef RBX_PRIMITIVES_HPP
#define RBX_PRIMITIVES_HPP

#include "executor.hpp"
#include "prelude.hpp"

#include <stdexcept>

namespace rubinius {

  /* Forwards */
  class Object;
  class Message;
  class Primitives;
  class Symbol;


  enum PrimitiveRuntimeCode {
    // A weird value, but works well. It looks like a reference to the highest
    // address in memory.
    kPrimitiveFailed = ((unsigned int)-1) & ~TAG_REF_MASK
  };

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

    int arg_count() {
      return arg_count_;
    }

    void set_name(const char* name) {
      name_ = name;
    }

    const char* name() {
      return name_;
    }

    void set_pass_callframe(bool val) {
      pass_callframe_ = val;
    }

    bool pass_callframe() {
      return pass_callframe_;
    }

    void set_can_fail(bool val) {
      can_fail_ = val;
    }

    bool can_fail() {
      return can_fail_;
    }

  };


  class Primitives {
  public:
    static Object* failure() {
      return reinterpret_cast<Object*>(kPrimitiveFailed);
    }

    static void queue_for_jit(STATE, CallFrame* call_frame, int which);

    /*
     * The primitive generator emits one 'executor' function per
     * primitive. This simply checks the argument types and then
     * calls the C++ code that implements the primitive.
     * See VMMethod::execute for the version that handles 'regular'
     * Ruby code.
     */
    static executor resolve_primitive(STATE, Symbol* name, int* index = 0);
    static Object* unknown_primitive(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);
    static bool get_jit_stub(int index, JITStubResults& res);
    static InvokePrimitive get_invoke_stub(STATE, Symbol* name);

#include "gen/primitives_declare.hpp"

  };
}

#endif

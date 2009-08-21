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

  public:
    JITStubResults()
      : arg_count_(0)
      , name_(0)
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

  };


  class Primitives {
  public:
    static Object* failure() {
      return reinterpret_cast<Object*>(kPrimitiveFailed);
    }

    /*
     * The primitive generator emits one 'executor' function per
     * primitive. This simply checks the argument types and then
     * calls the C++ code that implements the primitive.
     * See VMMethod::execute for the version that handles 'regular'
     * Ruby code.
     */
    static executor resolve_primitive(STATE, Symbol* name, int* index = 0);
    static Object* unknown_primitive(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args);
    static bool get_jit_stub(int index, JITStubResults& res);

#include "gen/primitives_declare.hpp"

  };
}

#endif

#ifndef RBX_BUILTIN_EXECUTABLE_HPP
#define RBX_BUILTIN_EXECUTABLE_HPP

#include <cassert>

#include "builtin/object.hpp"
#include "type_info.hpp"
#include "executor.hpp"

#include <list>

namespace rubinius {
  class MemoryPointer;
  class Dispatch;
  class Arguments;

  class VMMethod;
  typedef std::list<VMMethod*> Inliners;

  class Executable : public Object {
  public:
    const static object_type type = ExecutableType;

  private:
    Symbol* primitive_; // slot
    Fixnum* serial_;    // slot

  public:
    // This one is public so it can be directly invoked.
    executor execute;

  protected:
    int prim_index_;
    Inliners* inliners_;

  public:
    /* accessors */

    attr_accessor(primitive, Symbol);
    attr_accessor(serial, Fixnum);

    void set_executor(rubinius::executor exc) {
      execute = exc;
    }

    int prim_index() {
      return prim_index_;
    }

    /* interface */

    // Ruby.primitive :executable_allocate
    static Executable* allocate(STATE, Object* self);

    static void init(STATE);
    static Object* default_executor(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args);

    // Ruby.primitive :executable_invoke
    Object* invoke(STATE, Symbol* name, Module* mod, Object* recv, Array* args, Object* block, CallFrame* calling_environment);

    bool resolve_primitive(STATE);

    void add_inliner(VMMethod* vmm);
    void clear_inliners(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
    };

    friend class Info;
  };

}

#endif

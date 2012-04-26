#ifndef RBX_BUILTIN_EXECUTABLE_HPP
#define RBX_BUILTIN_EXECUTABLE_HPP

#include <assert.h>

#include "builtin/object.hpp"
#include "type_info.hpp"
#include "executor.hpp"
#include "gc/code_resource.hpp"

#include <list>

namespace rubinius {
  class Arguments;

  class VMMethod;
  class CompiledMethod;
  class ObjectMemory;

  class Inliners : public CodeResource {
    std::list<CompiledMethod*> inliners_;

  public:
    Inliners(ObjectMemory* om);

    std::list<CompiledMethod*>& inliners() {
      return inliners_;
    }

    void cleanup(State* state, CodeManager* cm);
  };

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

    // Rubinius.primitive :executable_allocate
    static Executable* allocate(STATE, Object* self);

    static void init(STATE);
    static Object* default_executor(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

    // Rubinius.primitive :executable_invoke
    Object* invoke(STATE, Symbol* name, Module* mod, Object* recv, Array* args, Object* block, CallFrame* calling_environment);

    bool resolve_primitive(STATE);

    void add_inliner(ObjectMemory* om, CompiledMethod* cm);
    void clear_inliners(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)

      virtual void mark(Object* obj, ObjectMark& mark);

      void mark_inliners(Object* obj, ObjectMark& mark);
    };

    friend class Info;
  };

}

#endif

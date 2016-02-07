#ifndef RBX_BUILTIN_EXECUTABLE_HPP
#define RBX_BUILTIN_EXECUTABLE_HPP

#include "executor.hpp"
#include "object_utils.hpp"

#include "builtin/fixnum.hpp"
#include "builtin/object.hpp"
#include "builtin/symbol.hpp"

#include "memory/code_resource.hpp"

#include <vector>

namespace rubinius {
  class Arguments;

  class MachineCode;
  class CompiledCode;
  class ObjectMemory;

  class Inliners : public CodeResource {
    std::vector<CompiledCode*> inliners_;

  public:
    Inliners(STATE, ObjectMemory* om);

    std::vector<CompiledCode*>& inliners() {
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
    Inliners* inliners_;
    int prim_index_;
    bool custom_call_site_;

  public:
    /* accessors */

    attr_accessor(primitive, Symbol);
    attr_accessor(serial, Fixnum);

    static void initialize(STATE, Executable* exc) {
      exc->primitive_ = nil<Symbol>();
      exc->serial_ = Fixnum::from(0);
      exc->execute = Executable::default_executor;
      exc->inliners_ = 0;
      exc->prim_index_ = -1;
      exc->custom_call_site_ = false;
    }

    static void initialize(STATE, Executable* exc, executor exec) {
      Executable::initialize(state, exc);

      exc->execute = exec;
    }

    void set_executor(rubinius::executor exc) {
      execute = exc;
    }

    int prim_index() const {
      return prim_index_;
    }

    bool custom_call_site_p() const {
      return custom_call_site_;
    }

    /* interface */

    // Rubinius.primitive :executable_allocate
    static Executable* allocate(STATE, Object* self);

    static void bootstrap(STATE);
    static Object* default_executor(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);

    // Rubinius.primitive :executable_invoke
    Object* invoke(STATE, Symbol* name, Module* mod, Object* recv, Array* args, Object* block, CallFrame* calling_environment);

    // Rubinius.primitive :executable_set_custom_call_site
    Object* set_custom_call_site(STATE) {
      custom_call_site_ = true;
      return cNil;
    }

    bool resolve_primitive(STATE);

    void add_inliner(STATE, ObjectMemory* om, CompiledCode* code);
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

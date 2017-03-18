#include "arguments.hpp"
#include "call_frame.hpp"
#include "dispatch.hpp"
#include "memory.hpp"
#include "machine_code.hpp"
#include "object_utils.hpp"

#include "class/class.hpp"
#include "class/executable.hpp"
#include "class/symbol.hpp"

namespace rubinius {

  void Executable::bootstrap(STATE) {
    GO(executable).set(state->memory()->new_class<Class, Executable>(
          state, G(rubinius), "Executable"));
    G(executable)->set_const(state, "Experimental", Fixnum::from(eExperimental));
    G(executable)->set_const(state, "Stack", Fixnum::from(eStack));
    G(executable)->set_const(state, "Register", Fixnum::from(eRegister));
    G(executable)->set_const(state, "Parsing", Fixnum::from(eParsing));
    G(executable)->set_const(state, "Assertion", Fixnum::from(eAssertion));
    G(executable)->set_const(state, "Instrumentation", Fixnum::from(eInstrumentation));
  }

  Executable* Executable::allocate(STATE, Object* self) {
    Executable* executable =
      state->memory()->new_object<Executable>(state, G(executable));

    Executable::initialize(state, executable, Executable::default_executor);

    if(Class* cls = try_as<Class>(self)) {
      executable->klass(state, cls);
    }

    return executable;
  }

  bool Executable::resolve_primitive(STATE) {
    if(!primitive()->nil_p()) {
      if(Symbol* name = try_as<Symbol>(primitive())) {
        set_executor(Primitives::resolve_primitive(state, name, &_prim_index_));
        return true;
      }
    }

    return false;
  }

  Object* Executable::invoke(STATE, Symbol* name, Module* mod, Object* recv, Array* ary,
                             Object* block)
  {
    Arguments args(name, recv, block, ary);
    return execute(state, this, mod, args);
  }

  Object* Executable::default_executor(STATE,
      Executable* exec, Module* mod, Arguments& args)
  {
    args.unshift2(state, args.recv(), args.name());
    args.set_recv(exec);

    Dispatch dispatch(G(sym_call));

    return dispatch.send(state, args);
  }

  void Executable::add_inliner(STATE, Memory* om, CompiledCode* code) {
    if(!inliners() || inliners() == (Inliners*)cNil) {
      inliners(new Inliners(state, om));
    }
    inliners()->inliners().push_back(code);

    om->write_barrier(this, code);
  }

  void Executable::clear_inliners(STATE) {
    if(!inliners() || inliners() == (Inliners*)cNil) return;
    for(std::vector<CompiledCode*>::const_iterator i = inliners()->inliners().begin();
        i != inliners()->inliners().end();
        ++i) {
      (*i)->machine_code()->deoptimize(state, *i, 0);
    }

    inliners()->inliners().clear();
  }

  void Executable::Info::mark(Object* obj, memory::ObjectMark& mark) {
    auto_mark(obj, mark);
    mark_inliners(obj, mark);
  }

  void Executable::Info::mark_inliners(Object* obj, memory::ObjectMark& mark) {
    Executable* exc = static_cast<Executable*>(obj);
    if(!exc->inliners() || exc->inliners() == (Inliners*)cNil) return;

    Inliners* inl = exc->inliners();
    inl->set_mark();

    // std::cout << "Marking inliners: " << inl->inliners().size() << "\n";

    for(std::vector<CompiledCode*>::iterator i = inl->inliners().begin();
        i != inl->inliners().end();
        ++i) {
      CompiledCode* code = *i;

      if(Object* tmp = mark.call(code)) {
        *i = static_cast<CompiledCode*>(tmp);
        mark.just_set(obj, tmp);
      }
    }
  }

  Inliners::Inliners(STATE, Memory* om) {
    om->add_code_resource(state, this);
  }

  void Inliners::cleanup(STATE, memory::CodeManager* cm) {
    inliners().clear();
  }
}

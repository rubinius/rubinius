#include "vm.hpp"
#include "vm/object_utils.hpp"

#include "gc/gc.hpp"

#include "builtin/class.hpp"
#include "builtin/executable.hpp"
#include "builtin/symbol.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "objectmemory.hpp"

#include "ontology.hpp"

namespace rubinius {

  void Executable::init(STATE) {
    GO(executable).set(ontology::new_class(state, "Executable", G(object), G(rubinius)));
    G(executable)->set_object_type(state, ExecutableType);
  }

  Executable* Executable::allocate(STATE, Object* self) {
    Executable* executable = state->new_object<Executable>(G(executable));
    executable->primitive(state, nil<Symbol>());
    executable->serial(state, Fixnum::from(0));
    executable->inliners_ = 0;
    executable->prim_index_ = -1;

    executable->set_executor(Executable::default_executor);

    if(Class* cls = try_as<Class>(self)) {
      executable->klass(state, cls);
    }

    return executable;
  }

  bool Executable::resolve_primitive(STATE) {
    if(!primitive_->nil_p()) {
      if(Symbol* name = try_as<Symbol>(primitive_)) {
        set_executor(Primitives::resolve_primitive(state, name, &prim_index_));
        return true;
      }
    }

    return false;
  }

  Object* Executable::invoke(STATE, Symbol* name, Module* mod, Object* recv, Array* ary,
                             Object* block, CallFrame* call_frame)
  {
    Dispatch disp(name, mod, this);
    Arguments args(name, recv, 0, 0);
    args.use_array(ary);
    args.set_block(block);

    return execute(state, call_frame, disp.method, disp.module, args);
  }

  Object* Executable::default_executor(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                                       Arguments& args) {
    args.unshift2(state, args.recv(), args.name());
    args.set_recv(exec);

    Dispatch dis(G(sym_call));
    return dis.send(state, call_frame, args);
  }

  void Executable::add_inliner(ObjectMemory* om, CompiledCode* code) {
    if(!inliners_ || inliners_ == (Inliners*)cNil) inliners_ = new Inliners(om);
    inliners_->inliners().push_back(code);

    om->write_barrier(this, code);
  }

  void Executable::clear_inliners(STATE) {
    if(!inliners_ || inliners_ == (Inliners*)cNil) return;
    for(std::list<CompiledCode*>::const_iterator i = inliners_->inliners().begin();
        i != inliners_->inliners().end();
        ++i) {
      (*i)->machine_code()->deoptimize(state, *i, 0);
    }

    inliners_->inliners().clear();
  }

  void Executable::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
    mark_inliners(obj, mark);
  }

  void Executable::Info::mark_inliners(Object* obj, ObjectMark& mark) {
    Executable* exc = reinterpret_cast<Executable*>(obj);
    if(!exc->inliners_ || exc->inliners_ == (Inliners*)cNil) return;

    Inliners* inl = exc->inliners_;
    inl->set_mark();

    // std::cout << "Marking inliners: " << inl->inliners().size() << "\n";

    for(std::list<CompiledCode*>::iterator i = inl->inliners().begin();
        i != inl->inliners().end();
        ++i) {
      CompiledCode* code = *i;

      Object* tmp = mark.call(code);
      if(tmp) {
        assert(kind_of<CompiledCode>(tmp));
        *i = reinterpret_cast<CompiledCode*>(tmp);
        mark.just_set(obj, tmp);
      }
    }
  }

  Inliners::Inliners(ObjectMemory* om) {
    om->add_code_resource(this);
  }

  void Inliners::cleanup(STATE, CodeManager* cm) {
    inliners_.clear();
  }
}

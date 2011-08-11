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

namespace rubinius {

  void Executable::init(STATE) {
    GO(executable).set(state->new_class("Executable", G(object), G(rubinius)));
    G(executable)->set_object_type(state, ExecutableType);
    G(executable)->name(state, state->symbol("Rubinius::Executable"));
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

    Dispatch dis(state->symbol("call"));
    return dis.send(state, call_frame, args);
  }

  void Executable::add_inliner(ObjectMemory* om, CompiledMethod* cm) {
    if(!inliners_ || inliners_ == (Inliners*)Qnil) inliners_ = new Inliners;
    inliners_->inliners().push_back(cm);

    om->write_barrier(this, cm);
  }

  void Executable::clear_inliners(STATE) {
    if(!inliners_ || inliners_ == (Inliners*)Qnil) return;
    for(std::list<CompiledMethod*>::const_iterator i = inliners_->inliners().begin();
        i != inliners_->inliners().end();
        ++i) {
      (*i)->backend_method()->deoptimize(state, *i);
    }

    inliners_->inliners().clear();
  }

  void Executable::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
    mark_inliners(obj, mark);
  }

  void Executable::Info::mark_inliners(Object* obj, ObjectMark& mark) {
    Executable* exc = (Executable*)obj;
    if(!exc->inliners_ || exc->inliners_ == (Inliners*)Qnil) return;

    if(exc->inliners_) {
      Inliners* inl = exc->inliners_;

      // std::cout << "Marking inliners: " << inl->inliners().size() << "\n";

      for(std::list<CompiledMethod*>::iterator i = inl->inliners().begin();
          i != inl->inliners().end();
          ++i) {
        CompiledMethod* cm = *i;

        Object* tmp = mark.call(cm);
        if(tmp) {
          assert(kind_of<CompiledMethod>(tmp));
          *i = (CompiledMethod*)tmp;
          mark.just_set(obj, tmp);
        }
      }
    }
  }

  void Executable::Info::visit(Object* obj, ObjectVisitor& visit) {
    auto_visit(obj, visit);
    visit_inliners(obj, visit);
  }

  void Executable::Info::visit_inliners(Object* obj, ObjectVisitor& visit) {
    Executable* exc = (Executable*)obj;
    if(!exc->inliners_ || exc->inliners_ == (Inliners*)Qnil) return;

    if(exc->inliners_) {
      for(std::list<CompiledMethod*>::iterator i = exc->inliners_->inliners().begin();
          i != exc->inliners_->inliners().end();
          ++i) {
        visit.call(*i);
      }
    }
  }
}

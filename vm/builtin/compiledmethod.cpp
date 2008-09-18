#include "builtin/compiledmethod.hpp"
#include "builtin/methodvisibility.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"

#include "ffi.hpp"
#include "marshal.hpp"
#include "primitives.hpp"
#include "llvm.hpp"
#include "objectmemory.hpp"

namespace rubinius {

  void CompiledMethod::init(STATE) {
    GO(cmethod).set(state->new_class("CompiledMethod", G(executable), CompiledMethod::fields));
    G(cmethod)->set_object_type(state, CMethodType);

    GO(cmethod_vis).set(state->new_class("Visibility", G(object),
        MethodVisibility::fields, G(cmethod)));
    G(cmethod_vis)->set_object_type(state, CMVisibilityType);
  }

  CompiledMethod* CompiledMethod::create(STATE) {
    CompiledMethod* cm = (CompiledMethod*)state->new_object(G(cmethod));
    cm->local_count(state, Fixnum::from(0));
    cm->set_executor(CompiledMethod::default_executor);
    cm->backend_method_ = NULL;

    return cm;
  }

  CompiledMethod* CompiledMethod::generate_tramp(STATE, size_t stack_size) {
    CompiledMethod* cm = CompiledMethod::create(state);

    cm->stack_size(state, Fixnum::from(stack_size));
    cm->required_args(state, Fixnum::from(0));
    cm->total_args(state, cm->required_args());
    cm->name(state, state->symbol("__halt__"));

    cm->iseq(state, InstructionSequence::create(state, 1));
    cm->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_halt));

    StaticScope* ss = StaticScope::create(state);
    ss->module(state, G(object));
    cm->scope(state, ss);

    cm->formalize(state, false);

    return cm;
  }

  VMMethod* CompiledMethod::formalize(STATE, bool ondemand) {
    if(!backend_method_) {
      VMMethod* vmm;
      /* Controls whether we use LLVM out of the gate or not. */
      if(state->config.compile_up_front) {
        if(ondemand) {
          set_executor(VMLLVMMethod::uncompiled_execute);
        } else {
          VMLLVMMethod* llvm = new VMLLVMMethod(state, this);
          llvm->compile(state);
          vmm = llvm;
        }
      } else {
        vmm = new VMMethod(state, this);
      }
      backend_method_ = vmm;

      if(!primitive()->nil_p()) {
        if(SYMBOL name = try_as<Symbol>(primitive())) {
          set_executor(Primitives::resolve_primitive(state, name));
        }
      }
      return vmm;
    }

    return backend_method_;
  }

  void CompiledMethod::specialize(TypeInfo* ti) {
    backend_method_->specialize(ti);
  }

  OBJECT CompiledMethod::compile(STATE) {
    backend_method_ = NULL;
    formalize(state);
    return this;
  }

  bool CompiledMethod::default_executor(STATE, Executable* exec, Task* task, Message& msg) {
    CompiledMethod* cm = as<CompiledMethod>(exec);
    cm->formalize(state, false);
    return cm->execute(state, task, msg);
  }

  void CompiledMethod::post_marshal(STATE) {
    formalize(state); // side-effect, populates backend_method_
  }

  size_t CompiledMethod::number_of_locals() {
    return local_count_->to_native();
  }

  void CompiledMethod::Info::show(STATE, OBJECT self, int level) {
    CompiledMethod* cm = as<CompiledMethod>(self);

    class_header(state, self);
    indent_attribute(++level, "exceptions"); cm->exceptions()->show_simple(state, level);
    indent_attribute(level, "file"); cm->file()->show(state, level);
    indent_attribute(level, "iseq"); cm->iseq()->show(state, level);
    indent_attribute(level, "lines"); cm->lines()->show_simple(state, level);
    indent_attribute(level, "literals"); cm->literals()->show_simple(state, level);
    indent_attribute(level, "local_count"); cm->local_count()->show(state, level);
    indent_attribute(level, "local_names"); cm->local_names()->show_simple(state, level);
    indent_attribute(level, "name"); cm->name()->show(state, level);
    indent_attribute(level, "required_args"); cm->required_args()->show(state, level);
    indent_attribute(level, "scope"); cm->scope()->show(state, level);
    indent_attribute(level, "splat"); cm->splat()->show(state, level);
    indent_attribute(level, "stack_size"); cm->stack_size()->show(state, level);
    indent_attribute(level, "total_args"); cm->total_args()->show(state, level);
    close_body(level);
  }
}

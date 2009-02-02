#include "builtin/compiledmethod.hpp"
#include "builtin/methodvisibility.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"
#include "builtin/machine_method.hpp"
#include "builtin/sendsite.hpp"

#include "ffi.hpp"
#include "marshal.hpp"
#include "primitives.hpp"
#include "llvm.hpp"
#include "objectmemory.hpp"
#include "message.hpp"
#include "vm/object_utils.hpp"

namespace rubinius {

  void CompiledMethod::init(STATE) {
    GO(cmethod).set(state->new_class("CompiledMethod", G(executable)));
    G(cmethod)->set_object_type(state, CompiledMethodType);

    GO(cmethod_vis).set(state->new_class("Visibility", G(object), G(cmethod)));
    G(cmethod_vis)->set_object_type(state, MethodVisibilityType);
  }

  CompiledMethod* CompiledMethod::create(STATE) {
    CompiledMethod* cm = state->new_object<CompiledMethod>(G(cmethod));
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

  int CompiledMethod::start_line(STATE) {
    if(lines_->nil_p()) return -1;
    if(lines_->num_fields() < 1) return -1;
    Tuple* top = as<Tuple>(lines_->at(state, 0));
    return as<Fixnum>(top->at(state, 2))->to_native();
  }

  VMMethod* CompiledMethod::formalize(STATE, bool ondemand) {
    if(!backend_method_) {
      VMMethod* vmm = NULL;
#ifdef ENABLE_LLVM
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
#else
      vmm = new VMMethod(state, this);
#endif
      backend_method_ = vmm;

      if(!primitive()->nil_p()) {
        if(Symbol* name = try_as<Symbol>(primitive())) {
          set_executor(Primitives::resolve_primitive(state, name));
        }
      }
      return vmm;
    }

    return backend_method_;
  }

  void CompiledMethod::specialize(STATE, TypeInfo* ti) {
    backend_method_->specialize(state, ti);
  }

  Object* CompiledMethod::compile(STATE) {
    if(backend_method_ == NULL || backend_method_->run != VMMethod::debugger_interpreter) {
      backend_method_ = NULL;
      formalize(state);
    }
    return this;
  }

  ExecuteStatus CompiledMethod::default_executor(STATE, Task* task, Message& msg) {
    CompiledMethod* cm = as<CompiledMethod>(msg.method);
    cm->formalize(state, false);
    return cm->execute(state, task, msg);
  }

  void CompiledMethod::post_marshal(STATE) {
    formalize(state); // side-effect, populates backend_method_
    // Set the sender attribute of all SendSites in this method to this CM
    Tuple *lit = literals();
    for(std::size_t i = 0; i < lit->num_fields(); i++) {
      SendSite *ss = try_as<SendSite>(lit->at(state, i));
      if(ss != NULL) ss->sender(state, this);
    }
  }

  size_t CompiledMethod::number_of_locals() {
    return local_count_->to_native();
  }

  ExecuteStatus CompiledMethod::activate(STATE, Executable* exec, Task* task, Message& msg) {
    CompiledMethod* meth = as<CompiledMethod>(msg.recv);
    Object* recv = msg.get_argument(0);
    Module* mod  = as<Module>(msg.get_argument(1));
    Array*  args = as<Array>(msg.get_argument(2));
    // Leave msg.block set and pass it through.

    msg.recv = recv;
    msg.method = meth;
    msg.module = mod;
    msg.set_arguments(state, args);
    msg.name = meth->name();
    msg.priv = true;
    msg.method_missing = false;

    // NOTE even when we're activating a method_missing, we don't
    // push the name given, because there really isn't one. So if
    // this is used to call a method_missing, you have to supply all
    // the args.
    return meth->execute(state, task, msg);
  }

  MachineMethod* CompiledMethod::make_machine_method(STATE) {
    if(backend_method_ == 0) {
      formalize(state, false);
    }

    JITCompiler jit;
    jit.compile(state, backend_method_);
    return MachineMethod::create(state, backend_method_, jit);
  }

  bool CompiledMethod::is_rescue_target(STATE, int ip) {
    Tuple* table = exceptions();

    if(table->nil_p()) return false;
    for(size_t i = 0; i < table->num_fields(); i++) {
      Tuple* entry = as<Tuple>(table->at(state, i));
      if(as<Fixnum>(entry->at(state, 2))->to_native() == ip) return true;
    }

    return false;
  }

  Object* CompiledMethod::set_breakpoint(STATE, Fixnum* ip) {
    int i = ip->to_native();
    if(backend_method_ == NULL) formalize(state);
    if(!backend_method_->validate_ip(state, i)) return Primitives::failure();
    backend_method_->run = VMMethod::debugger_interpreter;
    backend_method_->set_breakpoint_flags(state, i, cBreakpoint);
    return ip;
  }

  Object* CompiledMethod::clear_breakpoint(STATE, Fixnum* ip) {
    int i = ip->to_native();
    if(backend_method_ == NULL) return ip;
    if(!backend_method_->validate_ip(state, i)) return Primitives::failure();
    // TODO Should this always reset to the basic interpreter?
    backend_method_->run = VMMethod::interpreter;
    backend_method_->set_breakpoint_flags(state, i,
            backend_method_->get_breakpoint_flags(state, i) & ~cBreakpoint);
    return ip;
  }

  Object* CompiledMethod::is_breakpoint(STATE, Fixnum* ip) {
    int i = ip->to_native();
    if(backend_method_ == NULL) return Qfalse;
    if(!backend_method_->validate_ip(state, i)) return Primitives::failure();
    if(backend_method_->get_breakpoint_flags(state, i) == cBreakpoint)
      return Qtrue;
    return Qfalse;
  }

  void CompiledMethod::Info::show(STATE, Object* self, int level) {
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

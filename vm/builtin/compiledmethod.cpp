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
#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "object_utils.hpp"
#include "vm/object_utils.hpp"
#include "assembler/jit.hpp"

namespace rubinius {

  void CompiledMethod::init(STATE) {
    GO(cmethod).set(state->new_class("CompiledMethod", G(executable), G(rubinius)));
    G(cmethod)->set_object_type(state, CompiledMethodType);
    G(cmethod)->name(state, state->symbol("Rubinius::CompiledMethod"));

    GO(cmethod_vis).set(state->new_class("Visibility", G(object), G(cmethod)));
    G(cmethod_vis)->set_object_type(state, MethodVisibilityType);
    G(cmethod_vis)->name(state, state->symbol("Rubinius::CompiledMethod::Visibility"));
  }

  CompiledMethod* CompiledMethod::create(STATE) {
    CompiledMethod* cm = state->new_object<CompiledMethod>(G(cmethod));
    cm->local_count(state, Fixnum::from(0));
    cm->set_executor(CompiledMethod::default_executor);
    cm->backend_method_ = NULL;

    return cm;
  }

  int CompiledMethod::start_line(STATE) {
    if(lines_->nil_p()) return -1;
    if(lines_->num_fields() < 2) return -1;
    // This is fixed as one because entry 0 is always ip = 0 and
    // 1 is the first line
    return as<Fixnum>(lines_->at(state, 1))->to_native();
  }

  int CompiledMethod::line(STATE, int ip) {
    if(lines_->nil_p()) return -3;

    size_t fin = lines_->num_fields() - 2;
    for(size_t i = 0; i < fin; i += 2) {
      Fixnum* start_ip = as<Fixnum>(lines_->at(state, i));
      Fixnum* end_ip   = as<Fixnum>(lines_->at(state, i+2));

      if(start_ip->to_native() <= ip && end_ip->to_native() > ip) {
        return as<Fixnum>(lines_->at(state, i+1))->to_native();
      }
    }

    return as<Fixnum>(lines_->at(state, fin+1))->to_native();
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

      resolve_primitive(state);
      return vmm;
    }

    return backend_method_;
  }

  Object* CompiledMethod::primitive_failed(STATE, CallFrame* call_frame, Dispatch& msg,
                                           Arguments& args) {
    if(try_as<CompiledMethod>(msg.method)) {
      return VMMethod::execute(state, call_frame, msg, args);
    }

    // TODO fix me to raise an exception
    assert(0);
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

  Object* CompiledMethod::default_executor(STATE, CallFrame* call_frame, Dispatch& msg,
                                           Arguments& args) {
    CompiledMethod* cm = as<CompiledMethod>(msg.method);
    cm->formalize(state, false);
    // Refactor
    cm->backend_method_->find_super_instructions();
    return cm->execute(state, call_frame, msg, args);
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

  Object* CompiledMethod::activate(STATE, Executable* exec, CallFrame* call_frame, Dispatch& msg,
                                   Arguments& args) {
    CompiledMethod* meth = as<CompiledMethod>(args.recv());
    Object* recv = args.get_argument(0);
    Module* mod  = as<Module>(args.get_argument(1));
    Array*  ary = as<Array>(args.get_argument(2));

    Dispatch disp(meth->name(), mod, meth);
    Arguments new_args(recv, 0, 0);

    new_args.use_array(ary);
    new_args.set_block(args.block());

    // NOTE even when we're activating a method_missing, we don't
    // push the name given, because there really isn't one. So if
    // this is used to call a method_missing, you have to supply all
    // the args.
    return meth->execute(state, call_frame, disp, new_args);
  }

  MachineMethod* CompiledMethod::make_machine_method(STATE) {
    if(backend_method_ == NULL) {
      formalize(state, false);
    }

    return MachineMethod::create(state, backend_method_);
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

  CompiledMethod* CompiledMethod::of_sender(STATE, CallFrame* calling_environment) {
    CallFrame* caller = static_cast<CallFrame*>(calling_environment->previous);
    if(caller) {
      if(caller->cm) {
        return caller->cm;
      }
    }

    return (CompiledMethod*)Qnil;
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

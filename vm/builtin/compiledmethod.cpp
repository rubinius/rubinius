#include "gc/gc.hpp"

#include "builtin/compiledmethod.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/iseq.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/string.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/call_unit.hpp"
#include "builtin/cache.hpp"

#include "ffi.hpp"
#include "marshal.hpp"
#include "primitives.hpp"
#include "objectmemory.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "object_utils.hpp"
#include "vm/object_utils.hpp"
#include "configuration.hpp"

#include "inline_cache.hpp"
#include "bytecode_verification.hpp"
#include "instruments/timing.hpp"

#ifdef ENABLE_LLVM
#include "llvm/jit.hpp"
#include "llvm/jit_compiler.hpp"
#include "llvm/jit_runtime.hpp"
#endif

namespace rubinius {

  void CompiledMethod::init(STATE) {
    GO(cmethod).set(state->new_class("CompiledMethod", G(executable), G(rubinius)));
    G(cmethod)->set_object_type(state, CompiledMethodType);
    G(cmethod)->name(state, state->symbol("Rubinius::CompiledMethod"));
  }

  CompiledMethod* CompiledMethod::create(STATE) {
    CompiledMethod* cm = state->new_object<CompiledMethod>(G(cmethod));
    cm->local_count(state, Fixnum::from(0));
    cm->set_executor(CompiledMethod::default_executor);
    cm->backend_method_ = NULL;
    cm->inliners_ = NULL;
    cm->prim_index_ = -1;

#ifdef ENABLE_LLVM
    cm->jit_data_ = NULL;
#endif

    return cm;
  }

  CompiledMethod* CompiledMethod::dup_cm(STATE) {
    CompiledMethod* cm = CompiledMethod::create(state);
    cm->copy_object(state, this);

    cm->set_executor(CompiledMethod::default_executor);
    cm->jit_data_ = NULL;
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

  int CompiledMethod::start_line() {
    if(lines_->nil_p()) return -1;
    if(lines_->num_fields() < 2) return -1;
    // This is fixed as one because entry 0 is always ip = 0 and
    // 1 is the first line
    return as<Fixnum>(lines_->at(1))->to_native();
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

  VMMethod* CompiledMethod::internalize(STATE, const char** reason, int* ip) {
    VMMethod* vmm = backend_method_;
    atomic::memory_barrier();
    if(!vmm) {
      hard_lock(state);

      vmm = backend_method_;
      if(!vmm) {
        {
          BytecodeVerification bv(this);
          if(!bv.verify(state)) {
            if(reason) *reason = bv.failure_reason();
            if(ip) *ip = bv.failure_ip();
            std::cerr << "Error validating bytecode: " << bv.failure_reason() << "\n";
            return 0;
          }
        }

        vmm = new VMMethod(state, this);

        if(!resolve_primitive(state)) {
          vmm->setup_argument_handler(this);
        }

        // We need to have an explicit memory barrier here, because we need to
        // be sure that vmm is completely initialized before it's set.
        // Otherwise another thread might see a partially initialized
        // VMMethod.
        atomic::memory_barrier();
        backend_method_ = vmm;
      }

      hard_unlock(state);
    }
    return vmm;
  }

  Object* CompiledMethod::primitive_failed(STATE, CallFrame* call_frame,
              Executable* exec, Module* mod, Arguments& args)
  {
    if(try_as<CompiledMethod>(exec)) {
      return VMMethod::execute(state, call_frame, exec, mod, args);
    }

    // TODO fix me to raise an exception
    assert(0);
    return Qnil;
  }

  void CompiledMethod::specialize(STATE, TypeInfo* ti) {
    backend_method_->specialize(state, this, ti);
  }

  Object* CompiledMethod::default_executor(STATE, CallFrame* call_frame,
                          Executable* exec, Module* mod, Arguments& args)
  {
    LockableScopedLock lg(state, &state->shared, __FILE__, __LINE__);

    CompiledMethod* cm = as<CompiledMethod>(exec);
    if(cm->execute == default_executor) {
      const char* reason = 0;
      int ip = -1;

      if(!cm->internalize(state, &reason, &ip)) {
        Exception::bytecode_error(state, call_frame, cm, ip, reason);
        return 0;
      }
    }

    lg.unlock();

    return cm->execute(state, call_frame, exec, mod, args);
  }

  void CompiledMethod::post_marshal(STATE) {
  }

  size_t CompiledMethod::number_of_locals() {
    return local_count_->to_native();
  }

  String* CompiledMethod::full_name(STATE) {
    return name_->to_str(state);
  }

  Object* CompiledMethod::set_breakpoint(STATE, Fixnum* ip, Object* bp) {
    int i = ip->to_native();
    if(backend_method_ == NULL) {
      if(!internalize(state)) return Primitives::failure();
    }
    if(!backend_method_->validate_ip(state, i)) return Primitives::failure();

    if(breakpoints_->nil_p()) {
      breakpoints(state, LookupTable::create(state));
    }

    breakpoints_->store(state, ip, bp);
    backend_method_->debugging = 1;
    backend_method_->run = VMMethod::debugger_interpreter;
    return ip;
  }

  Object* CompiledMethod::clear_breakpoint(STATE, Fixnum* ip) {
    int i = ip->to_native();
    if(backend_method_ == NULL) return ip;
    if(!backend_method_->validate_ip(state, i)) return Primitives::failure();

    bool removed = false;
    if(!breakpoints_->nil_p()) {
      breakpoints_->remove(state, ip, &removed);

      // No more breakpoints, switch back to the normal interpreter
      if(breakpoints_->entries()->to_native() == 0) {
        backend_method_->debugging = 0;
        backend_method_->run = VMMethod::interpreter;
      }
    }

    return removed ? Qtrue : Qfalse;
  }

  Object* CompiledMethod::is_breakpoint(STATE, Fixnum* ip) {
    int i = ip->to_native();
    if(backend_method_ == NULL) return Qfalse;
    if(!backend_method_->validate_ip(state, i)) return Primitives::failure();
    if(breakpoints_->nil_p()) return Qfalse;

    bool found = false;
    breakpoints_->fetch(state, ip, &found);

    if(found) return Qtrue;
    return Qfalse;
  }

  CompiledMethod* CompiledMethod::of_sender(STATE, CallFrame* calling_environment) {
    CallFrame* caller = static_cast<CallFrame*>(calling_environment->previous);
    if(caller) {
      if(caller->cm) {
        return caller->cm;
      }
    }

    return nil<CompiledMethod>();
  }

  CompiledMethod* CompiledMethod::current(STATE, CallFrame* calling_environment) {
    return calling_environment->cm;
  }

  void CompiledMethod::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);

    mark_inliners(obj, mark);

    CompiledMethod* cm = as<CompiledMethod>(obj);
    if(!cm->backend_method_) return;

    VMMethod* vmm = cm->backend_method_;
    vmm->set_mark();

    Object* tmp;

#ifdef ENABLE_LLVM
    if(cm->jit_data()) {
      cm->jit_data()->set_mark();
      cm->jit_data()->mark_all(cm, mark);
    }
#endif

    for(size_t i = 0; i < vmm->inline_cache_count(); i++) {
      InlineCache* cache = &vmm->caches[i];

      MethodCacheEntry* mce = cache->cache_;
      if(mce) {
        tmp = mark.call(mce);
        if(tmp) {
          cache->cache_ = (MethodCacheEntry*)tmp;
          mark.just_set(obj, tmp);
        }
      }

      if(cache->call_unit_) {
        tmp = mark.call(cache->call_unit_);
        if(tmp) {
          cache->call_unit_ = (CallUnit*)tmp;
          mark.just_set(obj, tmp);
        }
      }

      for(int i = 0; i < cTrackedICHits; i++) {
        Module* mod = cache->seen_classes_[i].klass();
        if(mod) {
          tmp = mark.call(mod);
          if(tmp) {
            cache->seen_classes_[i].set_klass(force_as<Class>(tmp));
            mark.just_set(obj, tmp);
          }
        }
      }
    }
  }

  void CompiledMethod::Info::visit(Object* obj, ObjectVisitor& visit) {
    auto_visit(obj, visit);

    visit_inliners(obj, visit);

    CompiledMethod* cm = as<CompiledMethod>(obj);
    if(!cm->backend_method_) return;

    VMMethod* vmm = cm->backend_method_;

#ifdef ENABLE_LLVM
    if(cm->jit_data()) {
      cm->jit_data()->visit_all(visit);
    }
#endif

    for(size_t i = 0; i < vmm->inline_cache_count(); i++) {
      InlineCache* cache = &vmm->caches[i];

      MethodCacheEntry* mce = cache->cache_;
      if(mce) visit.call(mce);

      for(int i = 0; i < cTrackedICHits; i++) {
        Module* mod = cache->seen_classes_[i].klass();
        if(mod) visit.call(mod);
      }
    }
  }

  void CompiledMethod::Info::show(STATE, Object* self, int level) {
    CompiledMethod* cm = as<CompiledMethod>(self);

    class_header(state, self);
    indent_attribute(++level, "file"); cm->file()->show(state, level);
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

    indent_attribute(level, "internalized");
    if(!cm->backend_method_) {
      std::cout << "no\n";
    } else {
      std::cout << "yes\n";

#ifdef ENABLE_LLVM
      if(cm->backend_method_->llvm_function()) {
        llvm::outs() << "<LLVM>\n"
                     << *cm->backend_method_->llvm_function()
                     << "</LLVM>\n<MachineCode>\n";
        LLVMState::show_machine_code(
            cm->backend_method_->jitted_impl(),
            cm->backend_method_->jitted_bytes());
        llvm::outs() << "</MachineCode>\n";
      }
#endif
    }

    close_body(level);
  }
}

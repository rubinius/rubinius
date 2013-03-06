#include "gc/gc.hpp"

#include "builtin/compiledcode.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/iseq.hpp"
#include "builtin/constantscope.hpp"
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

#include "on_stack.hpp"

#include "ontology.hpp"

#ifdef ENABLE_LLVM
#include "llvm/state.hpp"
#include "llvm/jit_compiler.hpp"
#include "llvm/jit_runtime.hpp"
#endif

namespace rubinius {

  void CompiledCode::init(STATE) {
    GO(compiled_code).set(ontology::new_class(state,
                      "CompiledCode", G(executable), G(rubinius)));
    G(compiled_code)->set_object_type(state, CompiledCodeType);
  }

  CompiledCode* CompiledCode::create(STATE) {
    CompiledCode* code = state->new_object<CompiledCode>(G(compiled_code));
    code->local_count(state, Fixnum::from(0));
    code->set_executor(CompiledCode::default_executor);
    code->machine_code_ = NULL;
    code->inliners_ = 0;
    code->prim_index_ = -1;

#ifdef ENABLE_LLVM
    code->jit_data_ = NULL;
#endif

    return code;
  }

  CompiledCode* CompiledCode::dup(STATE) {
    CompiledCode* code = state->new_object_dirty<CompiledCode>(G(compiled_code));
    code->copy_object(state, this);

    code->set_executor(CompiledCode::default_executor);
    code->jit_data_ = NULL;
    code->machine_code_ = NULL;

    return code;
  }

  int CompiledCode::start_line(STATE) {
    return start_line();
  }

  int CompiledCode::start_line() {
    if(lines_->nil_p()) return -1;
    if(lines_->num_fields() < 2) return -1;
    // This is fixed as one because entry 0 is always ip = 0 and
    // 1 is the first line
    return as<Fixnum>(lines_->at(1))->to_native();
  }

  int CompiledCode::line(STATE, int ip) {
    return line(ip);
  }

  int CompiledCode::line(int ip) {
    if(lines_->nil_p()) return -3;

    native_int fin = lines_->num_fields() - 2;
    for(native_int i = 0; i < fin; i += 2) {
      Fixnum* start_ip = as<Fixnum>(lines_->at(i));
      Fixnum* end_ip   = as<Fixnum>(lines_->at(i+2));

      if(start_ip->to_native() <= ip && end_ip->to_native() > ip) {
        return as<Fixnum>(lines_->at(i+1))->to_native();
      }
    }

    return as<Fixnum>(lines_->at(fin+1))->to_native();
  }

  MachineCode* CompiledCode::internalize(STATE, GCToken gct,
                                        const char** reason, int* ip)
  {
    MachineCode* mcode = machine_code_;

    atomic::memory_barrier();

    if(mcode) return mcode;

    CompiledCode* self = this;
    OnStack<1> os(state, self);

    self->hard_lock(state, gct);

    mcode = self->machine_code_;
    if(!mcode) {
      {
        BytecodeVerification bv(self);
        if(!bv.verify(state)) {
          if(reason) *reason = bv.failure_reason();
          if(ip) *ip = bv.failure_ip();
          std::cerr << "Error validating bytecode: " << bv.failure_reason() << "\n";
          return 0;
        }
      }

      mcode = new MachineCode(state, self);

      if(self->resolve_primitive(state)) {
        mcode->fallback = execute;
      } else {
        mcode->setup_argument_handler();
      }

      // We need to have an explicit memory barrier here, because we need to
      // be sure that mcode is completely initialized before it's set.
      // Otherwise another thread might see a partially initialized
      // MachineCode.
      atomic::write(&self->machine_code_, mcode);

      set_executor(mcode->fallback);
    }

    self->hard_unlock(state, gct);
    return mcode;
  }

  Object* CompiledCode::primitive_failed(STATE, CallFrame* call_frame,
              Executable* exec, Module* mod, Arguments& args)
  {
    CompiledCode* code = as<CompiledCode>(exec);

    Class* cls = args.recv()->lookup_begin(state);
    int id = cls->class_id();

    MachineCode* v = code->machine_code();

    executor target = v->unspecialized;

    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      int c_id = v->specializations[i].class_id;
      executor x = v->specializations[i].execute;

      if(c_id == id && x != 0) {
        target = x;
        break;
      }
    }

    if(target) {
      return target(state, call_frame, exec, mod, args);
    } else {
      return MachineCode::execute(state, call_frame, exec, mod, args);
    }
  }

  void CompiledCode::specialize(STATE, TypeInfo* ti) {
    machine_code_->specialize(state, this, ti);
  }

  Object* CompiledCode::default_executor(STATE, CallFrame* call_frame,
                          Executable* exec, Module* mod, Arguments& args)
  {
    LockableScopedLock lg(state, &state->shared(), __FILE__, __LINE__);

    CompiledCode* code = as<CompiledCode>(exec);
    if(code->execute == default_executor) {
      const char* reason = 0;
      int ip = -1;

      OnStack<4> os(state, code, exec, mod, args.argument_container_location());
      GCTokenImpl gct;

      if(!code->internalize(state, gct, &reason, &ip)) {
        Exception::bytecode_error(state, call_frame, code, ip, reason);
        return 0;
      }
    }

    lg.unlock();

    return code->execute(state, call_frame, exec, mod, args);
  }

  Object* CompiledCode::specialized_executor(STATE, CallFrame* call_frame,
                          Executable* exec, Module* mod, Arguments& args)
  {
    CompiledCode* code = as<CompiledCode>(exec);

    Class* cls = args.recv()->lookup_begin(state);
    int id = cls->class_id();

    MachineCode* v = code->machine_code();

    executor target = v->unspecialized;

    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      int c_id = v->specializations[i].class_id;
      executor x = v->specializations[i].execute;

      if(c_id == id && x != 0) {
        target = x;
        break;
      }
    }

    // This is a bug. We should not have this setup if there are no
    // specializations. FIX THIS BUG!
    if(!target) target = v->fallback;

    return target(state, call_frame, exec, mod, args);
  }

  bool CompiledCode::can_specialize_p() {
    if(!machine_code_) rubinius::bug("specializing with no backend");

    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      if(machine_code_->specializations[i].class_id == 0) return true;
    }

    return false;
  }

  void CompiledCode::set_unspecialized(executor exec, jit::RuntimeDataHolder* rd) {
    if(!machine_code_) rubinius::bug("specializing with no backend");

    machine_code_->set_execute_status(MachineCode::eJIT);

    jit_data_ = rd;
    machine_code_->unspecialized = exec;

    // See if we can also just make this the normal execute
    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      if(machine_code_->specializations[i].class_id > 0) return;
    }

    if(primitive()->nil_p()) {
      execute = exec;
    }
  }

  void CompiledCode::add_specialized(int spec_id, executor exec,
                                       jit::RuntimeDataHolder* rd)
  {
    if(!machine_code_) rubinius::bug("specializing with no backend");

    MachineCode* v = machine_code_;

    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      int id = v->specializations[i].class_id;
      if(id == 0 || id == spec_id) {
        v->specializations[i].class_id = spec_id;
        v->specializations[i].execute = exec;
        v->specializations[i].jit_data = rd;

        v->set_execute_status(MachineCode::eJIT);
        if(primitive()->nil_p()) {
          execute = specialized_executor;
        }
        return;
      }
    }

    // No room for the specialization, this is bad.
    std::cerr << "No room for specialization!\n";
  }

  executor CompiledCode::find_specialized(int spec_id) {
    MachineCode* v = machine_code_;

    if(!v) return 0;

    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      if(v->specializations[i].class_id == spec_id) {
        return v->specializations[i].execute;
      }
    }

    return 0;
  }

  void CompiledCode::post_marshal(STATE) {
  }

  size_t CompiledCode::number_of_locals() {
    return local_count_->to_native();
  }

  String* CompiledCode::full_name(STATE) {
    return name_->to_str(state);
  }

  bool CompiledCode::kernel_method(STATE) {
    std::string s = file()->cpp_str(state);
    if(s.size() >= 7 && strncmp(s.data(), "kernel/", 7) == 0) return true;
    return false;
  }

  void CompiledCode::set_interpreter(executor interp) {
    set_executor(interp);
    machine_code_->fallback = interp;
  }

  Object* CompiledCode::set_breakpoint(STATE, GCToken gct, Fixnum* ip, Object* bp) {
    CompiledCode* self = this;
    OnStack<3> os(state, self, ip, bp);

    int i = ip->to_native();
    if(self->machine_code_ == NULL) {
      if(!self->internalize(state, gct)) return Primitives::failure();
    }

    if(!self->machine_code_->validate_ip(state, i)) return Primitives::failure();

    if(self->breakpoints_->nil_p()) {
      self->breakpoints(state, LookupTable::create(state));
    }

    self->breakpoints_->store(state, ip, bp);
    self->machine_code_->debugging = 1;
    self->machine_code_->run = MachineCode::debugger_interpreter;

    return ip;
  }

  Object* CompiledCode::clear_breakpoint(STATE, Fixnum* ip) {
    int i = ip->to_native();
    if(machine_code_ == NULL) return ip;
    if(!machine_code_->validate_ip(state, i)) return Primitives::failure();

    bool removed = false;
    if(!breakpoints_->nil_p()) {
      breakpoints_->remove(state, ip, &removed);

      // No more breakpoints, switch back to the normal interpreter
      if(breakpoints_->entries()->to_native() == 0) {
        machine_code_->debugging = 0;
        machine_code_->run = MachineCode::interpreter;
      }
    }

    return removed ? cTrue : cFalse;
  }

  Object* CompiledCode::is_breakpoint(STATE, Fixnum* ip) {
    int i = ip->to_native();
    if(machine_code_ == NULL) return cFalse;
    if(!machine_code_->validate_ip(state, i)) return Primitives::failure();
    if(breakpoints_->nil_p()) return cFalse;

    bool found = false;
    breakpoints_->fetch(state, ip, &found);

    if(found) return cTrue;
    return cFalse;
  }

  CompiledCode* CompiledCode::of_sender(STATE, CallFrame* calling_environment) {
    CallFrame* caller = calling_environment->previous;
    if(caller) {
      if(caller->compiled_code) {
        return caller->compiled_code;
      }
    }

    return nil<CompiledCode>();
  }

  CompiledCode* CompiledCode::current(STATE, CallFrame* calling_environment) {
    return calling_environment->compiled_code;
  }

  void CompiledCode::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);

    mark_inliners(obj, mark);

    CompiledCode* code = as<CompiledCode>(obj);
    if(!code->machine_code_) return;

    MachineCode* mcode = code->machine_code_;
    mcode->set_mark();

    Object* tmp;

#ifdef ENABLE_LLVM
    if(code->jit_data()) {
      code->jit_data()->set_mark();
      code->jit_data()->mark_all(code, mark);
    }


    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      if(mcode->specializations[i].jit_data) {
        mcode->specializations[i].jit_data->set_mark();
        mcode->specializations[i].jit_data->mark_all(code, mark);
      }
    }
#endif

    for(size_t i = 0; i < mcode->inline_cache_count(); i++) {
      InlineCache* cache = &mcode->caches[i];

      for(int j = 0; j < cTrackedICHits; ++j) {
        MethodCacheEntry* mce = cache->cache_[j].entry();
        if(mce) {
          tmp = mark.call(mce);
          if(tmp) {
            cache->cache_[j].assign(static_cast<MethodCacheEntry*>(tmp));
            mark.just_set(obj, tmp);
          }
        }
      }

      if(cache->call_unit_) {
        tmp = mark.call(cache->call_unit_);
        if(tmp) {
          cache->call_unit_ = static_cast<CallUnit*>(tmp);
          mark.just_set(obj, tmp);
        }
      }
    }
  }

  void CompiledCode::Info::show(STATE, Object* self, int level) {
    CompiledCode* code = as<CompiledCode>(self);

    class_header(state, self);
    indent_attribute(++level, "file"); code->file()->show(state, level);
    indent_attribute(level, "iseq"); code->iseq()->show(state, level);
    indent_attribute(level, "lines"); code->lines()->show_simple(state, level);
    indent_attribute(level, "literals"); code->literals()->show_simple(state, level);
    indent_attribute(level, "local_count"); code->local_count()->show(state, level);
    indent_attribute(level, "local_names"); code->local_names()->show_simple(state, level);
    indent_attribute(level, "name"); code->name()->show(state, level);
    indent_attribute(level, "required_args"); code->required_args()->show(state, level);
    indent_attribute(level, "scope"); code->scope()->show(state, level);
    indent_attribute(level, "splat"); code->splat()->show(state, level);
    indent_attribute(level, "stack_size"); code->stack_size()->show(state, level);
    indent_attribute(level, "total_args"); code->total_args()->show(state, level);

    indent_attribute(level, "internalized");
    if(!code->machine_code_) {
      std::cout << "no\n";
    } else {
      std::cout << "yes\n";

#ifdef ENABLE_LLVM
      MachineCode* v = code->machine_code();

      for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
        if(!v->specializations[i].jit_data) continue;
        llvm::outs() << "<MachineCode>\n";

        LLVMState::show_machine_code(
            v->specializations[i].jit_data->native_func(),
            v->specializations[i].jit_data->native_size());
        llvm::outs() << "</MachineCode>\n";
      }
#endif
    }

    close_body(level);
  }
}

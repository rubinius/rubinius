#include "arguments.hpp"
#include "bytecode_verifier.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "object_utils.hpp"
#include "memory.hpp"
#include "machine_code.hpp"
#include "on_stack.hpp"
#include "logger.hpp"

#include "class/call_site.hpp"
#include "class/class.hpp"
#include "class/compiled_code.hpp"
#include "class/lexical_scope.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/iseq.hpp"
#include "class/lookup_table.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"

#include "memory/object_mark.hpp"

#include "instruments/timing.hpp"

#include <ostream>

namespace rubinius {

  void CompiledCode::bootstrap(STATE) {
    GO(compiled_code).set(state->memory()->new_class<Class, CompiledCode>(
          state, G(executable), G(rubinius), "CompiledCode"));
  }

  void CompiledCode::finalize(STATE, CompiledCode* code) {
    if(MachineCode* machine_code = code->machine_code()) {
      machine_code->finalize(state);
    }
  }

  CompiledCode* CompiledCode::create(STATE) {
    return CompiledCode::allocate(state, G(compiled_code));
  }

  CompiledCode* CompiledCode::allocate(STATE, Object* self) {
    CompiledCode* code =
      state->memory()->new_object<CompiledCode>(state, as<Class>(self));

    state->memory()->native_finalizer(state, code,
        (memory::FinalizerFunction)&CompiledCode::finalize);

    return code;
  }

  CompiledCode* CompiledCode::dup(STATE) {
    CompiledCode* code = allocate(state, G(compiled_code));

    code->copy_object(state, this);
    code->set_executor(CompiledCode::default_executor);
    code->machine_code(NULL);

    return code;
  }

  Tuple* CompiledCode::call_sites(STATE) {
    CompiledCode* self = this;
    OnStack<1> os(state, self);

    if(self->machine_code() == NULL) {
      if(!self->internalize(state)) return force_as<Tuple>(Primitives::failure());
    }
    MachineCode* mcode = self->machine_code();
    return mcode->call_sites(state);
  }

  Tuple* CompiledCode::constant_caches(STATE) {
    CompiledCode* self = this;
    OnStack<1> os(state, self);

    if(self->machine_code() == NULL) {
      if(!self->internalize(state)) return force_as<Tuple>(Primitives::failure());
    }
    MachineCode* mcode = self->machine_code();
    return mcode->constant_caches(state);
  }


  int CompiledCode::start_line(STATE) {
    return start_line();
  }

  int CompiledCode::start_line() {
    if(lines()->nil_p()) return -1;
    if(lines()->num_fields() < 2) return -1;
    // This is fixed as one because entry 0 is always ip = 0 and
    // 1 is the first line
    return as<Fixnum>(lines()->at(1))->to_native();
  }

  int CompiledCode::line(STATE, int ip) {
    return line(ip);
  }

  int CompiledCode::line(int ip) {
    if(lines()->nil_p()) return -3;

    native_int fin = lines()->num_fields() - 2;
    for(native_int i = 0; i < fin; i += 2) {
      Fixnum* start_ip = as<Fixnum>(lines()->at(i));
      Fixnum* end_ip   = as<Fixnum>(lines()->at(i+2));

      if(start_ip->to_native() <= ip && end_ip->to_native() > ip) {
        return as<Fixnum>(lines()->at(i+1))->to_native();
      }
    }

    return as<Fixnum>(lines()->at(fin+1))->to_native();
  }

  MachineCode* CompiledCode::internalize(STATE) {
    timer::StopWatch<timer::microseconds> timer(
        state->vm()->metrics().machine.bytecode_internalizer_us);

    atomic::memory_barrier();

    MachineCode* mcode = machine_code();

    if(mcode) return mcode;

    {
      BytecodeVerifier bytecode_verifier(this);
      bytecode_verifier.verify(state);
    }

    mcode = new MachineCode(state, this);

    if(resolve_primitive(state)) {
      mcode->fallback = execute;
    } else {
      mcode->setup_argument_handler();
    }

    /* There is a race here because another Thread may have run this
     * CompiledCode instance and internalized it. We attempt to store our
     * version assuming that we are the only ones to do so and throw away our
     * work if someone else has beat us to it.
     */
    MachineCode** mcode_ptr = &_machine_code_;
    if(atomic::compare_and_swap(reinterpret_cast<void**>(mcode_ptr), 0, mcode)) {
      set_executor(mcode->fallback);
      return mcode;
    } else {
      return machine_code();
    }
  }

  Object* CompiledCode::primitive_failed(STATE,
              Executable* exec, Module* mod, Arguments& args)
  {
    CompiledCode* code = as<CompiledCode>(exec);

    Class* cls = args.recv()->direct_class(state);
    uint64_t class_data = cls->data_raw();

    MachineCode* v = code->machine_code();

    executor target = v->unspecialized;

    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      uint64_t c_id = v->specializations[i].class_data.raw;
      executor x = v->specializations[i].execute;

      if(c_id == class_data && x != 0) {
        target = x;
        break;
      }
    }

    if(target) {
      return target(state, exec, mod, args);
    } else {
      return MachineCode::execute(state, exec, mod, args);
    }
  }

  void CompiledCode::specialize(STATE, TypeInfo* ti) {
    machine_code()->specialize(state, this, ti);
  }

  Object* CompiledCode::default_executor(STATE,
                          Executable* exec, Module* mod, Arguments& args)
  {
    CompiledCode* code = as<CompiledCode>(exec);

    if(code->execute == default_executor) {
      if(!code->internalize(state)) return 0;
    }

    return code->execute(state, exec, mod, args);
  }

  Object* CompiledCode::specialized_executor(STATE,
                          Executable* exec, Module* mod, Arguments& args)
  {
    CompiledCode* code = as<CompiledCode>(exec);

    Class* cls = args.recv()->direct_class(state);
    uint64_t class_data = cls->data_raw();

    MachineCode* v = code->machine_code();

    executor target = v->unspecialized;

    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      uint64_t c_id = v->specializations[i].class_data.raw;
      executor x = v->specializations[i].execute;

      if(c_id == class_data && x != 0) {
        target = x;
        break;
      }
    }

    // This is a bug. We should not have this setup if there are no
    // specializations. FIX THIS BUG!
    if(!target) target = v->fallback;

    return target(state, exec, mod, args);
  }

  bool CompiledCode::can_specialize_p() {
    if(!machine_code()) rubinius::bug("specializing with no backend");

    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      if(machine_code()->specializations[i].class_data.raw == 0) return true;
    }

    return false;
  }

  void CompiledCode::add_specialized(STATE,
      uint32_t class_id, uint32_t serial_id, executor exec)
  {
    if(!machine_code()) {
      logger::error("specializing with no backend");
      return;
    }

    MachineCode* v = machine_code();

    int i;

    for(i = 0; i < MachineCode::cMaxSpecializations; i++) {
      uint32_t id = v->specializations[i].class_data.f.class_id;

      if(id == 0 || id == class_id) break;
    }

    /* We have fixed space for specializations. If we exceed this, overwrite
     * the first one. This should be converted to some sort of LRU cache.
     */
    if(i == MachineCode::cMaxSpecializations) {
      std::ostringstream msg;

      msg << "Specialization space exceeded for " <<
        machine_code()->name()->cpp_str(state);
      logger::warn(msg.str().c_str());

      i = 0;
    }

    v->specializations[i].class_data.f.class_id = class_id;
    v->specializations[i].class_data.f.serial_id = serial_id;
    v->specializations[i].execute = exec;

    v->set_execute_status(MachineCode::eJIT);
    if(primitive()->nil_p()) {
      execute = specialized_executor;
    }
  }

  executor CompiledCode::find_specialized(Class* cls) {
    MachineCode* v = machine_code();

    if(!v) return 0;

    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      if(v->specializations[i].class_data.raw == cls->data_raw()) {
        return v->specializations[i].execute;
      }
    }

    return 0;
  }

  void CompiledCode::post_marshal(STATE) {
  }

  size_t CompiledCode::number_of_locals() {
    return local_count()->to_native();
  }

  String* CompiledCode::full_name(STATE) {
    return name()->to_str(state);
  }

  bool CompiledCode::core_method(STATE) {
    std::string& s = file()->cpp_str(state);
    if(s.compare(0, 5, "core/", 5) == 0) return true;
    return false;
  }

  Object* CompiledCode::set_breakpoint(STATE, Fixnum* ip, Object* bp) {
    CompiledCode* self = this;
    OnStack<3> os(state, self, ip, bp);

    int i = ip->to_native();
    if(self->machine_code() == NULL) {
      if(!self->internalize(state)) return Primitives::failure();
    }

    if(!self->machine_code()->validate_ip(state, i)) return Primitives::failure();

    if(self->breakpoints()->nil_p()) {
      self->breakpoints(state, LookupTable::create(state));
    }

    self->breakpoints()->store(state, ip, bp);
    self->machine_code()->debugging = 1;
    self->machine_code()->run = MachineCode::debugger_interpreter;

    return ip;
  }

  Object* CompiledCode::clear_breakpoint(STATE, Fixnum* ip) {
    int i = ip->to_native();
    if(machine_code() == NULL) return ip;
    if(!machine_code()->validate_ip(state, i)) return Primitives::failure();

    bool removed = false;
    if(!breakpoints()->nil_p()) {
      breakpoints()->remove(state, ip, &removed);

      // No more breakpoints, switch back to the normal interpreter
      if(breakpoints()->entries()->to_native() == 0) {
        machine_code()->debugging = 0;
        machine_code()->run = MachineCode::interpreter;
      }
    }

    return RBOOL(removed);
  }

  Object* CompiledCode::is_breakpoint(STATE, Fixnum* ip) {
    int i = ip->to_native();
    if(machine_code() == NULL) return cFalse;
    if(!machine_code()->validate_ip(state, i)) return Primitives::failure();
    if(breakpoints()->nil_p()) return cFalse;

    bool found = false;
    breakpoints()->fetch(state, ip, &found);

    return RBOOL(found);
  }

  CompiledCode* CompiledCode::of_sender(STATE) {
    if(CallFrame* frame = state->vm()->get_ruby_frame(1)) {
      if(frame->compiled_code) {
        return frame->compiled_code;
      }
    }

    return nil<CompiledCode>();
  }

  CompiledCode* CompiledCode::current(STATE) {
    return state->vm()->call_frame()->compiled_code;
  }

  Object* CompiledCode::jitted_p(STATE) {
    return RBOOL(machine_code() && machine_code()->jitted_p());
  }

  Fixnum* CompiledCode::sample_count(STATE) {
    return Fixnum::from(machine_code()->sample_count);
  }

  Object* CompiledCode::execute_script(STATE) {
    state->thread_state()->clear();

    Arguments args(state->symbol("script"), G(main));

    scope(state, LexicalScope::create(state));
    scope()->module(state, G(object));

    execute(state, this, G(object), args);

    /* We have to assume that this can fail before the Kernel is able to
     * handle that failure, so we manually process exceptional behavior here.
     *
     * TODO: Fix this by ensuring normal Exceptions can be raised
     */
    if(state->vm()->thread_state()->raise_reason() == cException) {
      Exception* exc = as<Exception>(state->vm()->thread_state()->current_exception());
      std::ostringstream msg;

      msg << "exception detected at toplevel: ";
      if(!exc->reason_message()->nil_p()) {
        if(String* str = try_as<String>(exc->reason_message())) {
          msg << str->c_str(state);
        } else {
          msg << "<non-string Exception message>";
        }
      } else if(Exception::argument_error_p(state, exc)) {
        msg << "given "
            << as<Fixnum>(exc->get_ivar(state, state->symbol("@given")))->to_native()
            << ", expected "
            << as<Fixnum>(exc->get_ivar(state, state->symbol("@expected")))->to_native();
      }
      msg << " (" << exc->klass()->debug_str(state) << ")";
      std::cout << msg.str() << "\n";
      exc->print_locations(state);
      Assertion::raise(msg.str().c_str());
    }

    return cNil;
  }

  void CompiledCode::Info::mark(Object* obj, memory::ObjectMark& mark) {
    auto_mark(obj, mark);

    mark_inliners(obj, mark);

    CompiledCode* code = as<CompiledCode>(obj);
    if(!code->machine_code()) return;

    MachineCode* mcode = code->machine_code();
    mcode->set_mark();

    for(int i = 0; i < MachineCode::cMaxSpecializations; i++) {
      // TODO: JIT
    }

    for(size_t i = 0; i < mcode->references_count(); i++) {
      if(size_t ip = mcode->references()[i]) {
        Object* ref = reinterpret_cast<Object*>(mcode->opcodes[ip]);
        if(Object* updated_ref = mark.call(ref)) {
          mcode->opcodes[ip] = reinterpret_cast<intptr_t>(updated_ref);
          mark.just_set(code, updated_ref);
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
    indent_attribute(level, "local_count"); code->local_count()->show(state, level);
    indent_attribute(level, "local_names"); code->local_names()->show_simple(state, level);
    indent_attribute(level, "name"); code->name()->show(state, level);
    indent_attribute(level, "required_args"); code->required_args()->show(state, level);
    indent_attribute(level, "scope"); code->scope()->show(state, level);
    indent_attribute(level, "splat"); code->splat()->show(state, level);
    indent_attribute(level, "stack_size"); code->stack_size()->show(state, level);
    indent_attribute(level, "total_args"); code->total_args()->show(state, level);

    indent_attribute(level, "internalized");
    if(!code->machine_code()) {
      std::cout << "no\n";
    } else {
      std::cout << "yes\n";
    }

    close_body(level);
  }
}

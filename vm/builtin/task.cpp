#include "builtin/task.hpp"
#include "builtin/array.hpp"
#include "builtin/channel.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/contexts.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/nativemethodcontext.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/string.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/symbol.hpp"
#include "builtin/taskprobe.hpp"
#include "builtin/tuple.hpp"

#include "context_cache.hpp"

#include "global_cache.hpp"

#include "objectmemory.hpp"
#include "profiler.hpp"
#include "message.hpp"

#include <iostream>
#include <fstream>

#define INSN_DEBUG

namespace rubinius {

  void Task::init(STATE) {
    GO(task).set(state->new_class("Task", G(object), Task::fields, G(rubinius)));
    G(task)->name(state, state->symbol("Rubinius::Task"));
    G(task)->set_object_type(state, Task::type);

    GO(channel).set(state->new_class("Channel", G(object), Channel::fields));
    G(channel)->set_object_type(state, Channel::type);
  }

  /* HACK:  This method is never used except in tests.
   *        Get rid of it (and fix the tests..)
   */
  Task* Task::create(STATE, OBJECT recv, CompiledMethod* meth) {
    Task* task = create(state, 0);

    Message msg(state);
    msg.name = meth->name() ? meth->name() : state->symbol("__weird_unnamed_method__");
    msg.module = recv->class_object(state);
    msg.method = meth;
    meth->execute(state, task, msg);

    return task;
  }

  Task* Task::create(STATE, size_t stack_size) {
    Task* task = (Task*)state->new_struct(G(task), sizeof(Task));
    task->state = state;
    task->call_flags = 0;
    task->msg = new Message(state);
    task->profiler = NULL;
    task->probe(state, state->probe.get());
    task->control_channel(state, (Channel*)Qnil);
    task->debug_channel(state, (Channel*)Qnil);
    task->exception(state, (Exception*)Qnil);

    if(stack_size == 0) stack_size = CompiledMethod::tramp_stack_size;

    CompiledMethod* cm = CompiledMethod::generate_tramp(state, stack_size);
    MethodContext* ctx = MethodContext::create(state, G(main), cm);

    // fully initialize this context
    ctx->sender(state, (MethodContext*)Qnil);
    ctx->name(state, state->symbol("__trampoline__"));
    ctx->block(state, Qnil);
    ctx->module(state, G(object));

    task->active(state, ctx);

    return task;
  }

  Task* Task::current(STATE) {
    return state->globals.current_task.get();
  }

  MethodContext* Task::current_context(STATE) {
    MethodContext* context = active_;
    context->reference(state);
    return context;
  }

  // Primitive
  Channel* Task::get_debug_channel(STATE) {
    return debug_channel_;
  }

  // Primitive
  Channel* Task::get_control_channel(STATE) {
    return control_channel_;
  }

  void Task::restore_context(MethodContext* ctx) {
    active(state, ctx);

    /* Stack Management procedures. Make sure that we don't
     * miss object stored into the stack of a context */
    if(ctx->zone == MatureObjectZone) {
      state->om->remember_object(ctx);
    }

    // We do this because blocks set locals in their home and we need
    // catch that.
    //
    // NOTE we could instead manually run the write barrier when setting
    // locals.
    if(home()->zone == MatureObjectZone && !home()->Remember) {
      state->om->remember_object(home());
    }
  }

  void Task::make_active(MethodContext* ctx) {
    ctx->sender(state, active_);

    restore_context(ctx);
  }

  /* Only called if send_message can't locate anything to run, which pretty
   * much never happens, since it means even method_missing wasn't available. */
  void Task::tragic_failure(Message& msg) {
    if(!probe_->nil_p()) {
      probe_->lookup_failed(this, msg);
    }
    std::stringstream ss;
    ss << "unable to locate any method '" << msg.send_site->name()->c_str(state) <<
      "' from '" << msg.lookup_from->name()->c_str(state) << "'";

    Exception::assertion_error(state, ss.str().c_str());
  }

  /* For details in msg, locate the proper method and begin execution
   * of it. */
  ExecuteStatus Task::send_message(Message& msg) {
    if(!msg.send_site->locate(state, msg)) tragic_failure(msg);

    if(!probe_->nil_p()) probe_->execute_method(state, this, msg);

    return msg.method->execute(state, this, msg);
  }

  ExecuteStatus Task::send_message_slowly(Message& msg) {
    SYMBOL original_name = msg.name;
    if(!GlobalCacheResolver::resolve(state, msg)) {
      msg.method_missing = true;
      msg.name = G(sym_method_missing);
      msg.priv = true; // lets us look for method_missing anywhere
      if(!GlobalCacheResolver::resolve(state, msg)) {
        tragic_failure(msg);
      }
    }

    if(msg.method_missing) {
      msg.unshift_argument(state, original_name);
    }

    if(!probe_->nil_p()) probe_->execute_method(state, this, msg);

    return msg.method->execute(state, this, msg);
  }

  bool Task::passed_arg_p(size_t pos) {
    return active_->args >= pos;
  }

  OBJECT Task::call_object(STATE, OBJECT recv, SYMBOL meth, Array* args) {
    recv->send_on_task(state, this, meth, args);

    // HACK by calling directly into another Task, the context cache gets
    // confused. So we force references to the top contexts here to keep
    // the cache sane.
    active_->reference(state);
    home()->reference(state);
    return Qtrue;
  }

  void Task::restore_sender() {
    MethodContext *target = active_->sender();

    if(unlikely(profiler)) profiler->leave_method();
    /* Try to recycle this context to be used again. */
    active_->recycle(state);

    restore_context(target);
  }

  /* TODO: Mirrors restore_sender too much, unify. */
  void Task::native_return(Object* return_value)
  {
    if (profiler) {
      profiler->leave_method();
    }

    NativeMethodContext* nmc = try_as<NativeMethodContext>(active_->sender());

    if (nmc) {
      active(state, nmc);

      nmc->value_returned_to_c(return_value);
      nmc->action(NativeMethodContext::RETURNED_BACK_TO_C);

      NativeMethod::activate_from(nmc);
    }
    else {
      restore_context(active_->sender());
      active_->push(return_value);
    }
  }

  /* Called after a primitive has executed and wants to return a value. */
  void Task::primitive_return(OBJECT value, Message& msg) {
    active_->clear_stack(msg.stack);
    push(value);
  }

  // Used as a primitive.
  Task* Task::raise(STATE, Exception* exc) {
    raise_exception(exc);
    return this;
  }

  void Task::raise_exception(Exception* exc) {
    exception(state, exc); // HACK test that we set this

    for(;;) {
      int ip = active_->ip;
      Tuple* table = active_->cm()->exceptions();

      if(!table->nil_p()) {
        for(size_t i = 0; i < table->num_fields(); i++) {
          Tuple* entry = as<Tuple>(table->at(state, i));
          if(as<Integer>(entry->at(state, 0))->to_native() <= ip
              && as<Integer>(entry->at(state, 1))->to_native() >= ip) {
            // Reset the stack back to the top. If we don't, then values
            // on the stack when the exception occurs end up accumulate, and
            // we run out of stack!
            active_->position_stack(active_->vmm->number_of_locals - 1);
            set_ip(as<Integer>(entry->at(state, 2))->to_native());
            return;
          }
        }
      }

      if(active_->sender()->nil_p()) break;
      if(profiler) profiler->leave_method();

      restore_sender();
    }
  }

  Tuple* Task::locate_method_on(OBJECT recv, SYMBOL sel, OBJECT priv_p) {
    Message msg(state);

    msg.recv = recv;
    msg.lookup_from = recv->lookup_begin(state);
    msg.name = sel;
    msg.priv = (priv_p == Qtrue);
    msg.set_caller(active_);

    if(!GlobalCacheResolver::resolve(state, msg)) {
      return (Tuple*)Qnil;
    }

    MethodVisibility *vis;

    vis = try_as<MethodVisibility>(msg.method);
    if(vis) {
      return Tuple::from(state, 2, vis->method(), msg.module);
    }

    return Tuple::from(state, 2, msg.method, msg.module);
  }

  void Task::attach_method(OBJECT recv, SYMBOL name, CompiledMethod* method) {
    if(kind_of<Module>(recv)) {
      StaticScope* ss = StaticScope::create(state);
      ss->module(state, (Module*)recv);
      ss->parent(state, method->scope());
      method->scope(state, ss);
    } else {
      /* Push the current scope down. */
      method->scope(state, active_->cm()->scope());
    }

    add_method(recv->metaclass(state), name, method);
  }

  void Task::add_method(Module* mod, SYMBOL name, CompiledMethod* method) {
    method->scope(state, active_->cm()->scope());
    method->serial(state, Fixnum::from(0));
    mod->method_table()->store(state, name, method);
    state->global_cache->clear(mod, name);

    if(!probe_->nil_p()) {
      probe_->added_method(this, mod, name, method);
    }

    if(instance_of<Class>(mod)) {
      Class* cls = as<Class>(mod);

      method->formalize(state, false);

      object_type type = (object_type)cls->instance_type()->to_native();
      TypeInfo* ti = state->om->type_info[type];
      if(ti) {
        method->specialize(state, ti);
      }
    }
  }

  bool Task::check_serial(OBJECT obj, SYMBOL sel, int ser) {
    Tuple* tup = locate_method_on(obj, sel, Qtrue);
    Executable* x = as<Executable>(tup->at(state, 0));

    /* If the method is absent, then indicate that the serial number
     * is correct. */
    if(x->nil_p()) return true;

    CompiledMethod* cm = try_as<CompiledMethod>(x);

    if(cm) return cm->serial()->to_native() == ser;

    return false;
  }

  /* Find +name+, a constant, by looking:
   *   in +mod+,
   *   up +mod+'s superclass heirarchy.
   */
  OBJECT Task::const_get(Module* mod, SYMBOL name, bool* found) {
    OBJECT res;

    *found = false;

    while(!mod->nil_p()) {
      res = mod->get_const(state, name, found);
      if(*found) return res;

      if(mod == G(object)) break;
      mod = mod->superclass();
    }

    return Qnil;
  }

  /* Find +name+ by:
   *   looking in the up lexical scope,
   *   looking in the superclass's of the current lexical module,
   *   looking in Object.
   */
  OBJECT Task::const_get(SYMBOL name, bool* found) {
    StaticScope *cur;
    OBJECT result;

    *found = false;

    cur = active_->cm()->scope();
    while(!cur->nil_p()) {
      result = cur->module()->get_const(state, name, found);
      if(*found) return result;

      if(cur->module() == G(object)) break;

      cur = cur->parent();
    }

    Module* mod = active_->cm()->scope()->module();
    while(!mod->nil_p()) {
      result = mod->get_const(state, name, found);
      if(*found) return result;

      mod = mod->superclass();
    }

    /* Lastly, check Object specificly */
    result = G(object)->get_const(state, name, found);
    if(*found) return result;

    return Qnil;
  }

  void Task::const_set(Module* mod, SYMBOL name, OBJECT val) {
    mod->set_const(state, name, val);
  }

  void Task::const_set(SYMBOL name, OBJECT val) {
    active_->cm()->scope()->module()->set_const(state, name, val);
  }

  // TODO - Make sure this cannot contaminate the stack!
  void Task::yield_debugger() {
    Channel* chan;
    if(debug_channel_->nil_p()) {
      chan = try_as<Channel>(G(vm)->get_ivar(state,
            state->symbol("@debug_channel")));

      if(!chan) return;
    } else {
      chan = debug_channel_;
    }

    if(control_channel_->nil_p()) {
      control_channel(state, Channel::create(state));
    }

    sassert(control_channel_->has_readers_p());

    active_->reference(state);

    debug_channel_->send(state, active_);
    control_channel_->receive(state);
  }

  Module* Task::current_module() {
    return home()->module();
  }

  static Class* check_superclass(STATE, Class* cls, OBJECT super) {
    if(super->nil_p()) return cls;
    if(cls->superclass() != super) {
      Exception::type_error(state, Class::type, super, "superclass mismatch");
    }

    return cls;
  }

  static Class* add_class(STATE, Module* under, OBJECT super, SYMBOL name) {
    if(super->nil_p()) super = G(object);
    Class* cls = Class::create(state, as<Class>(super));

    if(under == G(object)) {
      cls->name(state, name);
    } else {
      cls->set_name(state, under, name);
    }

    under->set_const(state, name, cls);

    return cls;
  }

  Class* Task::open_class(Module* under, OBJECT super, SYMBOL name, bool* created) {
    bool found;

    *created = false;

    OBJECT obj = under->get_const(state, name, &found);
    if(found) return check_superclass(state, as<Class>(obj), super);

    *created = true;
    return add_class(state, under, super, name);
  }

  Class* Task::open_class(OBJECT super, SYMBOL name, bool* created) {
    Module* under;

    if(active_->cm()->scope()->nil_p()) {
      under = G(object);
    } else {
      under = active_->cm()->scope()->module();
    }

    return open_class(under, super, name, created);
  }

  Module* Task::open_module(SYMBOL name) {
    Module* mod;
    Module* under;

    bool found;

    OBJECT obj = const_get(name, &found);
    if(found) return as<Module>(obj);

    mod = Module::create(state);
    if(active_->cm()->scope()->nil_p()) {
      under = G(object);
      mod->name(state, name);
    } else {
      under = active_->cm()->scope()->module();
      mod->set_name(state, under, name);
    }

    under->set_const(state, name, mod);

    return mod;
  }

  Module* Task::open_module(Module* under, SYMBOL name) {
    Module* mod;
    bool found;

    OBJECT obj = const_get(under, name, &found);
    if(found) return as<Module>(obj);

    mod = Module::create(state);
    if(under == G(object)) {
      mod->name(state, name);
    } else {
      mod->set_name(state, under, name);
    }

    under->set_const(state, name, mod);

    return mod;
  }

  /* Used only in debugging and testing. Direct access to the stack
   * can be dangerous. */
  OBJECT* Task::current_stack() {
    return active_->stk;
  }

  void Task::push(OBJECT val) {
    active_->push(val);
  }

  OBJECT Task::pop() {
    return active_->pop();
  }

  OBJECT Task::stack_top() {
    return active_->top();
  }

  /* Retrieve the object at position +pos+ in the current context */
  OBJECT Task::stack_at(size_t pos) {
    return active_->stack_at(pos);
  }

  int Task::calculate_sp() {
    return active_->calculate_sp();
  }

  /* Set the local variable at +pos+ to +val+ in the current context. */
  void Task::set_local(int pos, OBJECT val) {
    active_->set_local(pos, val);
  }

  /* Get local variable at +pos+ in the current context. */
  OBJECT Task::get_local(int pos) {
    return active_->get_local(pos);
  }

  /* Move the active context to executing instruction +ip+. */
  void Task::set_ip(int ip) {
    active_->ip = ip;
  }

  /* Returns the current instruction the active context is on. */
  int Task::current_ip() {
    return active_->ip;
  }

  void Task::enable_profiler() {
    profiler = new profiler::Profiler();
  }

  void Task::disable_profiler(char* results) {
    if(profiler) {
      std::ofstream stream(results);
      profiler->print_results(state, stream);
      delete profiler;
    }

    profiler = NULL;
  }

  /* This should only ever run for CompiledMethods currently. */
  void Task::execute() {
    try {
      for(;;) {
        active_->vmm->resume(this, active_);

        // Should we inspect the other interrupts?
        if(state->interrupts.check) {
          state->interrupts.check = false;
          return;
        }

      }
    } catch(RubyException &exc) {
      raise_exception(exc.exception);
    } catch(TypeError &exc) {
      Exception* e = Exception::make_type_error(state, exc.type, exc.object);
      raise_exception(e);
    }
  }

  /** TODO: Refactor the hell out of this with streams. */
  void Task::print_backtrace(MethodContext* ctx) {
    if(!ctx) ctx = active_;

    while(!ctx->nil_p()) {
      std::cout << static_cast<void*>(ctx) << ": ";

      if(kind_of<BlockContext>(ctx)) {
        std::cout << "__block__";
      } else {
        if(MetaClass* meta = try_as<MetaClass>(ctx->module())) {
          if(Module* mod = try_as<Module>(meta->attached_instance())) {
            std::cout << mod->name()->c_str(state) << ".";
          } else {
            std::cout << "#<" <<
              meta->attached_instance()->class_object(state)->name()->c_str(state) <<
              ":" << (void*)meta->attached_instance() << ">.";
          }
        } else {
          std::cout << ctx->module()->name()->c_str(state) << "#";
        }

        if (kind_of<NativeMethodContext>(ctx)) {
          /* Muhahaa. */
          std::cout << as<NativeMethodContext>(ctx) << std::endl;
          ctx = ctx->sender();
          continue;
        }

        SYMBOL name = try_as<Symbol>(ctx->name());
        if(name) {
          std::cout << name->c_str(state);
        } else {
          std::cout << ctx->cm()->name()->c_str(state);
        }
      }

      std::cout << " in ";
      if(SYMBOL file_sym = try_as<Symbol>(ctx->cm()->file())) {
        std::cout << file_sym->c_str(state) << ":" << ctx->line(state);
      } else {
        std::cout << "<unknown>";
      }

      std::cout << std::endl;
      ctx = ctx->sender();
    }
  }

  void Task::Info::show(STATE, OBJECT self, int level) {
    Task* task = as<Task>(self);

    class_header(state, self);
    indent_attribute(++level, "self"); task->self()->show(state, level);
    indent_attribute(level, "active"); task->active()->show(state, level);
    indent_attribute(level, "home"); task->home()->show(state, level);
    indent_attribute(level, "exception"); task->exception()->show(state, level);
    indent_attribute(level, "debug_channel"); task->debug_channel()->show(state, level);
    indent_attribute(level, "control_channel"); task->control_channel()->show(state, level);
    close_body(level);
  }
}

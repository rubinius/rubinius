#include "builtin/task.hpp"
#include "builtin/array.hpp"
#include "builtin/channel.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/contexts.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/string.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"

#include "resolver.hpp"
#include "global_cache.hpp"
#include "probes.hpp"

#include "objectmemory.hpp"

#define INSN_DEBUG

namespace rubinius {

  void Task::init(STATE) {
    GO(task).set(state->new_class("Task", Task::fields));
    G(task)->set_object_type(Task::type);

    GO(channel).set(state->new_class("Channel", Channel::fields));
    G(channel)->set_object_type(Channel::type);
  }

  Task* Task::create(STATE, OBJECT recv, CompiledMethod* meth) {
    Task* task = create(state);

    Message msg(state, Array::create(state, 0));
    meth->execute(state, task, msg);

    return task;
  }

  Task* Task::create(STATE) {
    Task* task = (Task*)state->new_struct(G(task), sizeof(Task));
    task->ip = 0;
    task->state = state;
    task->probe = state->probe;
    task->msg = new Message(state);
    SET(task, control_channel, Qnil);
    SET(task, debug_channel, Qnil);
    SET(task, self, G(main));
    SET(task, literals, Qnil);
    SET(task, exception, Qnil);
    SET(task, home, Qnil);

    CompiledMethod* cm = CompiledMethod::generate_tramp(state);
    MethodContext* ctx = task->generate_context(G(main), cm);

    SET(task, active, ctx);

    return task;
  }

  MethodContext* Task::generate_context(OBJECT recv, CompiledMethod* meth) {
    MethodContext* ctx = MethodContext::create(state);

    SET(ctx, sender, (MethodContext*)Qnil);
    SET(ctx, self, recv);
    SET(ctx, cm, meth);
    SET(ctx, module, G(object));
    SET(ctx, stack, Tuple::create(state, meth->stack_size->n2i()));
    SET(ctx, home, ctx);

    ctx->vmm = (VMMethod*)meth->executable;
    ctx->ip = 0;
    ctx->sp = meth->number_of_locals() - 1;
    ctx->js.stack = ctx->stack->field + ctx->sp;

    return ctx;
  }

  void Task::restore_context(MethodContext* ctx) {

    SET(this, literals, ctx->cm->literals);

    SET(this, active, ctx);
    SET(this, home, ctx->home);
    SET(this, self, home->self);

    ip = ctx->ip;
    ip_ptr = ctx->vmm->opcodes + ip;
  }

  void Task::make_active(MethodContext* ctx) {
    /* Save the current Task registers into active. */
    if(!active->nil_p()) {
      active->ip = ip;
    }

    SET(ctx, sender, active);

    restore_context(ctx);
  }


  void Task::import_arguments(MethodContext* ctx, Message& msg) {
    size_t total = ctx->cm->total_args->n2i();
    size_t required = ctx->cm->required_args->n2i();
    size_t fixed;

    ctx->block = msg.block;
    ctx->args = msg.args;

    /* No input args and no expected args. Done. */
    if(total == 0 && msg.args == 0) goto stack_cleanup;

    /* If too few args were passed in, throw an exception */
    if(msg.args < required) {
      throw new ArgumentError(required, msg.args);
    }

    /* If too many args were passed in, throw an exception.
     * * If there is a splat, this check is disabled.*/
    if(ctx->cm->splat == Qnil && msg.args > total) {
      throw new ArgumentError(required, msg.args);
    }

    fixed = total;
    if(msg.args < total) fixed = msg.args;

    for(size_t i = 0; i < fixed; i++) {
      ctx->stack->put(msg.state, i, msg.get_argument(i));
    }

    if(ctx->cm->splat != Qnil) {
      size_t splat_size = msg.args - total;
      Array* ary = Array::create(state, splat_size);

      for(size_t i = 0, n = total; i < splat_size; i++, n++) {
        ary->set(state, i, msg.get_argument(n));
      }

      ctx->stack->put(msg.state, as<Integer>(ctx->cm->splat)->n2i(), ary);
    }

    if(probe) {
      probe->start_method(this, msg);
    }

    /* Now that we've processed everything from the stack, we need to clean it up */
stack_cleanup:
    active->clear_stack(msg.stack);
  }

  /* Only called if send_message can't locate anything to run, which pretty
   * much never happens, since it means even method_missing wasn't available. */
  void Task::tragic_failure(Message& msg) {
    if(probe) {
      probe->lookup_failed(this, msg);
    }
    std::stringstream ss;
    ss << "unable to locate any method '" << *msg.send_site->name->to_str(state) << 
      "' from '" << *msg.lookup_from->name->to_str(state) << "'";

    throw new Assertion((char*)ss.str().c_str());
  }

  /* For details in msg, locate the proper method and begin execution
   * of it. */
  bool Task::send_message(Message& msg) {
    if(!msg.send_site->locate(state, msg)) tragic_failure(msg);
    return msg.method->execute(state, this, msg);
  }

  bool Task::send_message_slowly(Message& msg) {
    GlobalCacheResolver res;

    if(!res.resolve(state, msg)) {
      msg.name = G(sym_method_missing);
      if(!res.resolve(state, msg)) {
        tragic_failure(msg);
      }
    }

    return msg.method->execute(state, this, msg);
  }

  bool Task::passed_arg_p(size_t pos) {
    return active->args >= pos;
  }

  void Task::simple_return(OBJECT value) {
    MethodContext *target = active->sender;

    restore_context(target);
    active->push(value);
  }

  /* Called after a primitive has executed and wants to return a value. */
  void Task::primitive_return(OBJECT value, Message& msg) {
    active->clear_stack(msg.stack);
    push(value);
  }

  void Task::raise_exception(Exception* exc) {
    for(;;) {
      Tuple* table = active->cm->exceptions;

      if(!table->nil_p()) {
        for(size_t i = 0; i < table->field_count; i++) {
          Tuple* entry = as<Tuple>(table->at(i));
          if(as<Integer>(entry->at(0))->n2i() <= ip && as<Integer>(entry->at(1))->n2i() >= ip) {
            ip = as<Integer>(entry->at(2))->n2i();
            return;
          }
        }
      }

      if(active->sender->nil_p()) break;
      make_active(active->sender);
    }
  }

  Executable* Task::locate_method_on(OBJECT recv, SYMBOL sel, OBJECT priv_p) {
    Message msg(state);

    msg.recv = recv;
    msg.lookup_from = recv->lookup_begin(state);
    msg.name = sel;
    msg.priv = (priv_p == Qtrue);

    GlobalCacheResolver res;
    if(!res.resolve(state, msg)) {
      return (Executable*)Qnil;
    }

    MethodVisibility *vis;

    vis = try_as<MethodVisibility>(msg.method);
    if(vis) {
      return vis->method;
    }

    return msg.method;
  }

  void Task::attach_method(OBJECT recv, SYMBOL name, CompiledMethod* method) {
    if(kind_of<Module>(recv)) {
      StaticScope* ss = StaticScope::create(state);
      SET(ss, module, recv);
      SET(ss, parent, method->scope);
      SET(method, scope, ss);
    } else {
      /* Push the current scope down. */
      SET(method, scope, active->cm->scope);
    }

    add_method(recv->metaclass(state), name, method);
  }

  void Task::add_method(Module* mod, SYMBOL name, CompiledMethod* method) {
    SET(method, scope, active->cm->scope);
    mod->method_table->store(state, name, method);
    state->global_cache->clear(mod, name);

    if(probe) {
      probe->added_method(this, mod, name, method);
    }

    if(instance_of<Class>(mod)) {
      Class* cls = as<Class>(mod);

      object_type type = (object_type)cls->instance_type->n2i();
      TypeInfo* ti = state->om->type_info[type];
      if(!ti) {
        ti = new TypeInfo((object_type)0);
      }

      method->specialize(ti);
    }
  }

  bool Task::check_serial(OBJECT obj, SYMBOL sel, int ser) {
    Executable* x = locate_method_on(obj, sel, Qtrue);

    /* If the method is absent, then indicate that the serial number
     * is correct. */
    if(x->nil_p()) return true;

    CompiledMethod* cm = try_as<CompiledMethod>(x);

    if(cm) return cm->serial->n2i() == ser;

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
      mod = mod->superclass;
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

    cur = active->cm->scope;
    while(!cur->nil_p()) {
      result = cur->module->get_const(state, name, found);
      if(*found) return result;

      if(cur->module == G(object)) break;

      cur = cur->parent;
    }

    Module* mod = active->cm->scope->module;
    while(!mod->nil_p()) {
      result = mod->get_const(state, name, found);
      if(*found) return result;

      mod = mod->superclass;
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
    active->cm->scope->module->set_const(state, name, val);
  }

  // TODO - Make sure this cannot contaminate the stack!
  void Task::yield_debugger() {
    Channel* chan;
    if(debug_channel->nil_p()) {
      chan = try_as<Channel>(G(vm)->get_ivar(state,
            state->symbol("@debug_channel")));

      if(!chan) return;
    } else {
      chan = debug_channel;
    }

    if(control_channel->nil_p()) {
      control_channel = Channel::create(state);
    }

    sassert(control_channel->has_readers_p());

    active->reference(state);

    debug_channel->send(state, active);
    control_channel->receive(state);
  }

  Module* Task::current_module() {
    return active->cm->scope->module;
  }

  static Class* check_superclass(Class* cls, OBJECT super) {
    if(super->nil_p()) return cls;
    if(cls->superclass != super) {
      throw new TypeError(Class::type, super, "superclass mismatch");
    }

    return cls;
  }

  static Class* add_class(STATE, Module* under, OBJECT super, SYMBOL name) {
    if(super->nil_p()) super = G(object);
    Class* cls = Class::create(state, as<Class>(super));

    if(under == G(object)) {
      SET(cls, name, name);
    } else {
      cls->set_name(state, under, name);
    }

    under->set_const(state, name, cls);

    return cls;
  }

  Class* Task::open_class(Module* under, OBJECT super, SYMBOL name, bool* created) {
    bool found;

    *created = false;

    OBJECT obj = const_get(under, name, &found);
    if(found) return check_superclass(as<Class>(obj), super);

    *created = true;
    return add_class(state, under, super, name);
  }

  Class* Task::open_class(OBJECT super, SYMBOL name, bool* created) {
    bool found;
    Module* under;

    *created = false;

    OBJECT obj = const_get(name, &found);
    if(found) return check_superclass(as<Class>(obj), super);

    if(active->cm->scope->nil_p()) {
      under = G(object);
    } else {
      under = active->cm->scope->module;
    }

    *created = true;
    return add_class(state, under, super, name);
  }

  Module* Task::open_module(SYMBOL name) {
    Module* mod;
    Module* under;

    bool found;

    OBJECT obj = const_get(name, &found);
    if(found) return as<Module>(obj);

    mod = Module::create(state);
    if(active->cm->scope->nil_p()) {
      under = G(object);
      SET(mod, name, name);
    } else {
      under = active->cm->scope->module;
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
      SET(mod, name, name);
    } else {
      mod->set_name(state, under, name);
    }

    under->set_const(state, name, mod);

    return mod;
  }

  /* Used in testing. Sets the stack of the current context to +stack+ */
  void Task::set_stack(Tuple* stack) {
    SET(active, stack, stack);
    /* the - 1 is because the stack starts below the bottom, so push
     * always increments and sets. */
    active->js.stack = stack->field - 1;
  }

  Tuple* Task::current_stack() {
    return active->stack;
  }

  void Task::push(OBJECT val) {
    active->push(val);
  }

  OBJECT Task::pop() {
    return active->pop();
  }

  int Task::calculate_sp() {
    return active->js.stack - active->stack->field;
  }

  void Task::activate_method(Message&) { }

  void Task::cache_ip() { }
  void Task::check_interrupts() {
    if(state->om->collect_young_now) {
      state->om->collect_young_now = false;
      state->om->collect_young(state->globals.roots);
    }

    if(state->om->collect_mature_now) {
      state->om->collect_mature_now = false;
      state->om->collect_mature(state->globals.roots);
    }
  }

  void Task::execute() {
    if(active->nil_p()) return;

    for(;;) {
      active->vmm->resume(this, active);
    }
  }

}

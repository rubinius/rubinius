#include "builtin.hpp"
#include "vmmethod.hpp"
#include "builtin_task.hpp"
#include "builtin_channel.hpp"
#include "message.hpp"
#include <csignal>

namespace rubinius {
  void Task::init(STATE) {
    Tuple* tup = Tuple::from(state, 7, List::create(state), List::create(state),
        List::create(state), List::create(state), List::create(state),
        List::create(state), List::create(state));

    GO(scheduled_threads).set(tup);

    Class* cls = state->new_class("Task", Task::fields);
    cls->set_const(state, "ScheduledThreads", tup);

    GO(channel).set(state->new_class("Channel", Channel::fields));
    G(channel)->set_object_type(Channel::type);

    GO(thread).set(state->new_class("Thread",  Thread::fields));
    G(thread)->set_object_type(Thread::type);
  }

  Task* Task::create(STATE, OBJECT recv, CompiledMethod* meth) {
    Task* task = (Task*)state->new_struct(G(task), sizeof(Task));
    task->ip = 0;
    task->sp = -1;
    task->state = state;
    task->make_active(task->generate_context(recv, meth));
    return task;
  }

  Task* Task::create(STATE) {
    Task* task = (Task*)state->new_struct(G(task), sizeof(Task));
    task->ip = 0;
    task->sp = -1;
    task->state = state;
    return task;
  }

  MethodContext* Task::generate_context(OBJECT recv, CompiledMethod* meth) {
    MethodContext* ctx = MethodContext::create(state);

    ctx->sender = (MethodContext*)Qnil;
    ctx->self = recv;
    ctx->cm = meth;
    ctx->vmm = meth->vmmethod(state);
    ctx->module = G(object);
    ctx->stack = Tuple::create(state, meth->stack_size->n2i());
    ctx->ip = 0;
    ctx->sp = meth->number_of_locals() - 1;

    return ctx;
  }

  void Task::make_active(MethodContext* ctx) {
    ip = ctx->ip;
    sp = ctx->sp;
    self = ctx->self;

    literals = ctx->cm->literals;
    stack = ctx->stack;
    ip_ptr = ctx->vmm->opcodes + ip;

    active = ctx;
  }

  void Task::import_arguments(MethodContext* ctx, Message& msg) {
    size_t total = ctx->cm->total_args->n2i();
    size_t required = ctx->cm->required_args->n2i();

    ctx->args = msg.args;

    /* No input args and no expected args. Done. */
    if(total == 0 && msg.args == 0) return;

    /* If too few args were passed in, throw an exception */
    if(msg.args < required) {
      throw new ArgumentError(required, msg.args);
    }

    /* If too many args were passed in, throw an exception.
     * * If there is a splat, this check is disabled.*/
    if(ctx->cm->splat == Qnil && msg.args > total) {
      throw new ArgumentError(required, msg.args);
    }

    size_t fixed = total;
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

      ctx->stack->put(msg.state, ctx->cm->splat->n2i(), ary);
    }
  }

  /* For details in msg, locate the proper method and begin execution
   * of it. */
  void Task::send_message(Message& msg) {
    if(!msg.send_site->locate(state, msg)) {
      throw new Assertion("unable to locate any method");
    }

    MethodContext* ctx = generate_context(msg.recv, as<CompiledMethod>(msg.method));

    import_arguments(ctx, msg);

    active->ip = ip;
    active->sp = sp;
    ctx->sender = active;

    make_active(ctx);
  }

  void Task::send_message_slowly(Message& msg) {
    GlobalCacheResolver res;

    if(!res.resolve(state, msg)) {
      if(!res.resolve(state, msg)) {
        throw new Assertion("unable to locate any method");
      }
    }

    MethodContext* ctx = generate_context(msg.recv, as<CompiledMethod>(msg.method));

    active->ip = ip;
    active->sp = sp;
    ctx->sender = active;

    make_active(ctx);
  }

 bool Task::perform_hook(OBJECT obj, SYMBOL name, OBJECT arg) {
    Message msg(state);
    msg.recv = obj;
    msg.lookup_from = obj->lookup_begin(state);
    msg.name = name;
    msg.args = 1;

    GlobalCacheResolver res;

    /* If we can't find it, give up. */
    if(!res.resolve(state, msg)) return false;

    MethodContext* ctx = generate_context(obj, as<CompiledMethod>(msg.method));
    /* Make sure we discard the return value */
    ctx->no_value = true;

    active->ip = ip;
    active->sp = sp;

    ctx->sender = active;

    make_active(ctx);

    return true;
  }

  bool Task::passed_arg_p(size_t pos) {
    return active->args >= pos;
  }

  void Task::simple_return(OBJECT value) {
    MethodContext *target = active->sender;
    bool push_value = !active->no_value;

    make_active(target);
    if(push_value) stack->put(state, ++sp, value);
  }

  void Task::raise_exception(Exception* exc) {
    for(;;) {
      Tuple* table = active->cm->exceptions;

      if(!table->nil_p()) {
        for(size_t i = 0; i < table->field_count; i++) {
          Tuple* entry = as<Tuple>(table->at(i));
          if(entry->at(0)->n2i() <= ip && entry->at(1)->n2i() >= ip) {
            ip = entry->at(2)->n2i();
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

    CompiledMethod::Visibility *vis;

    vis = try_as<CompiledMethod::Visibility>(msg.method);
    if(vis) {
      return vis->method;
    }

    return as<Executable>(msg.method);
  }

  void Task::attach_method(OBJECT recv, SYMBOL name, CompiledMethod* method) {
    add_method(recv->metaclass(state), name, method);
  }

  void Task::add_method(Module* mod, SYMBOL name, CompiledMethod* method) {
    mod->method_table->store(state, name, method);
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

  void Task::yield_debugger(OBJECT val) {
    kill(getpid(), SIGTRAP);
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

#define stack_push(val) stack->field[++sp] = (val)
#define stack_pop() stack->field[sp--]
#define stack_top() stack->field[sp]
#define stack_set_top(val) stack->field[sp] = (val)
#define stack_back(idx) stack->field[sp - idx]

#define check_bounds(obj, index)
#define check(expr)
#define next_op() *ip_ptr++
#define locals (stack)
#define current_block  (active->block)
#define argcount (active->args)
#define fast_fetch(obj, index) (obj->at(index))
#define current_scope (active->cm->scope)

#define next_literal_into(val) next_int; val = literals->at(_int)
#define next_int_into(val) val = (int)(next_op())
#define next_int _int = (int)(next_op())
#define next_literal next_int; _lit = literals->at(_int)

insn_start:
    if(active->nil_p()) return;

    for(;;) {
next_op:
      opcode op = next_op();
#include "gen/task_instructions_switch.c"
check_interrupts:
      check_interrupts();
    }
  }

#undef next_op
#define next_op() *stream++
  void Task::execute_stream(opcode* stream) {
    opcode op = next_op();
#include "gen/task_instructions_switch.c"
insn_start:
next_op:
check_interrupts:
    return;
  }
}

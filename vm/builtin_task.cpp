#include "objects.hpp"
#include "vmmethod.hpp"
#include "builtin_task.hpp"
#include "builtin_channel.hpp"
#include "cpu.hpp"

namespace rubinius {
  void Task::init(STATE) {
    Tuple* tup = Tuple::from(state, 7, List::create(state), List::create(state),
        List::create(state), List::create(state), List::create(state),
        List::create(state), List::create(state));

    G(scheduled_threads) = tup;

    Class* cls = state->new_class("Task", Task::fields);
    cls->set_const(state, "ScheduledThreads", tup);

    G(channel) = state->new_class("Channel", Channel::fields);
    G(channel)->set_object_type(Channel::type);

    G(thread) =  state->new_class("Thread",  Thread::fields);
    G(thread)->set_object_type(Thread::type);
  }

  Task* Task::create(STATE, OBJECT recv, CompiledMethod* meth) {
    Task* task = (Task*)state->new_struct(G(task), sizeof(Task));
    task->state = state;
    task->active = task->generate_context(state, recv, meth);
    return task;
  }

  MethodContext* Task::generate_context(STATE, OBJECT recv, CompiledMethod* meth) {
    MethodContext* ctx = MethodContext::create(state);

    ctx->sender = (MethodContext*)Qnil;
    ctx->self = recv;
    ctx->cm = meth;
    ctx->module = G(object);
    ctx->stack = Tuple::create(state, meth->stack_size());
    ctx->ip = 0;
    ctx->sp = meth->number_of_locals();

    return ctx;
  }

  void Task::send(OBJECT, OBJECT, size_t, OBJECT) { }
  void Task::simple_return(OBJECT) { }
  Executable* Task::locate_method_on(OBJECT, OBJECT, OBJECT) { }
  OBJECT Task::attach_method(OBJECT, OBJECT, CompiledMethod*) { }
  OBJECT Task::add_method(OBJECT, OBJECT, CompiledMethod*) { }
  void Task::send_message(Message&) { }
  OBJECT Task::check_serial(OBJECT, OBJECT, int) { }
  OBJECT Task::const_get_from(OBJECT, OBJECT) { }
  OBJECT Task::const_get_in_context(OBJECT) { }
  OBJECT Task::const_set(OBJECT, OBJECT, OBJECT) { }
  OBJECT Task::const_set(OBJECT, OBJECT) { }
  void Task::yield_debugger() { }
  Module* Task::current_module() { }
  void Task::check_interrupts() { }
  Class* Task::open_class(OBJECT, OBJECT, OBJECT, int*) { }
  Class* Task::open_class(OBJECT, OBJECT, int*) { }
  void Task::cache_ip() { }
  Module* Task::open_module(OBJECT) { }
  Module* Task::open_module(OBJECT, OBJECT) { }
  OBJECT Task::perform_hook(OBJECT, OBJECT, OBJECT) { }
  void Task::raise_exception(Exception* exc) { }
  void Task::activate_method(Message&) { }

  void Task::execute() {
    ip = active->ip;
    sp = active->sp;
    self = active->self;

    literals = active->cm->literals;
    stack = active->stack;
    ip_ptr = active->vmm->opcodes + ip;

#define stack_push(val) stack->field[++sp] = (val)
#define stack_pop() stack->field[sp--]
#define stack_top() stack->field[sp]
#define stack_set_top(val) stack->field[sp] = (val)
#define stack_back(idx) stack->field[sp - idx]

#define check_bounds(obj, index)
#define check(expr)
#define next_op() *ip_ptr++
#define locals (active->stack)
#define current_block  (active->block)
#define argcount (active->args)
#define fast_fetch(obj, index) (obj->at(index))
#define current_scope (active->cm->scope)

#define next_literal_into(val) next_int; val = literals->at(_int)
#define next_int_into(val) val = (int)(*ip_ptr++)
#define next_int _int = (int)(*ip_ptr++)
#define next_literal next_int; _lit = literals->at(_int)

insn_start:
    if(active->nil_p()) return;

    for(;;) {
next_op:
      opcode op = next_op();
#include "instructions.c.gen"
check_interrupts:
      check_interrupts();
    }
  }
}

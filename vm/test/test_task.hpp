#include "builtin/task.hpp"
#include "builtin/list.hpp"

#include "builtin/contexts.hpp"
#include "builtin/exception.hpp"
#include "builtin/taskprobe.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "global_cache.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestTask : public CxxTest::TestSuite {
  public:

  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  CompiledMethod* create_cm() {
    CompiledMethod* cm = CompiledMethod::create(state);
    cm->iseq(state, InstructionSequence::create(state, 1));
    cm->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    cm->stack_size(state, Fixnum::from(10));
    cm->total_args(state, Fixnum::from(0));
    cm->required_args(state, cm->total_args());

    cm->formalize(state);

    return cm;
  }

  void test_task_fields() {
    TS_ASSERT_EQUALS(0U, Task::fields);
  }

  void test_create() {
    Task* task = Task::create(state);

    TS_ASSERT(kind_of<Task>(task));
  }

  void test_create_trampoline() {
    Task* task = Task::create(state);

    TS_ASSERT_EQUALS(task->active()->name(), state->symbol("__trampoline__"));
    TS_ASSERT_EQUALS(task->active()->block(), Qnil);
    TS_ASSERT_EQUALS(task->active()->sender(), Qnil);
    TS_ASSERT(kind_of<MethodContext>(task->active()->home()));
    TS_ASSERT(kind_of<Object>(task->active()->self()));
    TS_ASSERT(kind_of<CompiledMethod>(task->active()->cm()));
    TS_ASSERT(kind_of<Module>(task->active()->module()));
  }

  void test_create_sets_probe() {
    TaskProbe* probe = TaskProbe::create(state);
    state->probe.set(probe);
    Task* task = Task::create(state);

    TS_ASSERT_EQUALS(probe, task->probe());
  }

  void test_current() {
    Task* task = Task::current(state);

    TS_ASSERT_EQUALS(task, state->globals.current_task.get());
  }

  void test_current_context() {
    Task* task = Task::current(state);

    TS_ASSERT_EQUALS(task->active(), task->current_context(state));
  }

  void task_get_control_channel() {
    Task* task = Task::current(state);

    Channel* control = task->get_control_channel(state);

    TS_ASSERT_EQUALS(task->control_channel(), control);
  }

  void task_get_debug_channel() {
    Task* task = Task::current(state);

    Channel* debug = task->get_debug_channel(state);

    TS_ASSERT_EQUALS(task->debug_channel(), debug);
  }

  void test_send_message() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state);

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.set_args(0);

    MethodContext* cur = task->active();

    task->send_message(msg);

    TS_ASSERT(cur != task->active());

    MethodContext* ncur = task->active();

    TS_ASSERT_EQUALS(ncur->self(), Qtrue);
    TS_ASSERT_EQUALS(ncur->sender(), cur);
  }

  void test_send_message_slowly() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state);

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = (SendSite*)Qnil;
    msg.set_args(0);

    MethodContext* cur = task->active();

    task->send_message_slowly(msg);

    TS_ASSERT(cur != task->active());

    MethodContext* ncur = task->active();

    TS_ASSERT_EQUALS(ncur->self(), Qtrue);
    TS_ASSERT_EQUALS(ncur->sender(), cur);
  }

  void test_send_message_sets_up_fixed_locals() {
    CompiledMethod* cm = create_cm();
    cm->required_args(state, Fixnum::from(2));
    cm->total_args(state, cm->required_args());
    cm->local_count(state, cm->required_args());
    cm->stack_size(state, cm->required_args());
    cm->splat(state, Qnil);

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Task* task = Task::create(state, 2);

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));

    MethodContext* input_context = task->active();

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.use_from_task(task, 2);

    task->send_message(msg);

    TS_ASSERT(task->active() != input_context);
    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->stack_at(1), Fixnum::from(4));
  }

  void test_send_message_throws_argerror_on_too_few() {
    CompiledMethod* cm = create_cm();
    cm->required_args(state, Fixnum::from(2));
    cm->total_args(state, cm->required_args());
    cm->local_count(state, cm->required_args());
    cm->stack_size(state, cm->required_args());
    cm->splat(state, Qnil);

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Task* task = Task::create(state, 2);

    MethodContext* top = task->active();

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.use_from_task(task, 1);

    bool thrown = false;
    try {
      task->send_message(msg);
    } catch(RubyException& error) {
      TS_ASSERT(Exception::argument_error_p(state, error.exception));
      TS_ASSERT_SAME_DATA("given 1, expected 2", error.exception->message()->c_str(), 19U);
      thrown = true;
    }

    TS_ASSERT(thrown);

    TS_ASSERT_EQUALS(task->active(), top);
  }

  void test_send_message_throws_argerror_on_too_many() {
    CompiledMethod* cm = create_cm();
    cm->required_args(state, Fixnum::from(2));
    cm->total_args(state, cm->required_args());
    cm->local_count(state, cm->required_args());
    cm->stack_size(state, cm->required_args());
    cm->splat(state, Qnil);

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Task* task = Task::create(state, 2);

    MethodContext* top = task->active();

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.use_from_task(task, 3);

    bool thrown = false;
    try {
      task->send_message(msg);
    } catch(RubyException& error) {
      TS_ASSERT(Exception::argument_error_p(state, error.exception));
      TS_ASSERT_SAME_DATA("given 3, expected 2", error.exception->message()->c_str(), 19U);
      thrown = true;
    }

    TS_ASSERT(thrown);

    TS_ASSERT_EQUALS(task->active(), top);
  }

  void test_send_message_sets_up_fixed_locals_with_optionals() {
    CompiledMethod* cm = create_cm();
    cm->required_args(state, Fixnum::from(2));
    cm->total_args(state, Fixnum::from(4));
    cm->local_count(state, cm->total_args());
    cm->stack_size(state, cm->total_args());
    cm->splat(state, Qnil);

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Task* task = Task::create(state, 4);

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));
    task->push(Fixnum::from(5));

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.use_from_task(task, 3);

    task->send_message(msg);

    TS_ASSERT(task->passed_arg_p(3));
    TS_ASSERT(!task->passed_arg_p(4));

    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->stack_at(1), Fixnum::from(4));
    TS_ASSERT_EQUALS(task->stack_at(2), Fixnum::from(5));
    TS_ASSERT_EQUALS(task->stack_at(3), Qnil);
  }

  void test_send_message_sets_up_fixed_locals_with_splat() {
    CompiledMethod* cm = create_cm();
    cm->required_args(state, Fixnum::from(2));
    cm->total_args(state, cm->required_args());
    cm->local_count(state, Fixnum::from(4));
    cm->stack_size(state, cm->local_count());
    cm->splat(state, Fixnum::from(2));

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Task* task = Task::create(state, 10);

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));
    task->push(Fixnum::from(5));
    task->push(Fixnum::from(6));

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.use_from_task(task, 4);

    task->send_message(msg);

    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->stack_at(1), Fixnum::from(4));

    Array* splat = as<Array>(task->stack_at(2));

    TS_ASSERT_EQUALS(splat->size(), 2U);
    TS_ASSERT_EQUALS(splat->get(state, 0), Fixnum::from(5));
    TS_ASSERT_EQUALS(splat->get(state, 1), Fixnum::from(6));
  }

  void test_send_message_sets_up_fixed_locals_with_optional_and_splat() {
    CompiledMethod* cm = create_cm();
    cm->required_args(state, Fixnum::from(1));
    cm->total_args(state, Fixnum::from(2));
    cm->local_count(state, Fixnum::from(4));
    cm->stack_size(state, cm->local_count());
    cm->splat(state, Fixnum::from(2));

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Task* task = Task::create(state, 10);

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));
    task->push(Fixnum::from(5));
    task->push(Fixnum::from(6));

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.use_from_task(task, 4);

    task->send_message(msg);

    TS_ASSERT_EQUALS(task->stack_at(0), Fixnum::from(3));
    TS_ASSERT_EQUALS(task->stack_at(1), Fixnum::from(4));

    Array* splat = as<Array>(task->stack_at(2));

    TS_ASSERT_EQUALS(splat->size(), 2U);
    TS_ASSERT_EQUALS(splat->get(state, 0), Fixnum::from(5));
    TS_ASSERT_EQUALS(splat->get(state, 1), Fixnum::from(6));
  }

  void test_send_message_does_not_throw_argerror_on_too_many_with_splat() {
    CompiledMethod* cm = create_cm();
    cm->required_args(state, Fixnum::from(2));
    cm->total_args(state, cm->required_args());
    cm->local_count(state, Fixnum::from(3));
    cm->stack_size(state, Fixnum::from(3));
    cm->splat(state, Fixnum::from(2));

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Task* task = Task::create(state, 10);

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));
    task->push(Fixnum::from(5));

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.use_from_task(task, 3);

    task->send_message(msg);
  }

  /**
   *  class Parent
   *    def callee
   *      1
   *    end
   *    protected :callee
   *  end
   *
   *  class Child < Parent
   *    def caller
   *      Parent.new.callee
   *    end
   *  end
   *
   *  Child.new.caller # => 1
   */
  void test_send_message_finds_protected_methods() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state);

    MethodVisibility* vis = MethodVisibility::create(state);
    vis->method(state, cm);
    vis->visibility(state, G(sym_protected));

    Class* parent = state->new_class("Parent", G(object), 1);
    Class* child =  state->new_class("Child", parent, 1);

    SYMBOL callee = state->symbol("callee");
    parent->method_table()->store(state, callee, vis);

    OBJECT p = state->new_object(parent);
    OBJECT c = state->new_object(child);
    task->self(state, c);

    StaticScope *sc = StaticScope::create(state);
    sc->module(state, child);
    cm->scope(state, sc);

    task->active()->module(state, child);
    task->active()->name(state, state->symbol("caller"));
    task->active()->self(state, task->self());

    Message msg(state);
    msg.recv = p;
    msg.lookup_from = parent;
    msg.name = callee;
    msg.send_site = SendSite::create(state, callee);
    msg.set_args(0);

    MethodContext* cur = task->active();

    task->send_message(msg);

    TS_ASSERT(cur != task->active());

    MethodContext* ncur = task->active();

    TS_ASSERT_EQUALS(ncur->self(), p);
    TS_ASSERT_EQUALS(ncur->sender(), cur);
  }

  /**
   *  class Parent
   *    def callee
   *      1
   *    end
   *    protected :callee
   *  end
   *
   *  class Child < Parent
   *    def caller
   *      Parent.new.callee
   *    end
   *  end
   *
   *  Child.new.caller # => 1
   */
  void test_send_message_slowly_finds_protected_methods() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state);

    MethodVisibility* vis = MethodVisibility::create(state);
    vis->method(state, cm);
    vis->visibility(state, G(sym_protected));

    Class* parent = state->new_class("Parent", G(object), 1);
    Class* child =  state->new_class("Child", parent, 1);

    SYMBOL callee = state->symbol("callee");
    parent->method_table()->store(state, callee, vis);

    OBJECT p = state->new_object(parent);
    OBJECT c = state->new_object(child);
    task->self(state, c);

    StaticScope *sc = StaticScope::create(state);
    sc->module(state, child);
    cm->scope(state, sc);

    task->active()->module(state, child);
    task->active()->name(state, state->symbol("caller"));
    task->active()->self(state, task->self());

    Message msg(state);
    msg.recv = p;
    msg.lookup_from = parent;
    msg.name = callee;
    msg.send_site = SendSite::create(state, callee);
    msg.set_args(0);

    MethodContext* cur = task->active();

    task->send_message_slowly(msg);

    TS_ASSERT(cur != task->active());

    MethodContext* ncur = task->active();

    TS_ASSERT_EQUALS(ncur->self(), p);
    TS_ASSERT_EQUALS(ncur->sender(), cur);
  }

  void test_simple_return() {
    CompiledMethod* cm = create_cm();
    cm->total_args(state, Fixnum::from(0));
    cm->stack_size(state, Fixnum::from(1));

    Task* task = Task::create(state, Qnil, cm);

    MethodContext* top = task->active();

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.set_args(0);

    MethodContext* before = task->active();

    task->send_message(msg);

    TS_ASSERT_DIFFERS(before, task->active());

    task->simple_return(Fixnum::from(3));
    TS_ASSERT_EQUALS(task->active(), before);
    TS_ASSERT_EQUALS(task->active(), top);
  }

  void test_locate_method_on() {
    CompiledMethod* cm = create_cm();
    cm->stack_size(state, Fixnum::from(1));

    Task* task = Task::create(state);

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    Tuple* tup = task->locate_method_on(Qtrue, state->symbol("blah"), Qfalse);

    TS_ASSERT_EQUALS(G(true_class), tup->at(state, 1));
    TS_ASSERT_EQUALS(cm,            tup->at(state, 0));
  }

  void test_locate_method_on_private() {
    CompiledMethod* cm = create_cm();
    cm->stack_size(state, Fixnum::from(1));

    Task* task = Task::create(state);

    MethodVisibility* vis = MethodVisibility::create(state);
    vis->method(state, cm);
    vis->visibility(state, G(sym_private));

    G(true_class)->method_table()->store(state, state->symbol("blah"), vis);

    Tuple* tup = task->locate_method_on(Qtrue, state->symbol("blah"), Qfalse);

    TS_ASSERT_EQUALS(Qnil, tup);
  }

  void test_locate_method_on_protected() {
    CompiledMethod* cm = create_cm();
    cm->stack_size(state, Fixnum::from(1));

    Task* task = Task::create(state);

    MethodVisibility* vis = MethodVisibility::create(state);
    vis->method(state, cm);
    vis->visibility(state, G(sym_protected));

    G(true_class)->method_table()->store(state, state->symbol("blah"), vis);

    Tuple* tup = task->locate_method_on(Qtrue, state->symbol("blah"), Qfalse);

    TS_ASSERT_EQUALS(Qnil, tup);
  }

  void test_locate_method_on_private_private_send() {
    CompiledMethod* cm = create_cm();
    cm->stack_size(state, Fixnum::from(1));

    Task* task = Task::create(state);

    MethodVisibility* vis = MethodVisibility::create(state);
    vis->method(state, cm);
    vis->visibility(state, G(sym_private));

    G(true_class)->method_table()->store(state, state->symbol("blah"), vis);

    Tuple* tup = task->locate_method_on(Qtrue, state->symbol("blah"), Qtrue);

    TS_ASSERT_EQUALS(G(true_class), tup->at(state, 1));
    TS_ASSERT_EQUALS(cm,            tup->at(state, 0));
  }

  void test_attach_method() {
    CompiledMethod* cm = create_cm();
    cm->stack_size(state, Fixnum::from(1));

    Task* task = Task::create(state);

    task->attach_method(Qtrue, state->symbol("blah"), cm);

    TS_ASSERT_EQUALS(cm, G(true_class)->method_table()->fetch(state, state->symbol("blah")));
  }

  void test_add_method() {
    CompiledMethod* cm = create_cm();
    cm->stack_size(state, Fixnum::from(1));

    Task* task = Task::create(state);

    SYMBOL blah = state->symbol("blah");

    state->global_cache->retain(state, G(true_class), blah, G(true_class), cm, false);
    task->add_method(G(true_class), state->symbol("blah"), cm);
    struct GlobalCache::cache_entry *ent;

    ent = state->global_cache->lookup(G(true_class), blah);
    TS_ASSERT(!ent);

    TS_ASSERT_EQUALS(cm, G(true_class)->method_table()->fetch(state, state->symbol("blah")));
  }

  void test_check_serial() {
    CompiledMethod* cm = create_cm();

    Task* task = Task::create(state);

    cm->serial(state, Fixnum::from(0));

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    TS_ASSERT( task->check_serial(Qtrue, state->symbol("blah"), 0));
    TS_ASSERT(!task->check_serial(Qtrue, state->symbol("blah"), 1));
  }

  void test_const_get_from_specific_module() {
    bool found;
    G(true_class)->set_const(state, "Number", Fixnum::from(3));
    Task* task = Task::create(state);

    TS_ASSERT_EQUALS(
        task->const_get(G(true_class), state->symbol("Number"), &found),
        Fixnum::from(3));

    TS_ASSERT(found);
  }

  void test_const_get_from_in_superclass() {
    bool found;
    G(object)->set_const(state, "Number", Fixnum::from(3));
    Task* task = Task::create(state);

    TS_ASSERT_EQUALS(
        task->const_get(G(true_class), state->symbol("Number"), &found),
        Fixnum::from(3));

    TS_ASSERT(found);
  }

  void test_const_get_from_module_in_object_are_not_found() {
    bool found;
    G(object)->set_const(state, "Number", Fixnum::from(3));
    Task* task = Task::create(state);

    Module* mod = state->new_module("Test");

    TS_ASSERT_EQUALS(
        task->const_get(mod, state->symbol("Number"), &found),
        Qnil);

    TS_ASSERT(!found);
  }

  void test_const_get_in_context() {
    bool found;
    Module* parent = state->new_module("Parent");
    Module* child =  state->new_module("Parent::Child");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    StaticScope* cs = StaticScope::create(state);
    cs->module(state, child);
    cs->parent(state, ps);

    CompiledMethod* cm = create_cm();

    Task* task = Task::create(state, Qnil, cm);
    cm->scope(state, cs);

    parent->set_const(state, "Number", Fixnum::from(3));
    child->set_const(state, "Name", state->symbol("blah"));

    TS_ASSERT_EQUALS(
        task->const_get(state->symbol("Number"), &found),
        Fixnum::from(3));

    TS_ASSERT(found);

    TS_ASSERT_EQUALS(
        task->const_get(state->symbol("Name"), &found),
        state->symbol("blah"));

    TS_ASSERT(found);
  }

  void test_const_get_in_context_uses_superclass_too() {
    bool found;
    Module* parent = state->new_module("Parent");
    Module* child =  state->new_module("Parent::Child");
    Module* inc =    state->new_module("Included");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    StaticScope* cs = StaticScope::create(state);
    cs->module(state, child);
    cs->parent(state, ps);

    inc->set_const(state, "Age", Fixnum::from(28));

    child->superclass(state, inc);

    CompiledMethod* cm = create_cm();

    Task* task = Task::create(state, Qnil, cm);
    cm->scope(state, cs);

    TS_ASSERT_EQUALS(
      task->const_get(state->symbol("Age"), &found),
      Fixnum::from(28));

    TS_ASSERT(found);
  }

  void test_const_get_in_context_checks_Object() {
    bool found;
    Module* parent = state->new_module("Parent");
    Module* child =  state->new_module("Parent::Child");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    StaticScope* cs = StaticScope::create(state);
    cs->module(state, child);
    cs->parent(state, ps);

    G(object)->set_const(state, "Age", Fixnum::from(28));

    CompiledMethod* cm = create_cm();

    Task* task = Task::create(state, Qnil, cm);
    cm->scope(state, cs);

    TS_ASSERT_EQUALS(
      task->const_get(state->symbol("Age"), &found),
      Fixnum::from(28));

    TS_ASSERT(found);
  }

  void test_const_set() {
    Module* parent = state->new_module("Parent");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    CompiledMethod* cm = create_cm();

    Task* task = Task::create(state, Qnil, cm);
    cm->scope(state, ps);

    task->const_set(parent, state->symbol("Age"), Fixnum::from(28));

    TS_ASSERT_EQUALS(parent->get_const(state, "Age"), Fixnum::from(28));
  }

  void test_const_set_under_scope() {
    Module* parent = state->new_module("Parent");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    CompiledMethod* cm = create_cm();

    Task* task = Task::create(state, Qnil, cm);
    cm->scope(state, ps);

    task->const_set(state->symbol("Age"), Fixnum::from(28));

    TS_ASSERT_EQUALS(parent->get_const(state, "Age"), Fixnum::from(28));
  }

  void test_yield_debugger() {
    Task* task = Task::create(state);
    bool thrown = false;
    try {
      task->yield_debugger();
    } catch(Assertion* e) {
      thrown = true;
    }

    TS_ASSERT(!thrown);
  }

  void test_current_module() {
    Module* parent = state->new_module("Parent");

    MethodContext* ctx = MethodContext::create(state, 10);
    ctx->home(state, ctx);
    ctx->module(state, parent);

    Task* task = Task::create(state, 10);
    task->active(state, ctx);

    TS_ASSERT_EQUALS(task->current_module(), parent);
  }

  void test_open_class() {
    Module* parent = state->new_module("Parent");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    CompiledMethod* cm = create_cm();

    Task* task = Task::create(state, Qnil, cm);
    cm->scope(state, ps);

    bool created;
    Class* cls = task->open_class(Qnil, state->symbol("Person"), &created);
    TS_ASSERT(created);
    TS_ASSERT(kind_of<Class>(cls));

    TS_ASSERT_EQUALS(cls->name(), state->symbol("Parent::Person"));
    TS_ASSERT_EQUALS(parent->get_const(state, "Person"), cls);
  }

  void test_open_class_under_specific_module() {
    Module* parent = state->new_module("Parent");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    CompiledMethod* cm = create_cm();

    Task* task = Task::create(state, Qnil, cm);
    cm->scope(state, ps);

    bool created;
    Class* cls = task->open_class(G(object), Qnil, state->symbol("Person"), &created);
    TS_ASSERT(created);
    TS_ASSERT(kind_of<Class>(cls));

    TS_ASSERT_EQUALS(cls->name(), state->symbol("Person"));
    TS_ASSERT_EQUALS(G(object)->get_const(state, "Person"), cls);
  }

  void test_open_module() {
    Module* parent = state->new_module("Parent");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    CompiledMethod* cm = create_cm();

    Task* task = Task::create(state, Qnil, cm);
    cm->scope(state, ps);

    Module* mod = task->open_module(state->symbol("Person"));
    TS_ASSERT(kind_of<Module>(mod));

    TS_ASSERT_EQUALS(mod->name(), state->symbol("Parent::Person"));
    TS_ASSERT_EQUALS(parent->get_const(state, "Person"), mod);
  }

  void test_open_module_under_specific_module() {
    Module* parent = state->new_module("Parent");

    StaticScope* ps = StaticScope::create(state);
    ps->module(state, parent);
    ps->parent(state, (StaticScope*)Qnil);

    CompiledMethod* cm = create_cm();

    Task* task = Task::create(state, Qnil, cm);
    cm->scope(state, ps);

    Module* mod = task->open_module(G(object), state->symbol("Person"));
    TS_ASSERT(kind_of<Module>(mod));

    TS_ASSERT_EQUALS(mod->name(), state->symbol("Person"));
    TS_ASSERT_EQUALS(G(object)->get_const(state, "Person"), mod);
  }

  void test_raise() {
    CompiledMethod* cm = create_cm();
    cm->iseq(state, InstructionSequence::create(state, 40));
    cm->total_args(state, Fixnum::from(0));
    cm->stack_size(state, Fixnum::from(1));
    cm->exceptions(state, Tuple::from(state, 1,
        Tuple::from(state, 3, Fixnum::from(0), Fixnum::from(3), Fixnum::from(5))));

    Task* task = Task::create(state, Qnil, cm);

    MethodContext* top = task->active();
    task->set_ip(3);

    /* Call a method ... */
    CompiledMethod* cm2 = create_cm();

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm2);

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.set_args(0);

    task->send_message(msg);
    TS_ASSERT(task->active() != top);

    Exception* exc = Exception::create(state);

    Task* t2 = task->raise(state, exc);

    TS_ASSERT_EQUALS(task, t2);
    TS_ASSERT_EQUALS(task->active(), top);
    TS_ASSERT_EQUALS(task->current_ip(), 5);
  }

  void test_raise_exception() {
    CompiledMethod* cm = create_cm();
    cm->iseq(state, InstructionSequence::create(state, 40));
    cm->total_args(state, Fixnum::from(0));
    cm->stack_size(state, Fixnum::from(1));
    cm->exceptions(state, Tuple::from(state, 1,
        Tuple::from(state, 3, Fixnum::from(0), Fixnum::from(3), Fixnum::from(5))));

    Task* task = Task::create(state, Qnil, cm);

    MethodContext* top = task->active();
    task->set_ip(3);

    /* Call a method ... */
    CompiledMethod* cm2 = create_cm();

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm2);

    Message msg(state);
    msg.recv = Qtrue;
    msg.lookup_from = G(true_class);
    msg.name = state->symbol("blah");
    msg.send_site = SendSite::create(state, state->symbol("blah"));
    msg.set_args(0);

    task->send_message(msg);
    TS_ASSERT(task->active() != top);

    Exception* exc = Exception::create(state);

    task->raise_exception(exc);

    TS_ASSERT_EQUALS(task->active(), top);
    TS_ASSERT_EQUALS(task->current_ip(), 5);
  }

  void test_raise_exception_into_sender() {
    CompiledMethod* cm = create_cm();
    cm->iseq(state, InstructionSequence::create(state, 40));
    cm->stack_size(state, Fixnum::from(1));
    cm->exceptions(state, Tuple::from(state, 1,
        Tuple::from(state, 3, Fixnum::from(0), Fixnum::from(3), Fixnum::from(5))));

    Task* task = Task::create(state, Qnil, cm);

    task->set_ip(3);

    Exception* exc = Exception::create(state);

    task->raise_exception(exc);

    TS_ASSERT_EQUALS(task->current_ip(), 5);
  }

  void test_check_interrupts() {
    Task* task = Task::create(state);

    task->check_interrupts();
    TS_ASSERT(!state->om->collect_young_now);
    TS_ASSERT(!state->om->collect_mature_now);
  }

  void test_old_contexts_are_remembered_on_activate() {
    Task* task = Task::create(state);
    TS_ASSERT(!task->active()->Remember);

    /* Evil, but lets us test this easy. Don't do this in real
     * code */
    task->active()->zone = MatureObjectZone;

    task->make_active(task->active());
    TS_ASSERT(task->active()->Remember);

    /* Check it only happens to old contexts. */
    task = Task::create(state);
    TS_ASSERT(!task->active()->Remember);
    task->make_active(task->active());
    TS_ASSERT(!task->active()->Remember);
  }

  void test_call_object() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state);

    G(true_class)->method_table()->store(state, state->symbol("blah"), cm);

    MethodContext* cur = task->active();

    task->call_object(state, Qtrue, state->symbol("blah"),
        Array::create(state, 0));

    TS_ASSERT(cur != task->active());

    MethodContext* ncur = task->active();

    TS_ASSERT_EQUALS(ncur->self(), Qtrue);
    TS_ASSERT_EQUALS(ncur->sender(), cur);
  }

};

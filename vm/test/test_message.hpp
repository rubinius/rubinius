#include "builtin/tuple.hpp"
#include "builtin/list.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/array.hpp"
#include "builtin/task.hpp"
#include "builtin/iseq.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/symbol.hpp"

#include "message.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestMessage : public CxxTest::TestSuite {
  public:

  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_append_arguments() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state, Qnil, cm);

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, state->symbol("blah"));
    ary->set(state, 1, state->symbol("foo"));

    Message msg(state);
    msg.name = state->symbol("testes");
    msg.use_from_task(task, 2);
    msg.append_arguments(state, ary);

    TS_ASSERT_EQUALS(state->symbol("blah"), msg.get_argument(0));
    TS_ASSERT_EQUALS(state->symbol("foo"),  msg.get_argument(1));
    TS_ASSERT_EQUALS(Fixnum::from(3),       msg.get_argument(2));
    TS_ASSERT_EQUALS(Fixnum::from(4),       msg.get_argument(3));

    TS_ASSERT_EQUALS(4U, msg.args());
  }

  void test_append_splat() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state, Qnil, cm);

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, state->symbol("blah"));
    ary->set(state, 1, state->symbol("foo"));

    Message msg(state);
    msg.name = state->symbol("testes");
    msg.use_from_task(task, 2);
    msg.append_splat(state, ary);

    TS_ASSERT_EQUALS(Fixnum::from(3),       msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(4),       msg.get_argument(1));
    TS_ASSERT_EQUALS(state->symbol("blah"), msg.get_argument(2));
    TS_ASSERT_EQUALS(state->symbol("foo"),  msg.get_argument(3));

    TS_ASSERT_EQUALS(4U, msg.args());
  }

  void test_get_argument_from_array() {
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Fixnum::from(3));
    ary->set(state, 1, Fixnum::from(4));

    Message msg(state, ary);

    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(4), msg.get_argument(1));
    TS_ASSERT_EQUALS(2U, msg.args());
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

  void test_unshift_argument() {
    Message msg(state);
    msg.name = state->symbol("testes");

    Task* task = Task::create(state, 10);
    task->push(Fixnum::from(3));
    msg.use_from_task(task, 1);

    TS_ASSERT_EQUALS(1U, msg.args());
    msg.unshift_argument(state, Fixnum::from(47));
    TS_ASSERT_EQUALS(2U, msg.args());

    TS_ASSERT_EQUALS(Fixnum::from(47), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(1));
  }

  void test_unshift_argument_into_splat() {
    Message msg(state);
    Array* ary = Array::create(state, 1);
    ary->set(state, 0, Fixnum::from(3));

    msg.use_array(ary);

    TS_ASSERT_EQUALS(1U, msg.args());
    msg.unshift_argument(state, Fixnum::from(47));
    TS_ASSERT_EQUALS(2U, msg.args());

    TS_ASSERT_EQUALS(Fixnum::from(47), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(1));
  }

  void test_unshit_argument_multiple_times() {
    Message msg(state);
    Task* task = Task::create(state, 10);
    task->push(Fixnum::from(3));
    msg.use_from_task(task, 1);

    TS_ASSERT_EQUALS(1U, msg.args());
    msg.unshift_argument(state, Fixnum::from(47));
    msg.unshift_argument(state, Fixnum::from(8));
    TS_ASSERT_EQUALS(3U, msg.args());

    TS_ASSERT_EQUALS(Fixnum::from(8), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(47), msg.get_argument(1));
    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(2));

  }

  void test_shift_argument() {
    Message msg(state);
    Task* task = Task::create(state, 10);
    task->push(Fixnum::from(3));
    task->push(Fixnum::from(47));
    msg.use_from_task(task, 2);

    TS_ASSERT_EQUALS(2U, msg.args());

    FIXNUM shifted = as<Fixnum>(msg.shift_argument(state));
    TS_ASSERT_EQUALS(shifted, Fixnum::from(3));
    TS_ASSERT_EQUALS(msg.args(), 1U);

    TS_ASSERT_EQUALS(msg.get_argument(0), Fixnum::from(47));
  }

  void test_shift_argument_then_unshift() {
    Message msg(state);
    Task* task = Task::create(state, 10);
    task->push(Fixnum::from(3));
    task->push(Fixnum::from(47));
    msg.use_from_task(task, 2);

    FIXNUM shifted = as<Fixnum>(msg.shift_argument(state));
    TS_ASSERT_EQUALS(shifted, Fixnum::from(3));
    TS_ASSERT_EQUALS(msg.get_argument(0), Fixnum::from(47));

    msg.unshift_argument(state, Fixnum::from(13));

    TS_ASSERT_EQUALS(msg.get_argument(0), Fixnum::from(13));
    TS_ASSERT_EQUALS(msg.get_argument(1), Fixnum::from(47));
  }

  void test_shift_argument_one_then_unshift_one() {
    Message msg(state);
    Task* task = Task::create(state, 10);
    task->push(Fixnum::from(3));
    task->push(state->symbol("to_int"));
    msg.use_from_task(task, 1);

    Symbol* shifted = as<Symbol>(msg.shift_argument(state));
    TS_ASSERT_EQUALS(shifted, state->symbol("to_int"));
    TS_ASSERT_EQUALS(0U, msg.args());

    msg.unshift_argument(state, shifted);

    TS_ASSERT_EQUALS(1U, msg.args());

    TS_ASSERT_EQUALS(msg.get_argument(0), shifted);
  }

  void test_shift_argument_one_then_unshift_two() {
    Message msg(state);
    Task* task = Task::create(state, 10);
    task->push(Fixnum::from(3));
    task->push(state->symbol("to_int"));
    msg.use_from_task(task, 1);

    Symbol* shifted = as<Symbol>(msg.shift_argument(state));
    TS_ASSERT_EQUALS(shifted, state->symbol("to_int"));
    TS_ASSERT_EQUALS(0U, msg.args());

    msg.unshift_argument2(state, shifted, Fixnum::from(47));

    TS_ASSERT_EQUALS(2U, msg.args());

    TS_ASSERT_EQUALS(msg.get_argument(0), shifted);
    TS_ASSERT_EQUALS(msg.get_argument(1), Fixnum::from(47));
  }

  void test_as_array() {
    Message msg(state);
    Task* task = Task::create(state, 10);
    SYMBOL sym = state->symbol("to_int");
    task->push(sym);
    msg.use_from_task(task, 1);

    Array* ary = msg.as_array(state);
    TS_ASSERT_EQUALS(ary->size(), 1U);
    TS_ASSERT_EQUALS(ary->get(state, 0), sym);

    msg.unshift_argument(state, Fixnum::from(3));

    ary = msg.as_array(state);
    TS_ASSERT_EQUALS(ary->size(), 2U);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(3));
    TS_ASSERT_EQUALS(ary->get(state, 1), sym);

    msg.shift_argument(state);

    ary = msg.as_array(state);
    TS_ASSERT_EQUALS(ary->size(), 1U);
    TS_ASSERT_EQUALS(ary->get(state, 0), sym);

  }
};

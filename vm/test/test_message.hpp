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

  void test_get_argument_from_array() {
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Fixnum::from(3));
    ary->set(state, 1, Fixnum::from(4));

    Message msg(state, ary);

    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(4), msg.get_argument(1));
    TS_ASSERT_EQUALS(2U, msg.args);
  }

  CompiledMethod* create_cm() {
    CompiledMethod* cm = CompiledMethod::create(state);
    cm->iseq = InstructionSequence::create(state, 1);
    cm->iseq->opcodes->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    cm->stack_size = Fixnum::from(10);
    cm->total_args = Fixnum::from(0);
    cm->required_args = cm->total_args;
    cm->formalize(state);

    return cm;
  }

  void test_import_arguments() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state, Qnil, cm);

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));

    Message msg(state);
    msg.import_arguments(state, task, 2);

    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(4), msg.get_argument(1));
    TS_ASSERT_EQUALS(2U, msg.args);
  }

  void test_combine_with_splat() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state, Qnil, cm);

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, state->symbol("blah"));
    ary->set(state, 1, state->symbol("foo"));

    Message msg(state);
    msg.args = 2;
    msg.combine_with_splat(state, task, ary);

    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(4), msg.get_argument(1));
    TS_ASSERT_EQUALS(state->symbol("blah"), msg.get_argument(2));
    TS_ASSERT_EQUALS(state->symbol("foo"), msg.get_argument(3));

    TS_ASSERT_EQUALS(4U, msg.args);
  }

  void test_use_from_task() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state, Qnil, cm);

    task->push(Fixnum::from(3));
    task->push(Fixnum::from(4));

    Message msg(state);
    msg.use_from_task(task, 2);

    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(4), msg.get_argument(1));
    TS_ASSERT_EQUALS(2U, msg.args);
  }

  void test_unshift_argument() {
    Message msg(state);
    Task* task = Task::create(state, 10);
    task->push(Fixnum::from(3));
    msg.use_from_task(task, 1);

    TS_ASSERT_EQUALS(1U, msg.args);
    msg.unshift_argument(state, Fixnum::from(47));
    TS_ASSERT_EQUALS(2U, msg.args);

    TS_ASSERT_EQUALS(Fixnum::from(47), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(1));
  }

  void test_unshift_argument_into_splat() {
    Message msg(state);
    Task* task = Task::create(state, 10);
    task->push(Fixnum::from(3));
    msg.import_arguments(state, task, 1);

    TS_ASSERT_EQUALS(1U, msg.args);
    msg.unshift_argument(state, Fixnum::from(47));
    TS_ASSERT_EQUALS(2U, msg.args);

    TS_ASSERT_EQUALS(Fixnum::from(47), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(1));
  }

  void test_unshit_argument_multiple_times() {
    Message msg(state);
    Task* task = Task::create(state, 10);
    task->push(Fixnum::from(3));
    msg.use_from_task(task, 1);

    TS_ASSERT_EQUALS(1U, msg.args);
    msg.unshift_argument(state, Fixnum::from(47));
    msg.unshift_argument(state, Fixnum::from(8));
    TS_ASSERT_EQUALS(3U, msg.args);

    TS_ASSERT_EQUALS(Fixnum::from(8), msg.get_argument(0));
    TS_ASSERT_EQUALS(Fixnum::from(47), msg.get_argument(1));
    TS_ASSERT_EQUALS(Fixnum::from(3), msg.get_argument(2));

  }
};

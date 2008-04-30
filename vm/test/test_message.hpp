#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestMessage : public CxxTest::TestSuite {
  public:

#undef state
  VM* state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_get_argument_from_array() {
    Array* ary = Array::create(state, 2);
    ary->set(state, 0, Object::i2n(3));
    ary->set(state, 1, Object::i2n(4));

    Message msg(state, ary);

    TS_ASSERT_EQUALS(Object::i2n(3), msg.get_argument(0));
    TS_ASSERT_EQUALS(Object::i2n(4), msg.get_argument(1));
    TS_ASSERT_EQUALS(2, msg.args);
  }

  CompiledMethod* create_cm() {
    CompiledMethod* cm = CompiledMethod::create(state);
    cm->iseq = ISeq::create(state, 40);
    cm->stack_size = Object::i2n(10);
    cm->total_args = Object::i2n(0);
    cm->required_args = cm->total_args;
    return cm;
  }

  void test_import_arguments() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state, Qnil, cm);

    task->stack = Tuple::from(state, 2, Object::i2n(3), Object::i2n(4));
    task->sp = 1;

    Message msg(state);
    msg.import_arguments(state, task, 2);

    TS_ASSERT_EQUALS(Object::i2n(3), msg.get_argument(0));
    TS_ASSERT_EQUALS(Object::i2n(4), msg.get_argument(1));
    TS_ASSERT_EQUALS(2, msg.args);
  }

  void test_combine_with_splat() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state, Qnil, cm);

    task->stack = Tuple::from(state, 2, Object::i2n(3), Object::i2n(4));
    task->sp = 1;

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, state->symbol("blah"));
    ary->set(state, 1, state->symbol("foo"));

    Message msg(state);
    msg.args = 2;
    msg.combine_with_splat(state, task, ary);
    
    TS_ASSERT_EQUALS(Object::i2n(3), msg.get_argument(0));
    TS_ASSERT_EQUALS(Object::i2n(4), msg.get_argument(1));
    TS_ASSERT_EQUALS(state->symbol("blah"), msg.get_argument(2));
    TS_ASSERT_EQUALS(state->symbol("foo"), msg.get_argument(3));

    TS_ASSERT_EQUALS(4, msg.args);
  }

  void test_use_from_task() {
    CompiledMethod* cm = create_cm();
    Task* task = Task::create(state, Qnil, cm);

    task->stack = Tuple::from(state, 2, Object::i2n(3), Object::i2n(4));
    task->sp = 1;

    Message msg(state);
    msg.use_from_task(task, 2);
    
    TS_ASSERT_EQUALS(Object::i2n(3), msg.get_argument(0));
    TS_ASSERT_EQUALS(Object::i2n(4), msg.get_argument(1));
    TS_ASSERT_EQUALS(2, msg.args);
  }
};

#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "builtin/access_variable.hpp"
#include "builtin/task.hpp"
#include "builtin/symbol.hpp"
#include "builtin/class.hpp"
#include "builtin/string.hpp"
#include "message.hpp"

#include <cxxtest/TestSuite.h>

#include <map>
#include <vector>

using namespace rubinius;

class TestAccessVariable : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_access_variable_fields() {
    TS_ASSERT_EQUALS(Executable::fields + 2U, AccessVariable::fields);
  }

  void test_access_variable_is_slot() {
    AccessVariable* av = AccessVariable::allocate(state);
    Task* task = Task::create(state, 10);
    Message msg(state);
    msg.recv = G(object);
    msg.method = av;
    msg.use_from_task(task, 0);
    av->name(state, state->symbol("@name"));

    TS_ASSERT(!av->execute(state, task, msg));
    Object* ret = task->pop();
    TS_ASSERT(try_as<Symbol>(ret));

    TS_ASSERT_EQUALS(std::string("Object"), as<Symbol>(ret)->c_str(state));
  }

  void test_write_variable_is_slot() {
    AccessVariable* av = AccessVariable::allocate(state);
    Task* task = Task::create(state, 10);
    Message msg(state);
    msg.recv = G(object);
    msg.method = av;
    msg.use_from_task(task, 0);
    av->name(state, state->symbol("@name"));
    av->write(state, Qtrue);

    Symbol* val = state->symbol("Blah");

    msg.unshift_argument(state, val);

    TS_ASSERT(!av->execute(state, task, msg));
    Object* ret = task->pop();
    TS_ASSERT_EQUALS(ret, val);

    TS_ASSERT_EQUALS(val, G(object)->name());
  }

  void test_access_variable() {
    AccessVariable* av = AccessVariable::allocate(state);
    Task* task = Task::create(state, 10);
    Message msg(state);
    msg.recv = G(object);
    msg.method = av;
    msg.use_from_task(task, 0);
    av->name(state, state->symbol("@blah"));

    G(object)->set_ivar(state, av->name(), state->symbol("Sweet"));

    TS_ASSERT(!av->execute(state, task, msg));
    Object* ret = task->pop();
    TS_ASSERT(try_as<Symbol>(ret));

    TS_ASSERT_EQUALS(std::string("Sweet"), as<Symbol>(ret)->c_str(state));
  }

  void test_write_variable() {
    AccessVariable* av = AccessVariable::allocate(state);
    Task* task = Task::create(state, 10);
    Message msg(state);
    msg.recv = G(object);
    msg.method = av;
    msg.use_from_task(task, 0);
    av->name(state, state->symbol("@blah"));
    av->write(state, Qtrue);

    Symbol* val = state->symbol("Blah");

    msg.unshift_argument(state, val);

    TS_ASSERT(!av->execute(state, task, msg));
    Object* ret = task->pop();
    TS_ASSERT_EQUALS(ret, val);

    Symbol* out = as<Symbol>(G(object)->get_ivar(state, av->name()));
    TS_ASSERT_EQUALS(val, out);
  }


};

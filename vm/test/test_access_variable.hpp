#include "vm/test/test.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/class.hpp"
#include "builtin/string.hpp"
#include "message.hpp"

#include <cxxtest/TestSuite.h>

#include <map>
#include <vector>

using namespace rubinius;

class TestAccessVariable : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_access_variable_is_slot() {
    AccessVariable* av = AccessVariable::allocate(state);
    CallFrame cf;

    Message msg(state);
    msg.recv = G(object);
    msg.method = av;
    av->name(state, state->symbol("@name"));

    Object* ret = av->access_execute(state, &cf, msg);
    TS_ASSERT(try_as<Symbol>(ret));

    TS_ASSERT_EQUALS(std::string("Object"), as<Symbol>(ret)->c_str(state));
  }

  void test_write_variable_is_slot() {
    AccessVariable* av = AccessVariable::allocate(state);

    CallFrame cf;
    Message msg(state);
    msg.recv = G(object);
    msg.method = av;

    av->name(state, state->symbol("@name"));
    av->write(state, Qtrue);

    Symbol* val = state->symbol("Blah");

    msg.unshift_argument(state, val);

    Object* ret = av->access_execute(state, &cf, msg);
    TS_ASSERT_EQUALS(ret, val);

    TS_ASSERT_EQUALS(val, G(object)->name());
  }

  void test_access_variable() {
    AccessVariable* av = AccessVariable::allocate(state);

    CallFrame cf;
    Message msg(state);
    msg.recv = G(object);
    msg.method = av;

    av->name(state, state->symbol("@blah"));

    G(object)->set_ivar(state, av->name(), state->symbol("Sweet"));

    Object* ret = av->execute(state, &cf, msg);
    TS_ASSERT(try_as<Symbol>(ret));

    TS_ASSERT_EQUALS(std::string("Sweet"), as<Symbol>(ret)->c_str(state));
  }

  void test_write_variable() {
    AccessVariable* av = AccessVariable::allocate(state);
    CallFrame cf;

    Message msg(state);
    msg.recv = G(object);
    msg.method = av;

    av->name(state, state->symbol("@blah"));
    av->write(state, Qtrue);

    Symbol* val = state->symbol("Blah");

    msg.unshift_argument(state, val);

    Object* ret = av->execute(state, &cf, msg);
    TS_ASSERT_EQUALS(ret, val);

    Symbol* out = as<Symbol>(G(object)->get_ivar(state, av->name()));
    TS_ASSERT_EQUALS(val, out);
  }


};

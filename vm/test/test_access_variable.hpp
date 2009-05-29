#include "vm/test/test.hpp"

#include "builtin/access_variable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/class.hpp"
#include "builtin/string.hpp"
#include "call_frame.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"

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

    Dispatch dis(state->symbol("blah"), G(object), av);
    Arguments args(G(object), 0, 0);

    av->name(state, state->symbol("@module_name"));

    Object* ret = av->access_execute(state, &cf, dis, args);
    TS_ASSERT(try_as<Symbol>(ret));

    TS_ASSERT_EQUALS(std::string("Object"), as<Symbol>(ret)->c_str(state));
  }

  void test_write_variable_is_slot() {
    AccessVariable* av = AccessVariable::allocate(state);

    CallFrame cf;
    Dispatch dis(state->symbol("blah"), G(object), av);
    Symbol* val = state->symbol("Blah");
    Object* ary[1] = {val};
    Arguments args(G(object), 1, ary);

    av->name(state, state->symbol("@module_name"));
    av->write(state, Qtrue);

    Object* ret = av->access_execute(state, &cf, dis, args);
    TS_ASSERT_EQUALS(ret, val);

    TS_ASSERT_EQUALS(val, G(object)->name());
  }

  void test_access_variable() {
    AccessVariable* av = AccessVariable::allocate(state);

    CallFrame cf;
    Dispatch dis(state->symbol("blah"), G(object), av);
    Arguments args(G(object), 0, 0);

    av->name(state, state->symbol("@blah"));

    G(object)->set_ivar(state, av->name(), state->symbol("Sweet"));

    Object* ret = av->execute(state, &cf, dis, args);
    TS_ASSERT(try_as<Symbol>(ret));

    TS_ASSERT_EQUALS(std::string("Sweet"), as<Symbol>(ret)->c_str(state));
  }

  void test_write_variable() {
    AccessVariable* av = AccessVariable::allocate(state);
    CallFrame cf;

    Dispatch dis(state->symbol("blah"), G(object), av);
    Symbol* val = state->symbol("Blah");
    Object* ary[1] = {val};
    Arguments args(G(object), 1, ary);

    av->name(state, state->symbol("@blah"));
    av->write(state, Qtrue);

    Object* ret = av->execute(state, &cf, dis, args);
    TS_ASSERT_EQUALS(ret, val);

    Symbol* out = as<Symbol>(G(object)->get_ivar(state, av->name()));
    TS_ASSERT_EQUALS(val, out);
  }


};

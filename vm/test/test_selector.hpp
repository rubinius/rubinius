#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestSelector : public CxxTest::TestSuite {
  public:

#undef state
  VM* state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_init() {
    TS_ASSERT(G(selector)->kind_of_p(state, G(klass)));
    TS_ASSERT_EQUALS(G(selector)->instance_type->n2i(), SelectorType);

    TS_ASSERT(G(selector)->get_const(state, "ALL")->kind_of_p(state, G(lookuptable)));
  }

  void test_create() {
    OBJECT sym = state->symbol("blah");
    Selector* sel = Selector::create(state, sym);
    TS_ASSERT_EQUALS(sel->name, sym);
    TS_ASSERT(sel->send_sites->kind_of_p(state, G(array)));
  }

  void test_lookup() {
    Selector* sel = Selector::lookup(state, state->symbol("foo"));
    TS_ASSERT_EQUALS(sel, Selector::lookup(state, state->symbol("foo")));
  }

  void test_clear() {
    Selector* sel = Selector::lookup(state, state->symbol("foo"));
    SendSite* ss = SendSite::create(state, sel->name);

    sel->associate(state, ss);
    TS_ASSERT(ss->basic_p(state));
    ss->specialized = true;
    TS_ASSERT(!ss->basic_p(state));
    sel->clear(state);
    TS_ASSERT(ss->basic_p(state));
  }

  void test_clear_by_name() {
    Selector* sel = Selector::lookup(state, state->symbol("foo"));
    SendSite* ss = SendSite::create(state, sel->name);

    sel->associate(state, ss);
    TS_ASSERT(ss->basic_p(state));
    ss->specialized = true;
    TS_ASSERT(!ss->basic_p(state));
    Selector::clear_by_name(state, state->symbol("foo"));
    TS_ASSERT(ss->basic_p(state));

  }

};

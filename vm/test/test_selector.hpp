#include "vm/test/test.hpp"

#include "builtin/selector.hpp"
#include "builtin/list.hpp"

class TestSelector : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_init() {
    TS_ASSERT(G(selector)->kind_of_p(state, G(klass)));
    TS_ASSERT_EQUALS(G(selector)->instance_type()->to_native(), SelectorType);

    TS_ASSERT(G(selector)->get_const(state, "ALL")->kind_of_p(state, G(lookuptable)));
  }

  void test_create() {
    Object* sym = state->symbol("blah");
    Selector* sel = Selector::create(state, sym);
    TS_ASSERT_EQUALS(sel->name(), sym);
    TS_ASSERT(sel->send_sites()->kind_of_p(state, G(array)));
  }

  void test_lookup() {
    Selector* sel = Selector::lookup(state, state->symbol("foo"));
    TS_ASSERT_EQUALS(sel, Selector::lookup(state, state->symbol("foo")));
  }

  void test_clear() {
    Selector* sel = Selector::lookup(state, state->symbol("foo"));
    SendSite* ss = SendSite::create(state, sel->name());
    MethodResolver fake = (MethodResolver)Qnil;

    sel->associate(state, ss);
    ss->resolver = fake;
    TS_ASSERT_EQUALS(fake, ss->resolver);
    sel->clear(state);
    TS_ASSERT_EQUALS(MonomorphicInlineCacheResolver::resolve, ss->resolver);
  }

  void test_clear_by_name() {
    Selector* sel = Selector::lookup(state, state->symbol("foo"));
    SendSite* ss = SendSite::create(state, sel->name());
    MethodResolver fake = (MethodResolver)Qnil;

    sel->associate(state, ss);
    ss->resolver = fake;
    TS_ASSERT_EQUALS(fake, ss->resolver);
    Selector::clear_by_name(state, state->symbol("foo"));
    TS_ASSERT_EQUALS(MonomorphicInlineCacheResolver::resolve, ss->resolver);
  }
};

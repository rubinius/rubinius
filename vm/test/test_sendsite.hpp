#include "vm/test/test.hpp"

#include "global_cache.hpp"

#include "builtin/list.hpp"

class TestSendSite : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_init() {
    TS_ASSERT(G(send_site)->kind_of_p(state, G(klass)));
    TS_ASSERT_EQUALS(G(send_site)->instance_type()->to_native(), SendSiteType);
  }

  void test_create() {
    Object* sym = state->symbol("blah");
    SendSite* ss = SendSite::create(state, sym);
    TS_ASSERT_EQUALS(ss->name(), sym);
    TS_ASSERT_EQUALS(ss->sender(), Qnil);
    
    Selector* sel = Selector::lookup(state, sym);
    TS_ASSERT_EQUALS(ss->selector(), sel);

    TS_ASSERT(sel->includes_p(state, ss));

    TS_ASSERT_EQUALS(ss->method(), Qnil);
    TS_ASSERT_EQUALS(ss->module(), Qnil);
    TS_ASSERT_EQUALS(ss->recv_class(), Qnil);
  }

  void test_initialize() {
    Object* sym = state->symbol("blah");
    SendSite* ss = SendSite::create(state, sym);
    CompiledMethod* cm = CompiledMethod::create(state);

    ss->recv_class(state, G(object));
    ss->method(state, cm);
    ss->module(state, G(object)->metaclass(state));

    ss->initialize(state);
    TS_ASSERT(ss->recv_class()->nil_p());
    TS_ASSERT(ss->method()->nil_p());
    TS_ASSERT(ss->module()->nil_p());
  }

  void test_set_sender() {
    Object* sym = state->symbol("blah");
    SendSite* ss = SendSite::create(state, sym);
    TS_ASSERT_EQUALS(ss->sender(), Qnil);

    CompiledMethod* cm = CompiledMethod::create(state);
    TS_ASSERT_EQUALS(ss->set_sender(state, cm), Qnil);
    TS_ASSERT_EQUALS(ss->sender(), cm);
  }

  void test_misses_prim() {
    Symbol* sym = state->symbol("blah");
    SendSite* ss = SendSite::create(state, sym);

    ss->misses = 10;

    TS_ASSERT_EQUALS(Fixnum::from(10), (Fixnum*)ss->misses_prim(state));
  }

  void test_hits_prim() {
    Symbol* sym = state->symbol("blah");
    SendSite* ss = SendSite::create(state, sym);

    ss->hits = 10;

    TS_ASSERT_EQUALS(Fixnum::from(10), (Fixnum*)ss->hits_prim(state));
  }
};

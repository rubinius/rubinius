#include "builtin/list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestSendSite : public CxxTest::TestSuite {
  public:

  VM* state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_init() {
    TS_ASSERT(G(send_site)->kind_of_p(state, G(klass)));
    TS_ASSERT_EQUALS(G(send_site)->instance_type()->to_native(), SendSiteType);
  }

  void test_create() {
    OBJECT sym = state->symbol("blah");
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
    OBJECT sym = state->symbol("blah");
    SendSite* ss = SendSite::create(state, sym);
    CompiledMethod* cm = CompiledMethod::create(state);
    MethodResolver fake = (MethodResolver)Qnil;

    ss->resolver = fake;
    ss->recv_class(state, G(object));
    ss->method(state, cm);
    ss->module(state, G(object)->metaclass(state));
    TS_ASSERT_EQUALS(fake, ss->resolver);

    ss->initialize(state);
    TS_ASSERT_EQUALS(MonomorphicInlineCacheResolver::resolve, ss->resolver);
    TS_ASSERT(ss->recv_class()->nil_p());
    TS_ASSERT(ss->method()->nil_p());
    TS_ASSERT(ss->module()->nil_p());
  }

  void test_set_sender() {
    OBJECT sym = state->symbol("blah");
    SendSite* ss = SendSite::create(state, sym);
    TS_ASSERT_EQUALS(ss->sender(), Qnil);

    CompiledMethod* cm = CompiledMethod::create(state);
    TS_ASSERT_EQUALS(ss->set_sender(state, cm), Qnil);
    TS_ASSERT_EQUALS(ss->sender(), cm);
  }

  void test_mono_inline_cache_locate_failed() {
    Message msg(state);
    SYMBOL sym = state->symbol("blah");
    SendSite* ss = SendSite::create(state, sym);
    Class* meta = G(object)->metaclass(state);
    CompiledMethod* cm = CompiledMethod::create(state);

    TS_ASSERT_EQUALS(MonomorphicInlineCacheResolver::resolve, ss->resolver);

    msg.name = sym;
    msg.lookup_from = meta;
    msg.recv = G(object);
    msg.current_self = G(object);
    msg.send_site = ss;
    msg.module = (Module*)Qnil;
    msg.method = (Executable*)Qnil;

    state->global_cache->retain(state, meta, sym, meta, cm);

    TS_ASSERT(ss->locate(state, msg));
    TS_ASSERT_EQUALS(1U, ss->misses);
    TS_ASSERT_EQUALS(0, ss->hits);
    TS_ASSERT_EQUALS(cm, msg.method);
    TS_ASSERT_EQUALS(meta, msg.module);
  }

  void test_mono_inline_cache_locate_succeeded() {
    Message msg(state);
    SYMBOL sym = state->symbol("blah");
    SendSite* ss = SendSite::create(state, sym);
    Class* meta = G(object)->metaclass(state);
    CompiledMethod* cm = CompiledMethod::create(state);
    CompiledMethod* g_cm = CompiledMethod::create(state);

    TS_ASSERT_EQUALS(MonomorphicInlineCacheResolver::resolve, ss->resolver);

    ss->recv_class(state, meta);
    ss->module(state, G(object));
    ss->method(state, cm);
    ss->hits = 1U;

    msg.name = sym;
    msg.lookup_from = meta;
    msg.recv = G(object);
    msg.current_self = G(object);
    msg.send_site = ss;
    msg.module = meta;
    msg.method = cm;

    state->global_cache->retain(state, meta, sym, meta, g_cm);

    TS_ASSERT(ss->locate(state, msg));
    TS_ASSERT_EQUALS(2U, ss->hits);
    TS_ASSERT_EQUALS(0, ss->misses);
    TS_ASSERT_EQUALS(cm, msg.method);
    TS_ASSERT_EQUALS(G(object), msg.module);
  }
};

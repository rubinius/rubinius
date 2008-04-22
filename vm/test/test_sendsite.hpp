#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestSendSite : public CxxTest::TestSuite {
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
    TS_ASSERT(G(send_site)->kind_of_p(state, G(klass)));
    TS_ASSERT_EQUALS(G(send_site)->instance_type->n2i(), SendSiteType);
  }

  void test_create() {
    OBJECT sym = state->symbol("blah");
    SendSite* ss = SendSite::create(state, sym);
    TS_ASSERT_EQUALS(ss->name, sym);
    TS_ASSERT_EQUALS(ss->sender, Qnil);
    
    Selector* sel = Selector::lookup(state, sym);
    TS_ASSERT_EQUALS(ss->selector, sel);

    TS_ASSERT(sel->includes_p(state, ss));

  }
};

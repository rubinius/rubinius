#include "prelude.hpp"
#include "vm.hpp"
#include "profiler.hpp"

#include "builtin/symbol.hpp"

class TestProfiler : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_enter_method() {
    Symbol* meth = state->symbol("blah");
    Symbol* klass = state->symbol("Sweet");

    profiler::Profiler prof;

    prof.enter_method(meth, klass);
    TS_ASSERT_EQUALS(prof.depth(), 1U);
    TS_ASSERT_EQUALS(prof.number_of_entries(), 1U);

    profiler::Key key(meth, klass);
    profiler::Method* mo = prof.find_key(key);
    TS_ASSERT(mo);

    TS_ASSERT_EQUALS(mo->container(), klass);
    TS_ASSERT_EQUALS(mo->method(), meth);
    TS_ASSERT_EQUALS(mo->total_time(), 0ULL);

    prof.enter_method(meth, klass);
    TS_ASSERT_EQUALS(prof.number_of_entries(), 1U);
    TS_ASSERT_EQUALS(prof.depth(), 2U);

    Symbol* meth2 = state->symbol("woo");
    prof.enter_method(meth2, klass);
    TS_ASSERT_EQUALS(prof.number_of_entries(), 2U);

    TS_ASSERT_EQUALS(prof.depth(), 3U);
  }

  void test_leave_method() {
    Symbol* meth = state->symbol("blah");
    Symbol* klass = state->symbol("Sweet");

    profiler::Profiler prof;

    prof.enter_method(meth, klass);
    profiler::Key key(meth, klass);
    profiler::Method* mo = prof.find_key(key);
    TS_ASSERT_EQUALS(mo->total_time(), 0ULL);

    usleep(10000);
    prof.leave_method();

    TS_ASSERT_EQUALS(prof.depth(), 0U);
    TS_ASSERT(mo->total_time() > 100);
  }

  void test_leave_method_adds_leaves() {
    Symbol* meth = state->symbol("blah");
    Symbol* klass = state->symbol("Sweet");

    profiler::Profiler prof;

    profiler::Method* outer = prof.enter_method(meth, klass);
    TS_ASSERT_EQUALS(prof.current_method(), outer);

    Symbol* meth2 = state->symbol("fun");

    profiler::Method* inner = prof.enter_method(meth2, klass);
    TS_ASSERT_EQUALS(prof.current_method(), inner);

    prof.leave_method();

    prof.leave_method();

    profiler::Key key(meth, klass);
    profiler::Method* mo = prof.find_key(key);

    TS_ASSERT_EQUALS(mo->number_of_leaves(), 1U);
    profiler::Leaves::iterator i = mo->leaves_begin();
    profiler::Leaf* leaf = i->second;

    profiler::Key key2(meth2, klass);
    profiler::Method* mo2 = prof.find_key(key2);

    TS_ASSERT_EQUALS(leaf->method(), mo2);

    TS_ASSERT(leaf->total_time() > 0);
    TS_ASSERT_EQUALS(leaf->total_time(), mo2->total_time());
  }

  void test_results() {
    Symbol* meth = state->symbol("blah");
    Symbol* meth2 = state->symbol("foo");
    Symbol* meth3 = state->symbol("done");
    Symbol* klass = state->symbol("Sweet");

    profiler::Profiler prof;

    profiler::Method* top = prof.enter_method(meth, klass);
    prof.enter_method(meth2, klass);
    prof.leave_method();

    TS_ASSERT_EQUALS(prof.current_method(), top);
    prof.enter_method(meth3, klass);
    prof.leave_method();

    TS_ASSERT_EQUALS(prof.current_method(), top);

    prof.enter_method(meth2, klass);
    prof.enter_method(meth3, klass);
    prof.leave_method();
    prof.leave_method();

    prof.leave_method();

    LookupTable* results = prof.results(state);

    TS_ASSERT(!results->nil_p());
  }
};

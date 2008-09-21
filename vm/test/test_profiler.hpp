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
    SYMBOL meth = state->symbol("blah");
    SYMBOL klass = state->symbol("Sweet");

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

    SYMBOL meth2 = state->symbol("woo");
    prof.enter_method(meth2, klass);
    TS_ASSERT_EQUALS(prof.number_of_entries(), 2U);

    TS_ASSERT_EQUALS(prof.depth(), 3U);
  }

  void test_leave_method() {
    SYMBOL meth = state->symbol("blah");
    SYMBOL klass = state->symbol("Sweet");

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
    SYMBOL meth = state->symbol("blah");
    SYMBOL klass = state->symbol("Sweet");

    profiler::Profiler prof;

    profiler::Method* outer = prof.enter_method(meth, klass);
    TS_ASSERT_EQUALS(prof.current_method(), outer);

    SYMBOL meth2 = state->symbol("fun");

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

  void test_print_results() {
    SYMBOL meth = state->symbol("blah");
    SYMBOL meth2 = state->symbol("foo");
    SYMBOL meth3 = state->symbol("done");
    SYMBOL klass = state->symbol("Sweet");

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

    std::stringstream stream;
    prof.print_results(state, stream);

    // This is a bullshit test, but i'm too lazy to test the entire
    // form of the XML for now.
    TS_ASSERT(stream.str().find("profile") != std::string::npos);
  }
};

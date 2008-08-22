#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "gc_debug.hpp"

#include <cxxtest/TestSuite.h>

#include <map>
#include <vector>

using namespace rubinius;

class TestVM : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM();
  }

  void tearDown() {
    delete state;
  }

  void test_symbol_given_cstr() {
    SYMBOL sym1 = state->symbol("blah");
    SYMBOL sym2 = state->symbol("blah");

    TS_ASSERT_EQUALS(sym1, sym2);
  }

  void test_symbol_given_string() {
    String* str1 = String::create(state, "symbolic");
    String* str2 = String::create(state, "symbolic");

    SYMBOL sym1 = state->symbol(str1);
    SYMBOL sym2 = state->symbol(str2);

    TS_ASSERT_EQUALS(sym1, sym2);
  }

  void test_symbol_given_std_string() {
    std::string str1("standard");
    std::string str2("standard");

    SYMBOL sym1 = state->symbol(str1);
    SYMBOL sym2 = state->symbol(str2);

    TS_ASSERT_EQUALS(sym1, sym2);
  }

  void test_new_object_uses_field_count_from_class() {
    Class* cls = state->new_class("Blah", G(object), 3);

    OBJECT blah = state->new_object(cls);

    TS_ASSERT_EQUALS(blah->field_count, 3U);
  }

  void test_globals() {
    TS_ASSERT_EQUALS(state->globals.roots.size(), 122U);
  }

  void test_collection() {
    std::map<int, OBJECT> objs;

    int index = 0;
    Roots::iterator i;
    for(i = state->globals.roots.begin(); i != state->globals.roots.end(); i++) {
      OBJECT tmp = (*i)->get();
      if(tmp->reference_p() && tmp->zone == YoungObjectZone) {
        objs[index] = tmp;
      }
      index++;
    }

    //std::cout << "young: " << index << " (" <<
    //  state->om->young.total_objects << ")" << std::endl;

    state->om->collect_young(state->globals.roots);

    index = 0;
    for(i = state->globals.roots.begin(); i != state->globals.roots.end(); i++) {
      if(OBJECT tmp = objs[index]) {
        TS_ASSERT((*i)->get() != tmp);
      }
      index++;
    }

    HeapDebug hd(state->om);
    hd.walk(state->globals.roots);

    //std::cout << "total: " << hd.seen_objects << " (" <<
    //  state->om->young.total_objects << ")" << std::endl;
  }

  void test_current_thread() {
    OBJECT current_thread = state->globals.current_thread.get();

    TS_ASSERT_EQUALS(state->current_thread(), current_thread);
  }

  void test_run_best_thread() {
    Thread* cur = Thread::current(state);
    Thread* low = Thread::create(state);

    low->priority = Fixnum::from(0);

    cur->wakeup(state);
    low->wakeup(state);

    state->run_best_thread();

    TS_ASSERT_EQUALS(cur, Thread::current(state));
  }

};

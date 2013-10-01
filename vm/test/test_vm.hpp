#include "vm/test/test.hpp"

#include "vm.hpp"
#include "object_memory.hpp"
#include "gc/debug.hpp"

#include <cxxtest/TestSuite.h>

#include <map>
#include <vector>

using namespace rubinius;

class TestVM : public CxxTest::TestSuite, public VMTest {
  public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_symbol_given_cstr() {
    Symbol* sym1 = state->symbol("blah");
    Symbol* sym2 = state->symbol("blah");

    TS_ASSERT_EQUALS(sym1, sym2);
  }

  void test_symbol_given_string() {
    String* str1 = String::create(state, "symbolic");
    String* str2 = String::create(state, "symbolic");

    Symbol* sym1 = state->symbol(str1);
    Symbol* sym2 = state->symbol(str2);

    TS_ASSERT_EQUALS(sym1, sym2);
  }

  void test_collection() {
    std::map<int, Object*> objs;

    int index = 0;
    Root* root = static_cast<Root*>(state->globals().roots.head());
    while(root) {
      Object* tmp = root->get();
      if(tmp->reference_p() && tmp->young_object_p()) {
        objs[index] = tmp;
      }
      index++;

      root = static_cast<Root*>(root->next());
    }

    //std::cout << "young: " << index << " (" <<
    //  state->om->young.total_objects << ")" << std::endl;

    GCData gc_data(state->vm());
    state->memory()->collect_young(state, &gc_data);

    index = 0;
    root = static_cast<Root*>(state->globals().roots.head());
    while(root) {
      if(Object* tmp = objs[index]) {
        TS_ASSERT(root->get() != tmp);
      }
      index++;

      root = static_cast<Root*>(root->next());
    }

    HeapDebug hd(state->memory());
    hd.walk(state->globals().roots);

    //std::cout << "total: " << hd.seen_objects << " (" <<
    //  state->om->young.total_objects << ")" << std::endl;
  }
};

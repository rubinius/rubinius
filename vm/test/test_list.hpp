#include "objects.hpp"
#include "builtin_list.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestList : public CxxTest::TestSuite {
  public:

#undef state
  VM* state;
  OBJECT one, two;

  void setUp() {
    state = new VM(1024);
    one = Object::i2n(1);
    two = Object::i2n(2);
  }

  void tearDown() {
    delete state;
  }

  void test_classes() {
    TS_ASSERT(state->globals.list->kind_of_p(state->globals.klass));
    TS_ASSERT(state->globals.list_node->kind_of_p(state->globals.klass));
  }

  void test_create() {
    List* lst = List::create(state);
    TS_ASSERT(lst->kind_of_p(state->globals.list));
  }

  void test_append() {
    List* lst = List::create(state);
    TS_ASSERT_EQUALS(lst->size(), 0);
    lst->append(state, one);
    TS_ASSERT_EQUALS(lst->locate(state, 0), one);
    TS_ASSERT_EQUALS(lst->size(), 1);

    lst->append(state, one);
    TS_ASSERT_EQUALS(lst->locate(state, 1), one);
    TS_ASSERT_EQUALS(lst->size(), 2);
    
    lst->append(state, two);
    TS_ASSERT_EQUALS(lst->locate(state, 2), two);
    TS_ASSERT_EQUALS(lst->size(), 3);
  }

  void test_shift() {
    List* lst = List::create(state);
    lst->append(state, one);
    lst->append(state, two);

    TS_ASSERT_EQUALS(lst->shift(state), one);
    TS_ASSERT_EQUALS(lst->size(), 1);
    
    TS_ASSERT_EQUALS(lst->shift(state), two);
    TS_ASSERT_EQUALS(lst->size(), 0);
  }

  void test_remove() {
    List* lst = List::create(state);
    lst->append(state, one);
    lst->append(state, two);
    lst->append(state, one);
    lst->append(state, two);

    TS_ASSERT_EQUALS(lst->remove(state, one), 2);
    TS_ASSERT_EQUALS(lst->size(), 2);
    
    TS_ASSERT_EQUALS(lst->remove(state, two), 2);
    TS_ASSERT_EQUALS(lst->size(), 0);
  }

};

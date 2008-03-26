#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestObject : public CxxTest::TestSuite {
  public:

#undef state
  VM *state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_ui2n() {
    TS_ASSERT_EQUALS(Object::ui2n(state, 93)->n2i(), 93);
  }

  void test_dup() {
    Tuple* tup = Tuple::create(state, 1);
    tup->put(state, 0, Qtrue);

    Tuple* tup2 = (Tuple*)tup->dup(state);

    TS_ASSERT_EQUALS(tup2->at(0), Qtrue);
  }

  void test_dup_bytes() {
    OBJECT obj = state->om->new_object(Qnil, 1);
    obj->StoresBytes = 1;

    obj->bytes[0] = 8;

    OBJECT obj2 = obj->dup(state);

    TS_ASSERT(obj2->stores_bytes_p());
    TS_ASSERT_EQUALS(obj2->bytes[0], 8);
  }

  void test_kind_of_p() {
    String* str = String::create(state, "blah");

    TS_ASSERT(str->kind_of_p(state->globals.string));
    TS_ASSERT(!str->kind_of_p(state->globals.tuple));
  }

  void test_hash() {
    TS_ASSERT(Object::i2n(8)->hash(state) > 0);
    TS_ASSERT(Object::i2n(-8)->hash(state) > 0);
  }

  void test_metaclass() {
    TS_ASSERT(MetaClass::is_a(state->globals.object->metaclass(state)));
    TS_ASSERT_EQUALS(Qnil->metaclass(state), state->globals.nil_class);
    TS_ASSERT_EQUALS(Qtrue->metaclass(state), state->globals.true_class);
    TS_ASSERT_EQUALS(Qfalse->metaclass(state), state->globals.false_class);

    Tuple *tup = Tuple::create(state, 1);
    TS_ASSERT(!MetaClass::is_a(tup->klass));

    TS_ASSERT(MetaClass::is_a(tup->metaclass(state)));
    TS_ASSERT(MetaClass::is_a(tup->klass));
  }
};

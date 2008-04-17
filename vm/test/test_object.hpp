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

  void test_i2n() {
    OBJECT f = Object::i2n(state, 3);
    TS_ASSERT(f->fixnum_p());
    TS_ASSERT_EQUALS(f->n2i(), 3);

    OBJECT b = Object::i2n(state, 2147483647);
    TS_ASSERT(!b->fixnum_p());
    TS_ASSERT(Bignum::is_a(b));
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

    TS_ASSERT(str->kind_of_p(state, state->globals.string));
    TS_ASSERT(!str->kind_of_p(state, state->globals.tuple));
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

  void test_get_ivar() {
    OBJECT sym = state->globals.symbols->lookup(state, "@test");
    OBJECT val = Object::i2n(33);
    OBJECT obj = state->om->new_object(state->globals.object, NormalObject::fields);

    TS_ASSERT_EQUALS(Qnil, obj->get_ivar(state, sym));

    obj->set_ivar(state, sym, val);

    TS_ASSERT_EQUALS(val, obj->get_ivar(state, sym));
  }

  void test_id() {
    Tuple* t1 = Tuple::create(state, 2);
    Tuple* t2 = Tuple::create(state, 2);

    uintptr_t id1 = t1->id(state);
    uintptr_t id2 = t2->id(state);

    TS_ASSERT(id1 > 0 && id2 > 0);
    TS_ASSERT(id1 != id2);

    TS_ASSERT_EQUALS(id1, t1->id(state));

    uintptr_t id3 = Object::i2n(33)->id(state);
    TS_ASSERT(id3 != id1);

    uintptr_t id4 = Object::i2n(33)->id(state);
    TS_ASSERT_EQUALS(id3, id4);
  }

  void test_nil_class() {
    TS_ASSERT_EQUALS(Qnil->class_object(state), G(nil_class));
  }

  void test_true_class() {
    TS_ASSERT_EQUALS(Qtrue->class_object(state), G(true_class));
  }

  void test_false_class() {
    TS_ASSERT_EQUALS(Qfalse->class_object(state), G(false_class));
  }

  void test_fixnum_class() {
    for(size_t i = 0; i < SPECIAL_CLASS_MASK; i++) {
      TS_ASSERT_EQUALS(Object::i2n(i)->class_object(state), G(fixnum_class));
    }
  }

  void test_symbol_class() {
    TS_ASSERT_EQUALS(state->symbol("blah")->class_object(state), G(symbol));
  }
};

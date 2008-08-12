#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/compactlookuptable.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestObject : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_dup() {
    Tuple* tup = Tuple::create(state, 1);
    tup->put(state, 0, Qtrue);

    Tuple* tup2 = (Tuple*)tup->dup(state);

    TS_ASSERT_EQUALS(tup2->at(0), Qtrue);
    TS_ASSERT_DIFFERS(tup->id(state), tup2->id(state));
  }

  void test_clone() {
    Tuple* tup = Tuple::create(state, 1);
    tup->put(state, 0, Qtrue);

    Tuple* tup2 = (Tuple*)tup->clone(state);

    TS_ASSERT_EQUALS(tup2->at(0), Qtrue);

    TS_ASSERT_DIFFERS(tup2->id(state), tup->id(state));

    TS_ASSERT_DIFFERS(tup2->metaclass(state), tup->metaclass(state));
    TS_ASSERT_DIFFERS(tup2->metaclass(state)->method_table, tup->metaclass(state)->method_table);
    TS_ASSERT_DIFFERS(tup2->metaclass(state)->constants, tup->metaclass(state)->constants);
  }

  void test_dup_bytes() {
    OBJECT obj = state->om->new_object_bytes(G(object), 1);
    obj->StoresBytes = 1;

    obj->bytes[0] = 8;

    OBJECT obj2 = obj->dup(state);

    TS_ASSERT(obj2->stores_bytes_p());
    TS_ASSERT_EQUALS(obj2->bytes[0], 8);
  }

  void test_kind_of_p() {
    String* str = String::create(state, "blah");

    TS_ASSERT(str->kind_of_p(state, G(string)));
    TS_ASSERT(!str->kind_of_p(state, G(tuple)));
  }

  void test_hash() {
    TS_ASSERT(Fixnum::from(8)->hash(state) > 0);
    TS_ASSERT(Fixnum::from(-8)->hash(state) > 0);
  }

  void test_metaclass() {
    TS_ASSERT(kind_of<MetaClass>(G(object)->metaclass(state)));
    TS_ASSERT_EQUALS(Qnil->metaclass(state), G(nil_class));
    TS_ASSERT_EQUALS(Qtrue->metaclass(state), G(true_class));
    TS_ASSERT_EQUALS(Qfalse->metaclass(state), G(false_class));

    Tuple *tup = Tuple::create(state, 1);
    TS_ASSERT(!kind_of<MetaClass>(tup->klass));

    TS_ASSERT(kind_of<MetaClass>(tup->metaclass(state)));
    TS_ASSERT(kind_of<MetaClass>(tup->klass));
  }

  void test_equal() {
    String* s1 = String::create(state, "whatever");
    String* s2 = String::create(state, "whatever");

    TS_ASSERT_EQUALS(as<Object>(s1)->equal(state, as<Object>(s2)), Qfalse);
    TS_ASSERT_EQUALS(as<Object>(Fixnum::from(0))->equal(state, as<Object>(Fixnum::from(0))), Qtrue);
  }

  void test_set_ivar() {
    char name[7] = "@testX";
    unsigned int i, size = COMPACTLOOKUPTABLE_SIZE / 2 + 2;
    OBJECT obj = state->om->new_object(G(object), Object::fields);
    OBJECT sym;

    for (i = 0; i < size; i++) {
      name[5] = '0' + i;
      sym = state->symbol(name);
      obj->set_ivar(state, sym, Fixnum::from(i));
    }

    sym = state->symbol("@test5");
    TS_ASSERT_EQUALS(obj->get_ivar(state, sym), Fixnum::from(5));
  }

  void test_get_ivar() {
    OBJECT sym = state->symbol("@test");
    OBJECT val = Fixnum::from(33);
    OBJECT obj = state->om->new_object(G(object), Object::fields);

    TS_ASSERT_EQUALS(Qnil, obj->get_ivar(state, sym));

    obj->set_ivar(state, sym, val);

    TS_ASSERT_EQUALS(val, obj->get_ivar(state, sym));
  }

  void test_id() {
    Tuple* t1 = Tuple::create(state, 2);
    Tuple* t2 = Tuple::create(state, 2);

    INTEGER id1 = t1->id(state);
    INTEGER id2 = t2->id(state);

    TS_ASSERT(id1->to_native() > 0);
    TS_ASSERT(id2->to_native() > 0);

    TS_ASSERT_DIFFERS(id1, id2)

    TS_ASSERT_EQUALS(id1, t1->id(state));

    INTEGER id3 = Fixnum::from(33)->id(state);
    TS_ASSERT_DIFFERS(id3, id1);

    INTEGER id4 = Fixnum::from(33)->id(state);
    TS_ASSERT_EQUALS(id3, id4);
    TS_ASSERT(id4->to_native() % 2 != 0);
  }

  void test_tainted_p() {
    OBJECT obj = state->om->new_object(G(object), Object::fields);

    TS_ASSERT_EQUALS(obj->tainted_p(), Qfalse);
    obj->IsTainted = TRUE;
    TS_ASSERT_EQUALS(obj->tainted_p(), Qtrue);
  }

  void test_taint() {
    OBJECT obj = state->om->new_object(G(object), Object::fields);

    TS_ASSERT(!obj->IsTainted);
    obj->taint();
    TS_ASSERT(obj->IsTainted);
  }

  void test_untaint() {
    OBJECT obj = state->om->new_object(G(object), Object::fields);

    obj->IsTainted = TRUE;
    TS_ASSERT(obj->IsTainted);
    obj->untaint();
    TS_ASSERT(!obj->IsTainted);
  }

  void test_frozen_p() {
    OBJECT obj = state->om->new_object(G(object), Object::fields);

    TS_ASSERT_EQUALS(obj->frozen_p(), Qfalse);
    obj->IsFrozen = TRUE;
    TS_ASSERT_EQUALS(obj->frozen_p(), Qtrue);
  }

  void test_freeze() {
    OBJECT obj = state->om->new_object(G(object), Object::fields);

    TS_ASSERT(!obj->IsFrozen);
    obj->freeze();
    TS_ASSERT(obj->IsFrozen);
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
      TS_ASSERT_EQUALS(Fixnum::from(i)->class_object(state), G(fixnum_class));
    }
  }

  void test_symbol_class() {
    TS_ASSERT_EQUALS(state->symbol("blah")->class_object(state), G(symbol));
  }
};

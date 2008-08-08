#include "objects.hpp"
#include "vm.hpp"
#include "builtin/compactlookuptable.hpp"
#include "builtin/lookuptable.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestCompactLookupTable : public CxxTest::TestSuite {
  public:

  VM *state;
  CompactLookupTable *tbl;
  void setUp() {
    state = new VM(1024);
    tbl = CompactLookupTable::create(state);
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    TS_ASSERT_EQUALS(tbl->obj_type, CompactLookupTableType);
    TS_ASSERT_EQUALS(tbl->field_count, COMPACTLOOKUPTABLE_SIZE);
  }

  void test_find() {
    tbl->put(state, 2, Object::i2n(1));
    tbl->put(state, 3, Object::i2n(2));

    TS_ASSERT_EQUALS(tbl->find(state, Object::i2n(1)), Object::i2n(2));
    TS_ASSERT_EQUALS(tbl->find(state, Object::i2n(2)), Qnil);
  }

  void test_add() {
    TS_ASSERT_EQUALS(tbl->at(0), Qnil);
    TS_ASSERT_EQUALS(tbl->at(1), Qnil);

    tbl->add(state, Object::i2n(1), Object::i2n(2));

    TS_ASSERT_EQUALS(tbl->at(0), Object::i2n(1));
    TS_ASSERT_EQUALS(tbl->at(1), Object::i2n(2));
  }

  void test_add_when_full() {
    int size = COMPACTLOOKUPTABLE_SIZE / 2;
    for (int i = 0; i < size; i++) {
      tbl->add(state, Object::i2n(i), Qnil);
    }
    TS_ASSERT_EQUALS(tbl->add(state, Object::i2n(6), Qnil), Qfalse);
  }

  void test_to_lookuptable() {
    LookupTable* lt;
    OBJECT key = Object::i2n(1);
    OBJECT val = Object::i2n(2);

    tbl->put(state, 2, key);
    tbl->put(state, 3, val);
    lt = tbl->to_lookuptable(state);

    TS_ASSERT_EQUALS(lt->fetch(state, key), val);
  }
};

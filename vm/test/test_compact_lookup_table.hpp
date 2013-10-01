#include "vm/test/test.hpp"

#include "builtin/compact_lookup_table.hpp"
#include "builtin/lookup_table.hpp"

class TestCompactLookupTable : public CxxTest::TestSuite, public VMTest {
public:

  CompactLookupTable *tbl;

  void setUp() {
    create();
    tbl = CompactLookupTable::create(state);
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    TS_ASSERT(kind_of<CompactLookupTable>(tbl));
    TS_ASSERT_EQUALS(tbl->num_fields(), (native_int)COMPACTLOOKUPTABLE_SIZE);
  }

  void test_find() {
    tbl->put(state, 2, Fixnum::from(1));
    tbl->put(state, 3, Fixnum::from(2));

    TS_ASSERT_EQUALS(tbl->fetch(state, Fixnum::from(1)), Fixnum::from(2));
    TS_ASSERT_EQUALS(tbl->fetch(state, Fixnum::from(2)), cNil);
  }

  void test_add() {
    TS_ASSERT_EQUALS(tbl->at(state, 0), cNil);
    TS_ASSERT_EQUALS(tbl->at(state, 1), cNil);

    tbl->store(state, Fixnum::from(1), Fixnum::from(2));

    TS_ASSERT_EQUALS(tbl->at(state, 0), Fixnum::from(1));
    TS_ASSERT_EQUALS(tbl->at(state, 1), Fixnum::from(2));
  }

  void test_add_when_full() {
    int size = COMPACTLOOKUPTABLE_SIZE / 2;
    for (int i = 0; i < size; i++) {
      tbl->store(state, Fixnum::from(i), cNil);
    }
    TS_ASSERT_EQUALS(tbl->store(state, Fixnum::from(6), cNil), cFalse);
  }

  void test_has_key() {
    tbl->store(state, Fixnum::from(0), cNil);
    TS_ASSERT_EQUALS(tbl->has_key(state, Fixnum::from(0)), cTrue);
    TS_ASSERT_EQUALS(tbl->has_key(state, Fixnum::from(1)), cFalse);
  }

  void test_keys() {
    tbl->store(state, Fixnum::from(0), Fixnum::from(1));
    tbl->store(state, Fixnum::from(2), Fixnum::from(3));
    Array* ary = tbl->keys(state);

    TS_ASSERT_EQUALS(ary->size(), 2);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(0));
    TS_ASSERT_EQUALS(ary->get(state, 1), Fixnum::from(2));
  }

  void test_values() {
    tbl->store(state, Fixnum::from(0), Fixnum::from(1));
    tbl->store(state, Fixnum::from(2), Fixnum::from(3));
    Array* ary = tbl->values(state);

    TS_ASSERT_EQUALS(ary->size(), 2);
    TS_ASSERT_EQUALS(ary->get(state, 0), Fixnum::from(1));
    TS_ASSERT_EQUALS(ary->get(state, 1), Fixnum::from(3));
  }

  void test_to_lookuptable() {
    LookupTable* lt;
    Object* key = Fixnum::from(1);
    Object* val = Fixnum::from(2);

    tbl->put(state, 2, key);
    tbl->put(state, 3, val);
    lt = tbl->to_lookuptable(state);

    TS_ASSERT_EQUALS(lt->fetch(state, key), val);
  }
};

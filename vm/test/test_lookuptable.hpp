#include "vm/test/test.hpp"

#include "ontology.hpp"

class TestLookupTable : public CxxTest::TestSuite, public VMTest {
public:

  LookupTable *tbl;

  void setUp() {
    create();
    tbl = LookupTable::create(state);
  }

  void tearDown() {
    destroy();
  }

  void test_create() {
    TS_ASSERT(kind_of<LookupTable>(tbl));
    TS_ASSERT_EQUALS(tbl-> bins()->to_native(), LOOKUPTABLE_MIN_SIZE);
  }

  void test_allocate() {
    Class* sub = ontology::new_class(state, "LookupTableSub", G(lookuptable), 0);
    LookupTable* tbl = LookupTable::allocate(state, sub);

    TS_ASSERT_EQUALS(tbl->klass(), sub);
  }

  void test_store_fetch() {
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 0);
    tbl->store(state, Qnil, Fixnum::from(47));
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 1);

    Object* out = tbl->aref(state, Qnil);
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), 47);
  }

  void test_store_overwrites_previous() {
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 0);
    tbl->store(state, Qnil, Fixnum::from(47));
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 1);

    Object* out = tbl->aref(state, Qnil);
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), 47);

    tbl->store(state, Qnil, Fixnum::from(42));
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 1);

    out = tbl->aref(state, Qnil);
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), 42);
  }

  void test_store_handles_entries_in_same_bin() {
    Object* k1 = Fixnum::from((4 << 4)  | 15);
    Object* k2 = Fixnum::from((10 << 4) | 15);
    Object* k3 = Fixnum::from((11 << 4) | 15);
    Object* v1 = Qtrue;
    Object* v2 = Qfalse;
    Object* v3 = Qtrue;

    tbl->store(state, k1, v1);
    tbl->store(state, k2, v2);
    tbl->store(state, k3, v3);
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 3);

    LookupTableBucket* entry = tbl->find_entry(state, k1);
    TS_ASSERT(entry != Qnil);

    TS_ASSERT(entry->next() != Qnil);
    TS_ASSERT_EQUALS(entry->next()->key(), k2);

    entry = tbl->find_entry(state, k3);
    TS_ASSERT(entry != Qnil);
    TS_ASSERT_EQUALS(entry->key(), k3);
  }

  void test_store_resizes_table() {
    size_t i;
    size_t bins = tbl-> bins()->to_native();

    for(i = 0; i < bins; i++) {
      tbl->store(state, Fixnum::from(i), Fixnum::from(i));
    }

    TS_ASSERT_EQUALS(i, (size_t)as<Integer>(tbl->entries())->to_native());

    TS_ASSERT((size_t)(tbl-> bins()->to_native()) > bins);

    for(i = 0; i < bins; i++) {
      TS_ASSERT_EQUALS(Fixnum::from(i), tbl->aref(state, Fixnum::from(i)));
    }
  }

  void test_store_resizes_table_with_chained_bins() {
    size_t i;
    size_t bins = tbl-> bins()->to_native() - 2;

    Object* k1 = Fixnum::from((4 << 5)  | 31);
    Object* k2 = Fixnum::from((10 << 5) | 31);
    Object* k3 = Fixnum::from((11 << 5) | 31);
    tbl->store(state, k1, Qtrue);
    tbl->store(state, k2, Qtrue);
    tbl->store(state, k3, Qtrue);

    for(i = 0; i < bins; i++) {
      tbl->store(state, Fixnum::from(i), Qtrue);
    }

    TS_ASSERT((size_t)(tbl-> bins()->to_native()) > bins);
  }

  void test_find_entry() {
    Object* k = Fixnum::from(47);
    tbl->store(state, k, Qtrue);

    LookupTableBucket* entry = tbl->find_entry(state, k);
    TS_ASSERT(entry != Qnil);
    TS_ASSERT_EQUALS(entry->key(),k);
    TS_ASSERT_EQUALS(entry->value(),Qtrue);

    entry = tbl->find_entry(state, Fixnum::from(40));
    TS_ASSERT(entry == Qnil);
  }

  void test_find() {
    Object* k = Fixnum::from(47);
    tbl->store(state, k, Qtrue);

    Object* out = tbl->find(state, k);
    TS_ASSERT_EQUALS(out, Qtrue);

    out = tbl->find(state, Fixnum::from(40));
    TS_ASSERT(out == Qundef);
  }

  void test_remove() {
    Object* k = Fixnum::from(47);
    tbl->store(state, k, Qtrue);

    Object* out = tbl->find(state, k);
    TS_ASSERT_EQUALS(out, Qtrue);

    out = tbl->remove(state, k);
    TS_ASSERT_EQUALS(out, Qtrue);
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 0);

    out = tbl->fetch(state, k);
    TS_ASSERT_EQUALS(out, Qnil);
  }

  void test_remove_redistributes() {
    size_t bins = tbl-> bins()->to_native();
    size_t bound = bins * 2;

    for(size_t i = 0; i < bound; i++) {
      tbl->store(state, Fixnum::from(i), Qtrue);
    }

    TS_ASSERT(bins < (size_t)tbl-> bins()->to_native());

    Object* out;
    for(size_t i = 0; i < bound; i++) {
      out = tbl->remove(state, Fixnum::from(i));
      TS_ASSERT_EQUALS(out, Qtrue);
    }

    TS_ASSERT_EQUALS(bins, static_cast<unsigned int>(tbl-> bins()->to_native()));
  }

  void test_remove_works_for_chained_bins() {
    Object* k1 = Fixnum::from((4 << 5)  | 31);
    Object* k2 = Fixnum::from((10 << 5) | 31);
    Object* k3 = Fixnum::from((11 << 5) | 31);
    tbl->store(state, k1, Qnil);
    tbl->store(state, k2, Qtrue);
    tbl->store(state, k3, Qfalse);

    TS_ASSERT_EQUALS(tbl->remove(state, k3), Qfalse);
    TS_ASSERT_EQUALS(tbl->remove(state, k2), Qtrue);
    TS_ASSERT_EQUALS(tbl->remove(state, k1), Qnil);

    TS_ASSERT_EQUALS(0, as<Integer>(tbl->entries())->to_native());
  }

  void test_remove_works_for_unknown_key() {
    Object* k1 = Fixnum::from(4);

    TS_ASSERT_EQUALS(Qnil, tbl->remove(state, k1));
  }

  void test_has_key() {
    Object* k1 = Fixnum::from(4);
    TS_ASSERT_EQUALS(Qfalse, tbl->has_key(state, k1));

    tbl->store(state, k1, Qtrue);

    TS_ASSERT_EQUALS(Qtrue, tbl->has_key(state, k1));
  }

  void test_dup() {
    Object* k1 = Fixnum::from(4);

    tbl->store(state, k1, Qtrue);

    LookupTable* tbl2 = tbl->duplicate(state);

    TS_ASSERT_EQUALS(tbl2->aref(state, k1), Qtrue);
  }

  void test_all_keys() {
    Object* k1 = Fixnum::from(4);

    tbl->store(state, k1, Qtrue);
    Array* ary = tbl->all_keys(state);

    TS_ASSERT_EQUALS(ary->total()->to_native(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), k1);
  }

  void test_all_values() {
    Object* k1 = Fixnum::from(4);

    tbl->store(state, k1, Qtrue);
    Array* ary = tbl->all_values(state);

    TS_ASSERT_EQUALS(ary->total()->to_native(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);

  }

  void test_fetch_returns_found() {
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 0);
    tbl->store(state, Qnil, Fixnum::from(47));
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 1);

    Object* out = tbl->fetch(state, Qnil, Qundef);
    TS_ASSERT_EQUALS(as<Integer>(out)->to_native(), 47);
  }

  void test_fetch_returns_given_when_not_found() {
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 0);
    tbl->store(state, Qnil, Fixnum::from(47));
    TS_ASSERT_EQUALS(as<Integer>(tbl->entries())->to_native(), 1);

    Object* out = tbl->fetch(state, Qtrue, Qundef);
    TS_ASSERT_EQUALS(Qundef, out);
  }
};

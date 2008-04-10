#include "objects.hpp"
#include "vm.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestLookupTable : public CxxTest::TestSuite {
  public:

#define state &vm

  void test_create() {
    VM vm(1024);
    LookupTable *tbl;

    tbl = LookupTable::create(&vm);
    TS_ASSERT_EQUALS(tbl->obj_type, LookupTableType);
  }

  void test_store_fetch() {
    VM vm(1024);
    LookupTable *tbl;

    tbl = LookupTable::create(&vm);
    TS_ASSERT_EQUALS(tbl->entries->n2i(), 0);
    tbl->store(&vm, Qnil, Object::i2n(47));
    TS_ASSERT_EQUALS(tbl->entries->n2i(), 1);

    OBJECT out = tbl->fetch(state, Qnil);
    TS_ASSERT_EQUALS(out->n2i(), 47);
  }

  void test_store_overwrites_previous() {
    VM vm(1024);
    LookupTable *tbl;

    tbl = LookupTable::create(&vm);
    TS_ASSERT_EQUALS(tbl->entries->n2i(), 0);
    tbl->store(&vm, Qnil, Object::i2n(47));
    TS_ASSERT_EQUALS(tbl->entries->n2i(), 1);

    OBJECT out = tbl->fetch(state, Qnil);
    TS_ASSERT_EQUALS(out->n2i(), 47);

    tbl->store(state, Qnil, Object::i2n(42));
    TS_ASSERT_EQUALS(tbl->entries->n2i(), 1);

    out = tbl->fetch(state, Qnil);
    TS_ASSERT_EQUALS(out->n2i(), 42);
  }

  void test_store_handles_entries_in_same_bin() {
    VM vm(1024);
    LookupTable *tbl;

    OBJECT k1 = Object::i2n((4 << 4)  | 15);
    OBJECT k2 = Object::i2n((10 << 4) | 15);
    OBJECT k3 = Object::i2n((11 << 4) | 15);
    OBJECT v1 = Qtrue;
    OBJECT v2 = Qfalse;
    OBJECT v3 = Qtrue;

    tbl = LookupTable::create(&vm);
    tbl->store(state, k1, v1);
    tbl->store(state, k2, v2);
    tbl->store(state, k3, v3);
    TS_ASSERT_EQUALS(tbl->entries->n2i(), 3);

    OBJECT entry = tbl->find_entry(state, k1);
    TS_ASSERT(!entry->nil_p());

    TS_ASSERT(!entry->at(2)->nil_p());
    TS_ASSERT_EQUALS(entry->at(2)->at(0), k2);

    entry = tbl->find_entry(state, k3);
    TS_ASSERT(!entry->nil_p());
    TS_ASSERT_EQUALS(entry->at(0), k3);
  }

  void test_store_resizes_table() {
    VM vm(1024);
    LookupTable *tbl;
    size_t i;

    tbl = LookupTable::create(&vm);
    size_t bins = tbl->bins->n2i();

    for(i = 0; i < bins; i++) {
      tbl->store(state, Object::i2n(i), Object::i2n(i));
    }

    TS_ASSERT_EQUALS(i, (size_t)tbl->entries->n2i());

    TS_ASSERT((size_t)(tbl->bins->n2i()) > bins);

    for(i = 0; i < bins; i++) {
      TS_ASSERT_EQUALS(Object::i2n(i), tbl->fetch(state, Object::i2n(i)));
    }
  }

  void test_store_resizes_table_with_chained_bins() {
    VM vm(1024);
    LookupTable *tbl;
    size_t i;

    tbl = LookupTable::create(&vm);
    size_t bins = tbl->bins->n2i() - 2;

    OBJECT k1 = Object::i2n((4 << 5)  | 31);
    OBJECT k2 = Object::i2n((10 << 5) | 31);
    OBJECT k3 = Object::i2n((11 << 5) | 31);
    tbl->store(state, k1, Qtrue);
    tbl->store(state, k2, Qtrue);
    tbl->store(state, k3, Qtrue);

    for(i = 0; i < bins; i++) {
      tbl->store(state, Object::i2n(i), Qtrue);
    }

    TS_ASSERT((size_t)(tbl->bins->n2i()) > bins);
  }

  void test_find_entry() {
    VM vm(1024);

    LookupTable* tbl = LookupTable::create(&vm);
    OBJECT k = Object::i2n(47);
    tbl->store(state, k, Qtrue);

    OBJECT entry = tbl->find_entry(state, k);
    TS_ASSERT_EQUALS(k, entry->at(0));

    entry = tbl->find_entry(state, Object::i2n(40));
    TS_ASSERT(entry->nil_p());
  }

  void test_find() {
    VM vm(1024);

    LookupTable* tbl = LookupTable::create(&vm);
    OBJECT k = Object::i2n(47);
    tbl->store(state, k, Qtrue);

    OBJECT out = tbl->find(state, k);
    TS_ASSERT_EQUALS(out, Qtrue);

    out = tbl->find(state, Object::i2n(40));
    TS_ASSERT(out->undef_p());
  }

  void test_remove() {
    VM vm(1024);

    LookupTable* tbl = LookupTable::create(&vm);
    OBJECT k = Object::i2n(47);
    tbl->store(state, k, Qtrue);

    OBJECT out = tbl->find(state, k);
    TS_ASSERT_EQUALS(out, Qtrue);

    out = tbl->remove(state, k);
    TS_ASSERT_EQUALS(out, Qtrue);
    TS_ASSERT_EQUALS(tbl->entries->n2i(), 0);

    out = tbl->fetch(state, k);
    TS_ASSERT_EQUALS(out, Qnil);
  }

  void test_remove_redistributes() {
    VM vm(1024);

    LookupTable* tbl = LookupTable::create(&vm);
    size_t bins = tbl->bins->n2i();
    size_t bound = bins * 2;

    for(size_t i = 0; i < bound; i++) {
      tbl->store(state, Object::i2n(i), Qtrue);
    }

    TS_ASSERT(bins < (size_t)tbl->bins->n2i());

    OBJECT out;
    for(size_t i = 0; i < bound; i++) {
      out = tbl->remove(state, Object::i2n(i));
      TS_ASSERT_EQUALS(out, Qtrue);
    }

    TS_ASSERT_EQUALS(bins, tbl->bins->n2i());
  }

  void test_remove_works_for_chained_bins() {
    VM vm(1024);
    LookupTable *tbl;

    tbl = LookupTable::create(&vm);

    OBJECT k1 = Object::i2n((4 << 5)  | 31);
    OBJECT k2 = Object::i2n((10 << 5) | 31);
    OBJECT k3 = Object::i2n((11 << 5) | 31);
    tbl->store(state, k1, Qnil);
    tbl->store(state, k2, Qtrue);
    tbl->store(state, k3, Qfalse);

    TS_ASSERT_EQUALS(tbl->remove(state, k3), Qfalse);
    TS_ASSERT_EQUALS(tbl->remove(state, k2), Qtrue);
    TS_ASSERT_EQUALS(tbl->remove(state, k1), Qnil);

    TS_ASSERT_EQUALS(0, tbl->entries->n2i());
  }

  void test_remove_works_for_unknown_key() {
    VM vm(1024);
    LookupTable *tbl;

    tbl = LookupTable::create(&vm);

    OBJECT k1 = Object::i2n(4);

    TS_ASSERT_EQUALS(Qnil, tbl->remove(state, k1));
  }

  void test_has_key() {
    VM vm(1024);
    LookupTable *tbl;

    tbl = LookupTable::create(&vm);

    OBJECT k1 = Object::i2n(4);
    TS_ASSERT_EQUALS(Qfalse, tbl->has_key(state, k1));

    tbl->store(state, k1, Qtrue);

    TS_ASSERT_EQUALS(Qtrue, tbl->has_key(state, k1));
  }

  void test_dup() {
    VM vm(1024);
    LookupTable *tbl;

    tbl = LookupTable::create(&vm);
    OBJECT k1 = Object::i2n(4);

    tbl->store(state, k1, Qtrue);

    LookupTable* tbl2 = tbl->dup(state);

    TS_ASSERT_EQUALS(tbl2->fetch(state, k1), Qtrue);
  }

  void test_all_keys() {
    VM vm(1024);
    LookupTable *tbl;

    tbl = LookupTable::create(&vm);
    OBJECT k1 = Object::i2n(4);

    tbl->store(state, k1, Qtrue);
    Array* ary = tbl->all_keys(state);

    TS_ASSERT_EQUALS(ary->total->n2i(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), k1);
  }

  void test_all_values() {
    VM vm(1024);
    LookupTable *tbl;

    tbl = LookupTable::create(&vm);
    OBJECT k1 = Object::i2n(4);

    tbl->store(state, k1, Qtrue);
    Array* ary = tbl->all_values(state);

    TS_ASSERT_EQUALS(ary->total->n2i(), 1);
    TS_ASSERT_EQUALS(ary->get(state, 0), Qtrue);

  }
};

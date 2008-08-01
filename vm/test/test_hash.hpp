#include "objects.hpp"
#include "vm.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestHash : public CxxTest::TestSuite {
  public:

  VM *state;
  Hash *tbl;
  hashval hash;
  OBJECT key;
  OBJECT val;

  hashval hash2;
  OBJECT key2;
  OBJECT val2;

  void setUp() {
    state = new VM();
    tbl = Hash::create(state);
    hash = 3;
    key = Object::i2n(8);
    val = Object::i2n(3234);

    hash2 = 8;
    key2 = Object::i2n(60);
    val2 =  Object::i2n(1123);
  }

  void tearDown() {
    delete state;
  }

  void test_create() {
    TS_ASSERT_EQUALS(tbl->obj_type, HashType);
  }

  void test_dup() {
    tbl->add(state, hash, key, val);

    tbl->add(state, (1 << 4) | hash, key2, val2);

    Hash* dup = tbl->dup(state);

    TS_ASSERT_EQUALS(dup->get(state, hash), val);
  }

  void test_add_and_get() {
    tbl->add(state, hash, key, val);
    OBJECT out = tbl->get(state, hash);

    TS_ASSERT_EQUALS(val, out);
    TS_ASSERT_EQUALS(tbl->entries->n2i(), 1);
  }

  void test_add_on_chained_buckets() {
    tbl->add(state, hash, key, val);
    hashval collide = (1 << 4) | hash;
    tbl->add(state, collide, key2, val2);

    hashval collide2 = (2 << 4) | hash;
    tbl->add(state, collide2, Object::i2n(12), Qtrue);

    TS_ASSERT_EQUALS(val, tbl->get(state, hash));
    TS_ASSERT_EQUALS(val2, tbl->get(state, collide));
    TS_ASSERT_EQUALS(Qtrue, tbl->get(state, collide2));

    size_t bins = tbl->values->field_count;

    for(size_t i = 0; i < bins; i++) {
      tbl->set(state, Object::i2n(i), Qtrue);
    }
  }

  void test_add_replaces_on_same_hash() {
    tbl->add(state, hash, key, val);
    tbl->add(state, hash, key2, val2);

    TS_ASSERT_EQUALS(tbl->entries->n2i(), 1);
    TS_ASSERT_EQUALS(tbl->get(state, hash), val2);
  }

  void test_set() {
    tbl->set(state, key, val);

    TS_ASSERT_EQUALS(tbl->get(state, key->hash(state)), val);
  }

  void test_get_returns_nil_on_not_found() {
    TS_ASSERT_EQUALS(tbl->get(state, hash), Qnil);
  }

  void test_lookup() {
    Tuple* entry = (Tuple*)Hash::entry_new(state, hash, key, val);
    tbl->add_entry(state, hash, entry);

    entry = (Tuple*)Hash::entry_new(state, hash, key2, val2);
    tbl->add_entry(state, hash, entry);

    TS_ASSERT_EQUALS(tbl->entries->n2i(), 2);

    OBJECT out;
    TS_ASSERT(tbl->lookup(state, key, hash, &out));
    TS_ASSERT_EQUALS(out, val);

    TS_ASSERT(tbl->lookup(state, key2, hash, &out));
    TS_ASSERT_EQUALS(out, val2);

    TS_ASSERT(!tbl->lookup(state, Qtrue, hash, &out));
  }

  static bool _compare1(VM* vm, OBJECT cur, OBJECT key) {
    return true;
  }

  static bool _compare2(VM* vm, OBJECT cur, OBJECT key) {
    return false;
  }

  void test_lookup2() {
    Tuple* entry = (Tuple*)Hash::entry_new(state, hash, key, val);
    tbl->add_entry(state, hash, entry);

    entry = (Tuple*)Hash::entry_new(state, hash, key2, val2);
    tbl->add_entry(state, hash, entry);

    OBJECT out;
    TS_ASSERT(tbl->lookup2(state, _compare1, key2, hash, &out));

    TS_ASSERT_EQUALS(val, out);

    TS_ASSERT(!tbl->lookup2(state, _compare2, key2, hash, &out));
  }

  void test_assign() {
    tbl->assign(state, _compare1, key, hash, val);
    tbl->assign(state, _compare1, key2, hash, val2);

    TS_ASSERT_EQUALS(tbl->entries->n2i(), 1);

    tbl->assign(state, _compare2, key2, hash, val2);

    TS_ASSERT_EQUALS(tbl->entries->n2i(), 2);
  }

  void test_assign_redistributes() {
    size_t bins = tbl->values->field_count;

    for(size_t i = 0; i < bins; i++) {
      OBJECT key = Object::i2n(i);
      tbl->assign(state, _compare1, key, key->hash(state), val);
    }

    TS_ASSERT(bins < (size_t)tbl->values->field_count);
  }

  void test_get_undef() {
    tbl->add(state, hash, key, val);

    TS_ASSERT_EQUALS(tbl->get_undef(state, hash), val);
    TS_ASSERT_EQUALS(Qundef, tbl->get_undef(state, hash2));
  }

  void test_remove() {
    tbl->add(state, hash, key, val);

    TS_ASSERT_EQUALS(tbl->remove(state, hash), val);
    TS_ASSERT_EQUALS(tbl->remove(state, hash), Qnil);
  }

  void test_remove_from_chained_bins() {
    tbl->add(state, hash, key, val);

    hashval collide = (1 << 4) | hash;
    tbl->add(state, collide, key2, val2);

    TS_ASSERT_EQUALS(tbl->remove(state, collide), val2);
  }

  void test_from_tuple() {
    Tuple *tup = Tuple::create(state, 4);
    tup->put(state, 0, key);
    tup->put(state, 1, val);
    tup->put(state, 2, key2);
    tup->put(state, 3, val2);

    tbl = Hash::from_tuple(state, tup);

    TS_ASSERT_EQUALS(tbl->get(state, key->hash(state)), val);
    TS_ASSERT_EQUALS(tbl->get(state, key2->hash(state)), val2);
  }

  void test_csm_new() {
    Tuple *tup = Hash::csm_new(state);
    TS_ASSERT_EQUALS(tup->field_count % 1, (size_t)0);
  }

  void test_csm_add_and_find() {
    Tuple *tup = Hash::csm_new(state);

    TS_ASSERT_EQUALS(Hash::csm_find(state, tup, key), Qnil);

    TS_ASSERT_EQUALS(Hash::csm_add(state, tup, key, val), Qtrue);
    TS_ASSERT_EQUALS(Hash::csm_find(state, tup, key), val);
  }

  void test_csm_add_returns_false_on_full() {
    Tuple *tup = Hash::csm_new(state);
    for(size_t i = 0; i < 6; i++) {
      TS_ASSERT_EQUALS(Hash::csm_add(state, tup, Object::i2n(i), val), Qtrue);
    }
    TS_ASSERT_EQUALS(Hash::csm_add(state, tup, key, val), Qfalse);
  }

  void test_csm_into_hash() {
    Tuple *tup = Hash::csm_new(state);

    Hash::csm_add(state, tup, key, val);

    tbl = Hash::csm_into_hash(state, tup);

    TS_ASSERT_EQUALS(tbl->get(state, key->hash(state)), val);
  }

  void test_csm_into_lookuptable() {
    Tuple *tup = Hash::csm_new(state);

    Hash::csm_add(state, tup, key, val);

    LookupTable *lt = Hash::csm_into_lookuptable(state, tup);

    TS_ASSERT_EQUALS(lt->fetch(state, key), val);
  }
};

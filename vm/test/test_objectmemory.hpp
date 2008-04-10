#include <iostream>
#include "objectmemory.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestObjectMemory : public CxxTest::TestSuite {
  public:

  void test_new_object() {
    ObjectMemory om(1024);
    OBJECT obj;

    TS_ASSERT_EQUALS(om.young.current->used(), 0);

    obj = om.new_object(Qnil, 3);

    TS_ASSERT_EQUALS(obj->field_count, 3);
    TS_ASSERT_EQUALS(obj->klass, Qnil);
    TS_ASSERT_EQUALS(obj->zone, YoungObjectZone);

    TS_ASSERT(om.young.current->used() == obj->size_in_bytes());
    TS_ASSERT(om.young.heap_a.used()  == obj->size_in_bytes());
    TS_ASSERT_EQUALS(om.young.current->remaining(), 1024 - obj->size_in_bytes() - 1);
  }

  void test_write_barrier() {
    ObjectMemory om(1024);
    OBJECT obj, obj2;

    obj  = om.new_object(Qnil, 2);
    obj2 = om.new_object(Qnil, 2);
    TS_ASSERT_EQUALS(obj->Remember, 0);
    TS_ASSERT_EQUALS(obj2->Remember, 0);
    TS_ASSERT_EQUALS(om.remember_set->size(), 0);

    obj->zone = MatureObjectZone;
    om.store_object(obj, 0, obj2);

    TS_ASSERT_EQUALS(om.remember_set->size(), 1);
    TS_ASSERT_EQUALS(obj->Remember, 1);
    TS_ASSERT_EQUALS(om.remember_set->operator[](0), obj);

    om.store_object(obj, 0, obj2);
    TS_ASSERT_EQUALS(om.remember_set->size(), 1);
  }

  /* Causes a segfault when fails. */
  void test_write_barrier_not_called_for_immediates() {
    ObjectMemory om(1024);
    OBJECT obj, obj2;

    obj = om.new_object(Qnil, 2);
    obj->zone = MatureObjectZone;

    obj2 = Qnil;

    om.store_object(obj, 0, obj2);
    TS_ASSERT_EQUALS(obj->Remember, 0);
  }

  void test_collect_young() {
    ObjectMemory om(1024);
    OBJECT obj;
    TS_ASSERT_EQUALS(om.young.current->used(), 0);

    om.new_object(Qnil, 3);
    om.new_object(Qnil, 3);
    om.new_object(Qnil, 3);
    om.new_object(Qnil, 3);
    obj = om.new_object(Qnil, 3);

    BakerGC::Heap *cur = om.young.next;
    TS_ASSERT_EQUALS(om.young.current->used(), obj->size_in_bytes() * 5);

    ObjectArray roots(0);
    om.collect_young(roots);

    TS_ASSERT_EQUALS(om.young.current->used(), 0);
    TS_ASSERT_EQUALS((void*)cur, (void*)om.young.current);

    obj = om.new_object(Qnil, 3);
    TS_ASSERT_EQUALS(obj->age, 0);
    roots.push_back(obj);

    om.collect_young(roots);
    TS_ASSERT_EQUALS(om.young.current->used(), obj->size_in_bytes());
  }

  void test_collect_young_through_references() {
    ObjectMemory om(1024);
    OBJECT obj, obj2, obj3;

    obj =  om.new_object(Qnil, 3);
    obj2 = om.new_object(Qnil, 3);

    om.store_object(obj, 0, obj2);
    om.store_object(obj2, 0, Qtrue);

    ObjectArray roots(0);
    roots.push_back(obj);

    om.collect_young(roots);

    TS_ASSERT_EQUALS(om.young.current->used(),
                     obj->size_in_bytes() + obj2->size_in_bytes());

    OBJECT new_obj = roots[0];
    TS_ASSERT(obj != new_obj);
    obj = new_obj;

    TS_ASSERT(om.young.current->contains_p(obj));
    obj3 = obj->at(0);
    TS_ASSERT(obj2 != obj3);

    TS_ASSERT_EQUALS(obj2->at(0), Qtrue);
  }

  /* Could crash on failure */
  void test_collect_young_skips_byte_storage() {
    ObjectMemory om(1024);
    OBJECT obj, obj2;

    obj =  om.new_object_bytes(Qnil, 3);
    obj2 = om.new_object(Qnil, 3);

    /* Force obj2 to appear in the body, but it should be seen as
     * just a generic series of bytes, not a reference. */
    obj->field[0] = obj2;

    ObjectArray roots(0);
    roots.push_back(obj);

    om.collect_young(roots);

    obj = roots[0];
    TS_ASSERT_EQUALS(obj->field[0], obj2);
  }

  void test_new_object_mature() {
    ObjectMemory om(1024);
    OBJECT obj;

    obj = om.new_object_mature(Qnil, 3);
    TS_ASSERT_EQUALS(obj->field_count, 3);
    TS_ASSERT_EQUALS(obj->klass, Qnil);
    TS_ASSERT_EQUALS(obj->zone, MatureObjectZone);
    TS_ASSERT(obj->stores_references_p());

    TS_ASSERT_EQUALS(om.young.current->used(), 0);

    obj = om.new_object_mature(Qnil, 3, true);
    TS_ASSERT(obj->mature_object_p());
    TS_ASSERT(obj->stores_bytes_p());
  }

  void test_new_large_object() {
    ObjectMemory om(1024);
    OBJECT obj;

    om.large_object_threshold = 10;

    obj = om.new_object(Qnil, 20);
    TS_ASSERT_EQUALS(obj->field_count, 20);
    TS_ASSERT_EQUALS(obj->klass, Qnil);
    TS_ASSERT_EQUALS(obj->zone, MatureObjectZone);

    TS_ASSERT_EQUALS(om.young.current->used(), 0);
  }

  void test_collect_young_doesnt_move_mature_objects() {
    ObjectMemory om(1024);
    OBJECT obj;

    om.large_object_threshold = 10;

    obj = om.new_object(Qnil, 20);

    ObjectArray roots(0);
    roots.push_back(obj);

    om.collect_young(roots);

    TS_ASSERT_EQUALS(obj, roots[0]);
  }

  void test_collect_young_uses_remember_set() {
    ObjectMemory om(1024);
    OBJECT young, mature;

    om.large_object_threshold = 10;

    young =  om.new_object(Qnil, 3);
    mature = om.new_object(Qnil, 20);

    om.store_object(young, 0, Qtrue);
    om.store_object(mature, 0, young);
    TS_ASSERT_EQUALS(mature->Remember, 1);

    ObjectArray roots(0);
    om.collect_young(roots);

    TS_ASSERT(mature->at(0) != young);
    TS_ASSERT_EQUALS(mature->at(0)->at(0), Qtrue);
  }

  void test_collect_young_promotes_objects() {
    ObjectMemory om(1024);
    OBJECT young;

    young = om.new_object(Qnil, 3);

    ObjectArray roots(0);
    roots.push_back(young);

    om.set_young_lifetime(1);

    TS_ASSERT_EQUALS(young->age, 0);
    om.collect_young(roots);
    TS_ASSERT_EQUALS(roots[0]->age, 1);
    om.collect_young(roots);

    TS_ASSERT_EQUALS(roots[0]->age, 0);

    TS_ASSERT(roots[0]->mature_object_p());
  }

  void test_collect_young_resets_remember_set() {
    ObjectMemory om(1024);
    OBJECT young, mature;

    om.large_object_threshold = 10;

    young =  om.new_object(Qnil, 3);
    mature = om.new_object(Qnil, 20);

    TS_ASSERT(mature->mature_object_p());
    TS_ASSERT(young->young_object_p());

    om.store_object(mature, 0, young);
    ObjectArray roots(0);

    om.set_young_lifetime(1);

    TS_ASSERT_EQUALS(mature->Remember, 1);
    TS_ASSERT_EQUALS(om.remember_set->size(), 1);

    TS_ASSERT_EQUALS(young->age, 0);
    om.collect_young(roots);
    TS_ASSERT_EQUALS(mature->at(0)->age, 1);
    om.collect_young(roots);

    TS_ASSERT_EQUALS(om.remember_set->size(), 0);
  }

  void test_collect_young_uses_forwarding_pointers() {
    ObjectMemory om(1024);
    OBJECT obj, obj2;

    obj =  om.new_object(Qnil, 3);
    obj2 = om.new_object(Qnil, 3);

    om.store_object(obj, 0, obj2);
    om.store_object(obj, 1, obj2);
    om.store_object(obj, 2, obj2);

    ObjectArray roots(0);
    roots.push_back(obj);

    om.collect_young(roots);

    obj = roots[0];

    obj2 = obj->at(0);
    TS_ASSERT_EQUALS(obj2, obj->at(1));
    TS_ASSERT_EQUALS(obj2, obj->at(2));
  }

  void test_collect_young_copies_byte_bodies() {
    ObjectMemory om(1024);
    OBJECT obj;

    obj = om.new_object_bytes(Qnil, 3);
    obj->bytes[0] = 47;

    ObjectArray roots(0);
    roots.push_back(obj);

    om.collect_young(roots);

    obj = roots[0];
    TS_ASSERT_EQUALS(obj->bytes[0], 47);
  }

  void test_collect_young_class_considered() {
    ObjectMemory om(1024);
    OBJECT obj, cls;

    cls = om.new_object(Qnil, 3);
    obj = om.new_object(cls, 3);

    ObjectArray roots(0);
    roots.push_back(obj);

    om.collect_young(roots);

    obj = roots[0];

    TS_ASSERT(om.valid_object_p(obj->klass));
  }

  void test_collect_mature() {
    ObjectMemory om(1024);
    OBJECT mature;

    om.debug_marksweep(true);

    om.large_object_threshold = 10;

    mature = om.new_object(Qnil, 20);
    TS_ASSERT(mature->mature_object_p());
    TS_ASSERT_EQUALS(om.mature.allocated_objects, 1);

    MarkSweepGC::Entry *entry = om.mature.find_entry(mature);
    TS_ASSERT(entry->unmarked_p());

    ObjectArray roots(0);
    om.collect_mature(roots);

    TS_ASSERT_EQUALS(om.mature.allocated_objects, 0);
    TS_ASSERT(entry->unmarked_p());

    /* debug_marksweep() causes it to not free any Entry's, so
     * we have to do it now. */
    delete entry;
  }

  void test_collect_mature_marks_young_objects() {
    ObjectMemory om(1024);
    OBJECT young, mature;

    om.large_object_threshold = 10;

    young =  om.new_object(Qnil, 3);
    mature = om.new_object(Qnil, 20);

    om.store_object(young, 0, mature);

    ObjectArray roots(0);
    roots.push_back(young);

    om.collect_mature(roots);
    TS_ASSERT_EQUALS(young->Marked, 0);

    TS_ASSERT_EQUALS(om.mature.allocated_objects, 1);
  }

  /* Could segfault on failure due to infinite loop. */
  void test_collect_mature_stops_at_already_marked_objects() {
    ObjectMemory om(1024);
    OBJECT young, mature;

    om.large_object_threshold = 10;

    young =  om.new_object(Qnil, 3);
    mature = om.new_object(Qnil, 20);

    om.store_object(young, 0, mature);
    om.store_object(mature, 0, young);

    ObjectArray roots(0);
    roots.push_back(young);

    om.collect_mature(roots);

    young = roots[0];
    mature = young->at(0);

    TS_ASSERT_EQUALS(mature->at(0), young);
  }

  void test_collect_young_stops_at_already_marked_objects() {
    ObjectMemory om(1024);
    OBJECT obj, obj2;

    obj =  om.new_object(Qnil, 3);
    obj2 = om.new_object(Qnil, 3);

    om.store_object(obj2, 1, Qtrue);

    om.store_object(obj, 0, obj2);
    om.store_object(obj2, 0, obj);

    ObjectArray roots(0);
    roots.push_back(obj);

    om.collect_young(roots);

    obj = roots[0];
    obj2 = obj->at(0);

    TS_ASSERT_EQUALS(obj2->at(0), obj);
    TS_ASSERT_EQUALS(obj2->at(1), Qtrue);
  }

  void test_collect_young_tells_objectmemory_about_collection() {
    ObjectMemory om(128);
    OBJECT obj;
    int left = 128;

    TS_ASSERT_EQUALS(om.collect_young_now, false);

    while(left > 0) {
      obj = om.new_object(Qnil, 3);
      left -= obj->size_in_bytes();
    }

    TS_ASSERT_EQUALS(om.collect_young_now, true);
  }

  void test_new_young_spills_to_mature() {
    ObjectMemory om(128);
    OBJECT obj;
    int left = 128 * 2;

    while(left > 0) {
      obj = om.new_object(Qnil, 3);
      left -= obj->size_in_bytes();
    }

    TS_ASSERT(obj->mature_object_p());
  }


  void test_collect_mature_tells_objectmemory_about_collection() {
    ObjectMemory om(1024);
    OBJECT obj;
    int left = 128;

    om.mature.next_collection_bytes = left;
    om.large_object_threshold = 3;

    TS_ASSERT_EQUALS(om.collect_mature_now, false);

    while(left > 0) {
      obj = om.new_object(Qnil, 4);
      left -= obj->size_in_bytes();
    }

    TS_ASSERT_EQUALS(om.collect_mature_now, true);
  }

  void test_valid_object_p() {
    ObjectMemory om(1024);
    OBJECT obj;

    obj = om.new_object(Qnil, 3);
    TS_ASSERT(om.valid_object_p(obj));

    obj = om.new_object_mature(Qnil, 3);
    TS_ASSERT(om.valid_object_p(obj));

    obj->zone = (gc_zone)0;
    TS_ASSERT(!om.valid_object_p(obj));
  }

};


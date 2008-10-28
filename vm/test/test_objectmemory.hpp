#include <iostream>

#include "vm/gc.hpp"
#include "vm/gc_root.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"

#include "builtin/array.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestObjectMemory : public CxxTest::TestSuite {
  public:

  VM *state;

  void setUp() {
    state = new VM(1024);

  }

  void tearDown() {
    delete state;
  }

  void test_new_object() {
    ObjectMemory om(state, 1024);

    Object* obj;

    TS_ASSERT_EQUALS(om.young.current->used(), 0U);

    obj = om.allocate_object(3);

    TS_ASSERT_EQUALS(obj->num_fields(), 3U);
    TS_ASSERT_EQUALS(obj->zone, YoungObjectZone);

    TS_ASSERT(om.young.current->used() == obj->size_in_bytes());
    TS_ASSERT(om.young.heap_a.used()  == obj->size_in_bytes());
    TS_ASSERT_EQUALS(om.young.current->remaining(), 
         static_cast<unsigned int>(1024 - obj->size_in_bytes() - 1));
  }

  void test_write_barrier() {
    ObjectMemory om(state, 1024);
    Object* obj;
    Object* obj2;

    obj  = om.allocate_object(2);
    obj2 = om.allocate_object(2);
    TS_ASSERT_EQUALS(obj->Remember, 0U);
    TS_ASSERT_EQUALS(obj2->Remember, 0U);
    TS_ASSERT_EQUALS(om.remember_set->size(), 0U);

    obj->zone = MatureObjectZone;
    om.store_object(obj, 0, obj2);

    TS_ASSERT_EQUALS(om.remember_set->size(), 1U);
    TS_ASSERT_EQUALS(obj->Remember, 1U);
    TS_ASSERT_EQUALS(om.remember_set->operator[](0), obj);

    om.store_object(obj, 0, obj2);
    TS_ASSERT_EQUALS(om.remember_set->size(), 1U);
  }

  /* Causes a segfault when fails. */
  void test_write_barrier_not_called_for_immediates() {
    ObjectMemory om(state, 1024);
    Object* obj;
    Object* obj2;

    obj = om.allocate_object(2);
    obj->zone = MatureObjectZone;

    obj2 = Qnil;

    om.store_object(obj, 0, obj2);
    TS_ASSERT_EQUALS(obj->Remember, 0U);
  }

  void test_collect_young() {
    ObjectMemory om(state, 1024);
    Object* obj;
    TS_ASSERT_EQUALS(om.young.current->used(), 0U);

    om.allocate_object(3);
    om.allocate_object(3);
    om.allocate_object(3);
    om.allocate_object(3);
    obj = om.allocate_object(3);

    Heap *cur = om.young.next;
    TS_ASSERT_EQUALS(om.young.current->used(), obj->size_in_bytes() * 5);

    Roots roots;
    om.collect_young(roots);

    TS_ASSERT_EQUALS(om.young.current->used(), 0U);
    TS_ASSERT_EQUALS((void*)cur, (void*)om.young.current);

    obj = om.allocate_object(3);
    TS_ASSERT_EQUALS(obj->age, 0U);
    Root r(&roots, obj);

    om.collect_young(roots);
    obj = roots.front()->get();
    TS_ASSERT_EQUALS(om.young.current->used(), obj->size_in_bytes());
  }

  void test_collect_young_through_references() {
    ObjectMemory om(state, 1024);
    Tuple *obj, *obj2, *obj3;

    obj =  (Tuple*)om.allocate_object(3);
    obj2 = (Tuple*)om.allocate_object(3);

    obj->field[0] = obj2;
    obj2->field[0] = Qtrue;

    om.write_barrier(obj, obj2);

    Roots roots;
    Root r(&roots, obj);

    om.collect_young(roots);

    TS_ASSERT_EQUALS(om.young.current->used(),
                     obj->size_in_bytes() + obj2->size_in_bytes());

    Object* new_obj = roots.front()->get();
    TS_ASSERT(obj != new_obj);
    obj = (Tuple*)new_obj;

    TS_ASSERT(om.young.current->contains_p(obj));
    obj3 = (Tuple*)obj->field[0];
    TS_ASSERT(obj2 != obj3);

    TS_ASSERT_EQUALS(obj2->field[0], Qtrue);
  }

  /* Could crash on failure */
  void test_collect_young_skips_byte_storage() {
    ObjectMemory& om = *state->om;

    Tuple *obj, *obj2;

    obj =  (Tuple*)om.new_object_bytes(G(object), 3);
    obj2 = (Tuple*)om.allocate_object(3);

    /* Force obj2 to appear in the body, but it should be seen as
     * just a generic series of bytes, not a reference. */
    obj->field[0] = obj2;

    Roots roots;
    Root r(&roots, obj);

    om.collect_young(roots);

    obj = (Tuple*)roots.front()->get();
    TS_ASSERT_EQUALS(obj->field[0], obj2);
  }

  void test_new_large_object() {
    ObjectMemory om(state, 1024);
    Object* obj;

    om.large_object_threshold = 10;

    obj = om.allocate_object(20);
    TS_ASSERT_EQUALS(obj->num_fields(), 20U);
    TS_ASSERT_EQUALS(obj->zone, MatureObjectZone);

    TS_ASSERT_EQUALS(om.young.current->used(), 0U);
  }

  void test_collect_young_doesnt_move_mature_objects() {
    ObjectMemory om(state, 1024);
    Object* obj;

    om.large_object_threshold = 10;

    obj = om.allocate_object(20);

    Roots roots;
    Root r(&roots, obj);

    om.collect_young(roots);

    TS_ASSERT_EQUALS(obj, roots.front()->get());
  }

  void test_collect_young_uses_remember_set() {
    ObjectMemory om(state, 1024);
    Tuple *young, *mature;

    om.large_object_threshold = 10;

    young =  (Tuple*)om.allocate_object(3);
    mature = (Tuple*)om.allocate_object(20);

    young->field[0] = Qtrue;
    mature->field[0] = young;

    om.write_barrier(mature, young);
    TS_ASSERT_EQUALS(mature->Remember, 1U);

    Roots roots;
    om.collect_young(roots);

    TS_ASSERT(mature->field[0] != young);
    TS_ASSERT_EQUALS(((Tuple*)mature->field[0])->field[0], Qtrue);
  }

  void test_collect_young_promotes_objects() {
    ObjectMemory om(state, 1024);
    Object* young;

    young = om.allocate_object(3);

    Roots roots;
    Root r(&roots, young);

    om.set_young_lifetime(1);

    TS_ASSERT_EQUALS(young->age, 0U);
    om.collect_young(roots);
    TS_ASSERT_EQUALS(roots.front()->get()->age, 1U);
    om.collect_young(roots);

    TS_ASSERT_EQUALS(roots.front()->get()->age, 0U);

    TS_ASSERT(roots.front()->get()->mature_object_p());
  }

  void test_collect_young_resets_remember_set() {
    ObjectMemory om(state, 1024);
    Tuple *young, *mature;

    om.large_object_threshold = 10;

    young =  (Tuple*)om.allocate_object(3);
    mature = (Tuple*)om.allocate_object(20);

    TS_ASSERT(mature->mature_object_p());
    TS_ASSERT(young->young_object_p());

    mature->field[0] = young;
    om.write_barrier(mature, young);

    Roots roots;

    om.set_young_lifetime(1);

    TS_ASSERT_EQUALS(mature->Remember, 1U);
    TS_ASSERT_EQUALS(om.remember_set->size(), 1U);

    TS_ASSERT_EQUALS(young->age, 0U);
    om.collect_young(roots);
    TS_ASSERT_EQUALS(mature->field[0]->age, 1U);
    om.collect_young(roots);

    TS_ASSERT_EQUALS(om.remember_set->size(), 0U);
  }

  void test_collect_young_uses_forwarding_pointers() {
    ObjectMemory om(state, 1024);
    Tuple *obj, *obj2;

    obj =  (Tuple*)om.allocate_object(3);
    obj2 = (Tuple*)om.allocate_object(3);

    obj->field[0] = obj2;
    obj->field[1] = obj2;
    obj->field[2] = obj2;

    om.write_barrier(obj, obj2);

    Roots roots;
    Root r(&roots, obj);

    om.collect_young(roots);

    obj = (Tuple*)roots.front()->get();

    obj2 = (Tuple*)obj->field[0];
    TS_ASSERT_EQUALS(obj2, obj->field[1]);
    TS_ASSERT_EQUALS(obj2, obj->field[2]);
  }

  void test_collect_young_copies_byte_bodies() {
    ObjectMemory& om = *state->om;

    ByteArray* obj;

    obj = (ByteArray*)om.new_object_bytes(G(object), 3);
    obj->bytes[0] = 47;

    Roots roots;
    Root r(&roots, obj);

    om.collect_young(roots);

    obj = (ByteArray*)roots.front()->get();
    TS_ASSERT_EQUALS(obj->bytes[0], static_cast<char>(47));
  }

  void test_collect_young_class_considered() {
    ObjectMemory om(state, 1024);
    Object* obj;
    Object* cls;

    cls = om.allocate_object(3);
    obj = om.allocate_object(3);
    obj->klass(state, (Class*)cls);

    Roots roots;
    Root r(&roots, obj);

    om.collect_young(roots);

    obj = roots.front()->get();

    TS_ASSERT(om.valid_object_p(obj->klass()));
  }

  void test_collect_mature() {
    ObjectMemory om(state, 1024);
    Object* mature;

    om.debug_marksweep(true);

    om.large_object_threshold = 10;

    mature = om.allocate_object(20);

    /* allocate_object leaves the objs uninitialised */
    mature->klass_ = reinterpret_cast<Class*>(Qnil);
    TS_ASSERT(mature->mature_object_p());
    TS_ASSERT_EQUALS(om.mature.allocated_objects, 1U);

    MarkSweepGC::Entry *entry = om.mature.find_entry(mature);
    TS_ASSERT(entry->unmarked_p());

    Roots roots;
    om.collect_mature(roots);

    TS_ASSERT_EQUALS(om.mature.allocated_objects, 0U);
    TS_ASSERT(entry->unmarked_p());

    /* debug_marksweep() causes it to not free any Entry's, so
     * we have to do it now. */
    delete entry;
  }

  void test_collect_mature_marks_young_objects() {
    ObjectMemory om(state, 1024);
    Object* young;
    Object* mature;

    om.large_object_threshold = 10;

    young =  om.allocate_object(3);
    mature = om.allocate_object(20);

    /* allocate_object leaves the objs uninitialised */
    young->klass_ = reinterpret_cast<Class*>(Qnil);
    mature->klass_ = reinterpret_cast<Class*>(Qnil);

    om.store_object(young, 0, mature);

    Roots roots;
    Root r(&roots, young);

    om.collect_mature(roots);
    TS_ASSERT_EQUALS(young->Marked, 0U);

    TS_ASSERT_EQUALS(om.mature.allocated_objects, 1U);
  }

  /* Could segfault on failure due to infinite loop. */
  void test_collect_mature_stops_at_already_marked_objects() {
    ObjectMemory om(state, 1024);
    Tuple *young, *mature;

    om.large_object_threshold = 10;

    young =  (Tuple*)om.allocate_object(3);
    mature = (Tuple*)om.allocate_object(20);

    young->field[0] = mature;
    mature->field[0] = young;

    /* allocate_object leaves the objs uninitialised */
    young->klass_ = reinterpret_cast<Class*>(Qnil);
    mature->klass_ = reinterpret_cast<Class*>(Qnil);

    om.write_barrier(young, mature);
    om.write_barrier(mature, young);

    Roots roots;
    Root r(&roots, young);

    om.collect_mature(roots);

    young = (Tuple*)roots.front()->get();
    mature = (Tuple*)young->field[0];

    TS_ASSERT_EQUALS(mature->field[0], young);
  }

  void test_collect_young_stops_at_already_marked_objects() {
    ObjectMemory om(state, 1024);
    Tuple *obj, *obj2;

    obj =  (Tuple*)om.allocate_object(3);
    obj2 = (Tuple*)om.allocate_object(3);

    /* allocate_object leaves the objs uninitialised */
    obj->klass_ = reinterpret_cast<Class*>(Qnil);
    obj2->klass_ = reinterpret_cast<Class*>(Qnil);

    obj2->field[1] = Qtrue;
    obj->field[0] = obj2;
    obj2->field[0] = obj;

    om.write_barrier(obj, obj2);
    om.write_barrier(obj2, obj);

    Roots roots;
    Root r(&roots, obj);

    om.collect_young(roots);

    obj = (Tuple*)roots.front()->get();
    obj2 = (Tuple*)obj->field[0];

    TS_ASSERT_EQUALS(obj2->field[0], obj);
    TS_ASSERT_EQUALS(obj2->field[1], Qtrue);
  }

  void test_collect_young_tells_objectmemory_about_collection() {
    ObjectMemory om(state, 128);
    Object* obj;
    int left = 128;

    TS_ASSERT_EQUALS(om.collect_young_now, false);

    while(left > 0) {
      obj = om.allocate_object(3);
      /* allocate_object leaves the objs uninitialised */
      obj->klass_ = reinterpret_cast<Class*>(Qnil);
      left -= obj->size_in_bytes();
    }

    TS_ASSERT_EQUALS(om.collect_young_now, true);
  }

  void test_new_young_spills_to_mature() {
    ObjectMemory om(state, 128);
    Object* obj;
    int left = 128 * 2;

    while(left > 0) {
      obj = om.allocate_object(3);
      /* allocate_object leaves the objs uninitialised */
      obj->klass_ = reinterpret_cast<Class*>(Qnil);
      left -= obj->size_in_bytes();
    }

    TS_ASSERT(obj->mature_object_p());
  }

  void test_collect_mature_tells_objectmemory_about_collection() {
    ObjectMemory om(state, 1024);
    Object* obj;
    int left = 128;

    om.mature.next_collection_bytes = left;
    om.large_object_threshold = 3;

    TS_ASSERT_EQUALS(om.collect_mature_now, false);

    while(left > 0) {
      obj = om.allocate_object(4);
      /* allocate_object leaves the objs uninitialised */
      obj->klass_ = reinterpret_cast<Class*>(Qnil);
      left -= obj->size_in_bytes();
    }

    TS_ASSERT_EQUALS(om.collect_mature_now, true);
  }

  void test_valid_object_p() {
    ObjectMemory om(state, 1024);
    Object* obj;

    obj = om.allocate_object(3);
    TS_ASSERT(om.valid_object_p(obj));

    obj->zone = (gc_zone)0;
    TS_ASSERT(!om.valid_object_p(obj));
  }

  /* Resource cleanup tests */

  void test_new_object_sets_cleanup_flag_if_class_so_indicates()
  {
    TypeInfo* ti = state->om->type_info[ObjectType];

    TS_ASSERT_EQUALS(ti->instances_need_cleanup, false);

    Object* obj = state->om->new_object(G(object), 1);

    TS_ASSERT_EQUALS(obj->RequiresCleanup, false);

    ti->instances_need_cleanup = true;

    Object* obj2 = state->om->new_object(G(object), 1);

    TS_ASSERT_EQUALS(obj2->RequiresCleanup, true);
    TS_ASSERT_EQUALS(obj->RequiresCleanup, false);

    ti->instances_need_cleanup = false;
  }

  /* TODO: Move this to test_gc when such exists. */

    /* eep */
    class Cleanupper : public TypeInfo {
    public:
      Object* squeaky;

      Cleanupper() : TypeInfo(ObjectType, true), squeaky(NULL) {}

      virtual void cleanup(Object* obj) { squeaky = obj; }
    };

  void test_gc_calls_cleanup_function_if_object_requires_cleanup()
  {
    /* Double eep */
    Cleanupper* c = new Cleanupper();

    TypeInfo* ti = state->om->type_info[ObjectType];
    state->om->type_info[ObjectType] = c;

    Object* obj = state->om->new_object(G(object), 1);

    TS_ASSERT_EQUALS(obj->RequiresCleanup, true);

    state->om->mature.delete_object(obj);

    TS_ASSERT_EQUALS(c->squeaky, obj);

    state->om->type_info[ObjectType] = ti;
  }

  void test_contexts_initialized() {
    TS_ASSERT(state->om->contexts.scan <= state->om->contexts.current);
  }

  void test_xmalloc_causes_gc() {
    // Knows that the threshold is 10M
    state->interrupts.check = false;
    void* ptr = XMALLOC(1000000);
    XFREE(ptr);
    TS_ASSERT_EQUALS(state->interrupts.check, false);
    ptr = XMALLOC(9000001);
    TS_ASSERT_EQUALS(state->interrupts.check, true);
    XFREE(ptr);
  }

};


#include "vm/test/test.hpp"

#include <iostream>

#include "gc/gc.hpp"
#include "gc/root.hpp"
#include "object_utils.hpp"

#include "builtin/array.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestObjectMemory : public CxxTest::TestSuite, public VMTest {
public:

  GCData* gc_data;
  Roots* roots;
  CallFrameLocationList call_frames;
  VariableRootBuffers variable_buffers;

  void setUp() {
    create();
    roots = new Roots;
    gc_data = new GCData(*roots, call_frames, variable_buffers);
  }

  void tearDown() {
    delete gc_data;
    delete roots;
    destroy();
  }

  Object* util_new_object(ObjectMemory &om, int count = 3) {
    return om.new_object_variable<Tuple>((Class*)Qnil, count);
  }

  void test_new_object() {
    ObjectMemory om(state, 1024);

    Object* obj;

    TS_ASSERT_EQUALS(om.young.current->used(), 0U);

    obj = util_new_object(om);

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

    obj  = util_new_object(om);
    obj2 = util_new_object(om);
    TS_ASSERT_EQUALS(obj->remembered_p(), 0U);
    TS_ASSERT_EQUALS(obj2->remembered_p(), 0U);
    TS_ASSERT_EQUALS(om.remember_set->size(), 0U);

    obj->zone = MatureObjectZone;
    om.store_object(obj, 0, obj2);

    TS_ASSERT_EQUALS(om.remember_set->size(), 1U);
    TS_ASSERT_EQUALS(obj->remembered_p(), 1U);
    TS_ASSERT_EQUALS(om.remember_set->operator[](0), obj);

    om.store_object(obj, 0, obj2);
    TS_ASSERT_EQUALS(om.remember_set->size(), 1U);
  }

  /* Causes a segfault when fails. */
  void test_write_barrier_not_called_for_immediates() {
    ObjectMemory om(state, 1024);
    Object* obj;
    Object* obj2;

    obj = util_new_object(om);
    obj->zone = MatureObjectZone;

    obj2 = Qnil;

    om.store_object(obj, 0, obj2);
    TS_ASSERT_EQUALS(obj->remembered_p(), 0U);
  }

  void test_collect_young() {
    ObjectMemory om(state, 1024);
    Object* obj;
    TS_ASSERT_EQUALS(om.young.current->used(), 0U);

    util_new_object(om);
    util_new_object(om);
    util_new_object(om);
    util_new_object(om);
    obj = util_new_object(om);

    Heap *cur = om.young.next;
    TS_ASSERT_EQUALS(om.young.current->used(), obj->size_in_bytes() * 5);

    om.collect_young(*gc_data);

    TS_ASSERT_EQUALS(om.young.current->used(), 0U);
    TS_ASSERT_EQUALS((void*)cur, (void*)om.young.current);

    obj = util_new_object(om);
    TS_ASSERT_EQUALS(obj->age, 0U);
    Root r(roots, obj);

    om.collect_young(*gc_data);
    obj = roots->front()->get();
    TS_ASSERT_EQUALS(om.young.current->used(), obj->size_in_bytes());
  }

  void test_collect_young_through_references() {
    ObjectMemory om(state, 1024);
    Tuple *obj, *obj2, *obj3;

    obj =  (Tuple*)util_new_object(om);
    obj2 = (Tuple*)util_new_object(om);

    obj->field[0] = obj2;
    obj2->field[0] = Qtrue;

    om.write_barrier(obj, obj2);

    Root r(roots, obj);

    om.collect_young(*gc_data);

    TS_ASSERT_EQUALS(om.young.current->used(),
                     obj->size_in_bytes() + obj2->size_in_bytes());

    Object* new_obj = roots->front()->get();
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

    //ByteArray *obj, *obj2;

    //obj =  om.new_object_bytes<ByteArray>(G(object), sizeof(Object*));
    //obj2 = om.new_object_bytes<ByteArray>(G(object), sizeof(Object*));

    /* Force obj2 to appear in the body, but it should be seen as
     * just a generic series of bytes, not a reference. */
    //Tuple* tup = reinterpret_cast<Tuple*>(obj);
    //tup->field[0] = obj2;

    //Root r(&roots, obj);

    om.collect_young(*gc_data);

    //tup = reinterpret_cast<Tuple*>(roots->front()->get());
    //TS_ASSERT_EQUALS(tup->field[0], obj2);
  }

  void test_new_large_object() {
    ObjectMemory om(state, 1024);
    Object* obj;

    om.large_object_threshold = 10;

    obj = util_new_object(om,20);
    TS_ASSERT_EQUALS(obj->num_fields(), 20U);
    TS_ASSERT_EQUALS(obj->zone, MatureObjectZone);

    TS_ASSERT_EQUALS(om.young.current->used(), 0U);
  }

  void test_collect_young_doesnt_move_mature_objects() {
    ObjectMemory om(state, 1024);
    Object* obj;

    om.large_object_threshold = 10;

    obj = util_new_object(om,20);

    Root r(roots, obj);

    om.collect_young(*gc_data);

    TS_ASSERT_EQUALS(obj, roots->front()->get());
  }

  void test_collect_young_uses_remember_set() {
    ObjectMemory om(state, 1024);
    Tuple *young, *mature;

    om.large_object_threshold = 50 * __WORDSIZE / 32;

    young =  (Tuple*)util_new_object(om);
    TS_ASSERT_EQUALS(young->zone, YoungObjectZone);
    mature = (Tuple*)util_new_object(om,20);
    TS_ASSERT_EQUALS(mature->zone, MatureObjectZone);

    young->field[0] = Qtrue;
    mature->field[0] = young;

    om.write_barrier(mature, young);
    TS_ASSERT_EQUALS(mature->remembered_p(), 1U);

    om.collect_young(*gc_data);

    TS_ASSERT(mature->field[0] != young);
    TS_ASSERT_EQUALS(((Tuple*)mature->field[0])->field[0], Qtrue);
  }

  void test_collect_young_promotes_objects() {
    ObjectMemory om(state, 1024);
    Object* young;

    young = util_new_object(om);

    Root r(roots, young);

    om.set_young_lifetime(1);

    TS_ASSERT_EQUALS(young->age, 0U);
    om.collect_young(*gc_data);
    TS_ASSERT_EQUALS(roots->front()->get()->age, 1U);
    om.collect_young(*gc_data);

    TS_ASSERT_EQUALS(roots->front()->get()->age, 0U);

    TS_ASSERT(roots->front()->get()->mature_object_p());
  }

  void test_collect_young_resets_remember_set() {
    ObjectMemory om(state, 1024);
    Tuple *young, *mature;

    om.large_object_threshold = 50 * __WORDSIZE / 32;

    young =  (Tuple*)util_new_object(om);
    mature = (Tuple*)util_new_object(om,20);

    TS_ASSERT(mature->mature_object_p());
    TS_ASSERT(young->young_object_p());

    mature->field[0] = young;
    om.write_barrier(mature, young);

    om.set_young_lifetime(1);

    TS_ASSERT_EQUALS(mature->remembered_p(), 1U);
    TS_ASSERT_EQUALS(om.remember_set->size(), 1U);

    TS_ASSERT_EQUALS(young->age, 0U);
    om.collect_young(*gc_data);
    TS_ASSERT_EQUALS(mature->field[0]->age, 1U);
    om.collect_young(*gc_data);

    TS_ASSERT_EQUALS(om.remember_set->size(), 0U);
  }

  void test_collect_young_uses_forwarding_pointers() {
    ObjectMemory om(state, 1024);
    Tuple *obj, *obj2;

    obj =  (Tuple*)util_new_object(om);
    obj2 = (Tuple*)util_new_object(om);

    obj->field[0] = obj2;
    obj->field[1] = obj2;
    obj->field[2] = obj2;

    om.write_barrier(obj, obj2);

    Root r(roots, obj);

    om.collect_young(*gc_data);

    obj = (Tuple*)roots->front()->get();

    obj2 = (Tuple*)obj->field[0];
    TS_ASSERT_EQUALS(obj2, obj->field[1]);
    TS_ASSERT_EQUALS(obj2, obj->field[2]);
  }

  void test_collect_young_copies_byte_bodies() {
    ObjectMemory& om = *state->om;

    ByteArray* obj;

    obj = om.new_object_bytes<ByteArray>(G(object), 3);
    obj->bytes[0] = 47;

    Root r(roots, obj);

    om.collect_young(*gc_data);

    obj = (ByteArray*)roots->front()->get();
    TS_ASSERT_EQUALS(obj->bytes[0], static_cast<char>(47));
  }

  void test_collect_young_class_considered() {
    ObjectMemory om(state, 1024);
    Object* obj;
    Object* cls;

    cls = util_new_object(om);
    obj = util_new_object(om);
    obj->klass(state, (Class*)cls);

    Root r(roots, obj);

    om.collect_young(*gc_data);

    obj = roots->front()->get();

    TS_ASSERT(om.valid_object_p(obj->klass()));
  }

  void test_collect_mature() {
    ObjectMemory om(state, 1024);
    Object* mature;

    om.debug_marksweep(true);

    om.large_object_threshold = 10;

    mature = util_new_object(om,20);

    /* allocate_object leaves the objs uninitialised */
    mature->klass_ = reinterpret_cast<Class*>(Qnil);
    TS_ASSERT(mature->mature_object_p());
    TS_ASSERT_EQUALS(om.mark_sweep_.allocated_objects, 1U);

    TS_ASSERT(!mature->marked_p());

    om.collect_mature(*gc_data);

    TS_ASSERT_EQUALS(om.mark_sweep_.allocated_objects, 0U);
    TS_ASSERT(!mature->marked_p());
  }

  void test_collect_mature_marks_young_objects() {
    ObjectMemory om(state, 1024);
    Object* young;
    Object* mature;

    om.large_object_threshold = 50 * __WORDSIZE / 32;

    young =  util_new_object(om);
    mature = util_new_object(om,20);

    /* allocate_object leaves the objs uninitialised */
    young->klass_ = reinterpret_cast<Class*>(Qnil);
    mature->klass_ = reinterpret_cast<Class*>(Qnil);

    om.store_object(young, 0, mature);

    Root r(roots, young);

    om.collect_mature(*gc_data);
    TS_ASSERT_EQUALS(young->marked_p(), 0U);

    TS_ASSERT_EQUALS(om.mark_sweep_.allocated_objects, 1U);
  }

  /* Could segfault on failure due to infinite loop. */
  void test_collect_mature_stops_at_already_marked_objects() {
    ObjectMemory om(state, 1024);
    Tuple *young, *mature;

    om.large_object_threshold = 50;

    young =  (Tuple*)util_new_object(om);
    mature = (Tuple*)util_new_object(om,20);

    young->field[0] = mature;
    mature->field[0] = young;

    /* allocate_object leaves the objs uninitialised */
    young->klass_ = reinterpret_cast<Class*>(Qnil);
    mature->klass_ = reinterpret_cast<Class*>(Qnil);

    om.write_barrier(young, mature);
    om.write_barrier(mature, young);

    Root r(roots, young);

    om.collect_mature(*gc_data);

    young = (Tuple*)roots->front()->get();
    mature = (Tuple*)young->field[0];

    TS_ASSERT_EQUALS(mature->field[0], young);
  }

  void test_collect_young_stops_at_already_marked_objects() {
    ObjectMemory om(state, 1024);
    Tuple *obj, *obj2;

    obj =  (Tuple*)util_new_object(om);
    obj2 = (Tuple*)util_new_object(om);

    /* allocate_object leaves the objs uninitialised */
    obj->klass_ = reinterpret_cast<Class*>(Qnil);
    obj2->klass_ = reinterpret_cast<Class*>(Qnil);

    obj2->field[1] = Qtrue;
    obj->field[0] = obj2;
    obj2->field[0] = obj;

    om.write_barrier(obj, obj2);
    om.write_barrier(obj2, obj);

    Root r(roots, obj);

    om.collect_young(*gc_data);

    obj = (Tuple*)roots->front()->get();
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
      obj = util_new_object(om);
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
      obj = util_new_object(om);
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

    om.mark_sweep_.next_collection_bytes = left - 1;
    om.large_object_threshold = 3;

    TS_ASSERT_EQUALS(om.collect_mature_now, false);

    while(left > 0) {
      obj = util_new_object(om);
      /* allocate_object leaves the objs uninitialised */
      obj->klass_ = reinterpret_cast<Class*>(Qnil);
      left -= obj->size_in_bytes();
    }

    TS_ASSERT_EQUALS(om.collect_mature_now, true);
  }

  void test_valid_object_p() {
    ObjectMemory om(state, 1024);
    Object* obj;

    obj = util_new_object(om);
    TS_ASSERT(om.valid_object_p(obj));

    obj->zone = (gc_zone)0;
    TS_ASSERT(!om.valid_object_p(obj));
  }

  /* Resource cleanup tests */

  void test_new_object_sets_cleanup_flag_if_class_so_indicates()
  {
    TypeInfo* ti = state->om->type_info[ObjectType];

    TS_ASSERT_EQUALS(ti->instances_need_cleanup, false);

    Object* obj = state->new_object<Object>(G(object));

    TS_ASSERT_EQUALS(obj->requires_cleanup_p(), false);

    ti->instances_need_cleanup = true;

    Object* obj2 = state->new_object<Object>(G(object));

    TS_ASSERT_EQUALS(obj2->requires_cleanup_p(), true);
    TS_ASSERT_EQUALS(obj->requires_cleanup_p(), false);

    ti->instances_need_cleanup = false;
  }

  /* TODO: Move this to test_gc when such exists. */

  /* eep */
  class Cleanupper : public TypeInfo {
  public:
    Object* squeaky;

    Cleanupper() : TypeInfo(ObjectType, true), squeaky(NULL) {}

    virtual void cleanup(Object* obj) { squeaky = obj; }
    virtual void auto_mark(Object* obj, ObjectMark& mark) {}
  };

  void test_gc_calls_cleanup_function_if_object_requires_cleanup()
  {
    /* Double eep */
    Cleanupper* c = new Cleanupper();

    TypeInfo* ti = state->om->type_info[ObjectType];
    state->om->type_info[ObjectType] = c;

    Object* obj = state->new_object<Object>(G(object));

    TS_ASSERT_EQUALS(obj->requires_cleanup_p(), true);

    state->om->mark_sweep_.delete_object(obj);

    TS_ASSERT_EQUALS(c->squeaky, obj);

    state->om->type_info[ObjectType] = ti;
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


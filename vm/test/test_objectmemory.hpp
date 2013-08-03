#include "vm/test/test.hpp"

#include <iostream>

#include "gc/gc.hpp"
#include "gc/root.hpp"
#include "gc/baker.hpp"
#include "gc/marksweep.hpp"
#include "capi/handles.hpp"

#include "object_utils.hpp"

#include "builtin/array.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestObjectMemory : public CxxTest::TestSuite, public VMTest {
public:

  GCData* gc_data;
  Roots* roots;
  VariableRootBuffers variable_buffers;
  capi::Handles handles;
  std::list<capi::Handle*> cached_handles;

  void setUp() {
    create();
    roots = &state->globals().roots;
    gc_data = new GCData(*roots, &handles,
                         &cached_handles, state->vm()->global_cache());
  }

  void tearDown() {
    if(gc_data) {
      delete gc_data;
    }
    destroy();
  }

  Tuple* util_new_object(ObjectMemory &om, size_t count = 3) {
    return Tuple::create(state, count);
  }

  void test_new_object() {
    ObjectMemory& om = *state->memory();

    Tuple* obj;

    obj = util_new_object(om);

    TS_ASSERT_EQUALS(obj->num_fields(), 3);
    TS_ASSERT_EQUALS(obj->zone(), YoungObjectZone);
  }

  void test_write_barrier() {
    ObjectMemory& om = *state->memory();
    Object* obj;
    Object* obj2;

    obj  = util_new_object(om);
    obj2 = util_new_object(om);
    TS_ASSERT_EQUALS(obj->remembered_p(), 0U);
    TS_ASSERT_EQUALS(obj2->remembered_p(), 0U);
    size_t start = om.remember_set()->size();

    obj->set_zone(MatureObjectZone);
    om.write_barrier(obj, obj2);

    TS_ASSERT_EQUALS(om.remember_set()->size(), start + 1);
    TS_ASSERT_EQUALS(obj->remembered_p(), 1U);

    om.write_barrier(obj, obj2);
    TS_ASSERT_EQUALS(om.remember_set()->size(), start + 1);
  }

  /* Causes a segfault when fails. */
  void test_write_barrier_not_called_for_immediates() {
    ObjectMemory& om = *state->memory();
    Object* obj;
    Object* obj2;

    obj = util_new_object(om);
    obj->set_zone(MatureObjectZone);

    obj2 = cNil;

    om.write_barrier(obj, obj2);
    TS_ASSERT_EQUALS(obj->remembered_p(), 0U);
  }

  void test_collect_young() {
    ObjectMemory& om = *state->memory();
    Object* obj;
    size_t start = om.young_->bytes_used();

    util_new_object(om);
    util_new_object(om);
    util_new_object(om);
    util_new_object(om);
    util_new_object(om);

    om.collect_young(state, gc_data);

    TS_ASSERT(om.young_->bytes_used() <= start);

    obj = util_new_object(om);
    TS_ASSERT_EQUALS(obj->age(), 0U);
    Root r(roots, obj);

    om.collect_young(state, gc_data);
    TS_ASSERT(obj->forwarded_p());
  }

  void test_collect_young_through_references() {
    ObjectMemory& om = *state->memory();
    Tuple *obj, *obj2, *obj3;

    obj =  as<Tuple>(util_new_object(om));
    obj2 = as<Tuple>(util_new_object(om));

    obj->field[0] = obj2;
    obj2->field[0] = cTrue;

    om.write_barrier(obj, obj2);

    Root r(roots, obj);

    om.collect_young(state, gc_data);

    TS_ASSERT(obj->forwarded_p());
    TS_ASSERT(obj2->forwarded_p());

    Object* new_obj = roots->front()->get();
    TS_ASSERT(obj != new_obj);
    obj = as<Tuple>(new_obj);

    TS_ASSERT(om.young_->in_current_p(obj));
    obj3 = as<Tuple>(obj->field[0]);
    TS_ASSERT(obj2 != obj3);

    TS_ASSERT_EQUALS(obj2->field[0], cTrue);
  }

  /* Could crash on failure */
  void test_collect_young_skips_byte_storage() {
    ObjectMemory& om = *state->memory();

    //ByteArray *obj, *obj2;

    //obj =  om.new_object_bytes<ByteArray>(G(object), sizeof(Object*));
    //obj2 = om.new_object_bytes<ByteArray>(G(object), sizeof(Object*));

    /* Force obj2 to appear in the body, but it should be seen as
     * just a generic series of bytes, not a reference. */
    //Tuple* tup = reinterpret_cast<Tuple*>(obj);
    //tup->field[0] = obj2;

    //Root r(&roots, obj);

    om.collect_young(state, gc_data);

    //tup = reinterpret_cast<Tuple*>(roots->front()->get());
    //TS_ASSERT_EQUALS(tup->field[0], obj2);
  }

  void test_new_large_object() {
    ObjectMemory& om = *state->memory();
    Tuple* obj;

    om.large_object_threshold = 10;

    size_t start = om.young_->bytes_used();

    obj = util_new_object(om,20);
    TS_ASSERT_EQUALS(obj->num_fields(), 20);
    TS_ASSERT_EQUALS(obj->zone(), MatureObjectZone);

    TS_ASSERT_EQUALS(om.young_->bytes_used(), start);
  }

  void test_collect_young_doesnt_move_mature_objects() {
    ObjectMemory& om = *state->memory();
    Object* obj;

    om.large_object_threshold = 10;

    obj = util_new_object(om,20);

    Root r(roots, obj);

    om.collect_young(state, gc_data);

    TS_ASSERT_EQUALS(obj, roots->front()->get());
  }

  void test_collect_young_uses_remember_set() {
    ObjectMemory& om = *state->memory();
    Tuple *young, *mature;

    om.large_object_threshold = sizeof(void *) * 50 * 8 / 32;

    young =  as<Tuple>(util_new_object(om));
    TS_ASSERT_EQUALS(young->zone(), YoungObjectZone);
    mature = as<Tuple>(util_new_object(om,20));
    TS_ASSERT_EQUALS(mature->zone(), MatureObjectZone);

    young->field[0] = cTrue;
    mature->field[0] = young;

    om.write_barrier(mature, young);
    TS_ASSERT_EQUALS(mature->remembered_p(), 1U);

    om.collect_young(state, gc_data);

    TS_ASSERT(mature->field[0] != young);
    TS_ASSERT_EQUALS((as<Tuple>(mature->field[0])->field[0]), cTrue);
  }

  void test_collect_young_promotes_objects() {
    ObjectMemory& om = *state->memory();
    Object* young;

    young = util_new_object(om);

    Root r(roots, young);

    om.set_young_lifetime(1);

    TS_ASSERT_EQUALS(young->age(), 0U);
    om.collect_young(state, gc_data);
    TS_ASSERT_EQUALS(roots->front()->get()->age(), 1U);
    om.collect_young(state, gc_data);

    TS_ASSERT(roots->front()->get()->mature_object_p());
  }

  void test_collect_young_resets_remember_set() {
    ObjectMemory& om = *state->memory();
    Tuple *young, *mature;

    om.large_object_threshold = sizeof(void *) * 50 * 8 / 32;

    young =  as<Tuple>(util_new_object(om));
    mature = as<Tuple>(util_new_object(om,20));

    TS_ASSERT(mature->mature_object_p());
    TS_ASSERT(young->young_object_p());

    mature->field[0] = young;
    om.write_barrier(mature, young);

    om.set_young_lifetime(1);

    TS_ASSERT_EQUALS(mature->remembered_p(), 1U);

    TS_ASSERT_EQUALS(young->age(), 0U);
    om.collect_young(state, gc_data);
    TS_ASSERT_EQUALS(mature->field[0]->age(), 1U);
    om.collect_young(state, gc_data);
  }

  void test_collect_young_uses_forwarding_pointers() {
    ObjectMemory& om = *state->memory();
    Tuple *obj, *obj2;

    obj =  as<Tuple>(util_new_object(om));
    obj2 = as<Tuple>(util_new_object(om));

    obj->field[0] = obj2;
    obj->field[1] = obj2;
    obj->field[2] = obj2;

    om.write_barrier(obj, obj2);

    Root r(roots, obj);

    om.collect_young(state, gc_data);

    obj = as<Tuple>(roots->front()->get());

    obj2 = as<Tuple>(obj->field[0]);
    TS_ASSERT_EQUALS(obj2, obj->field[1]);
    TS_ASSERT_EQUALS(obj2, obj->field[2]);
  }

  void test_collect_young_copies_byte_bodies() {
    ObjectMemory& om = *state->memory();

    ByteArray* obj;

    obj = ByteArray::create(state, 3);
    obj->raw_bytes()[0] = 47;

    Root r(roots, obj);

    om.collect_young(state, gc_data);

    obj = as<ByteArray>(roots->front()->get());
    TS_ASSERT_EQUALS(obj->raw_bytes()[0], static_cast<char>(47));
  }

  void test_collect_mature() {
    ObjectMemory& om = *state->memory();
    Object* mature;

    om.debug_marksweep(true);

    om.large_object_threshold = 10;

    mature = util_new_object(om,20);

    TS_ASSERT(mature->mature_object_p());
    unsigned int mark = om.mark();
    TS_ASSERT(!mature->marked_p(mark));
    Root r(roots, mature);

    om.collect_mature(state, gc_data);
    om.wait_for_mature_marker(state);
    // marker thread cleans up gc_data
    gc_data = NULL;

    TS_ASSERT(mature->marked_p(mark));
  }

  void test_collect_mature_marks_young_objects() {
    ObjectMemory& om = *state->memory();
    Tuple* young;
    Object* mature;

    om.large_object_threshold = sizeof(void *) * 50 * 8 / 32;

    young =  util_new_object(om);
    mature = util_new_object(om,20);

    young->field[0] = mature; // dangerous, but ok in tests

    unsigned int mark = om.mark();
    TS_ASSERT(!young->marked_p(mark));
    Root r(roots, young);

    om.collect_mature(state, gc_data);
    om.wait_for_mature_marker(state);
    gc_data = NULL;

    TS_ASSERT(young->marked_p(mark));
  }

  /* Could segfault on failure due to infinite loop. */
  void test_collect_mature_stops_at_already_marked_objects() {
    ObjectMemory& om = *state->memory();
    Tuple *young, *mature;

    om.large_object_threshold = 50;

    young =  as<Tuple>(util_new_object(om));
    mature = as<Tuple>(util_new_object(om,20));

    young->field[0] = mature;
    mature->field[0] = young;

    om.write_barrier(young, mature);
    om.write_barrier(mature, young);

    Root r(roots, young);

    om.collect_mature(state, gc_data);
    om.wait_for_mature_marker(state);
    gc_data = NULL;

    mature = as<Tuple>(young->field[0]);

    TS_ASSERT_EQUALS(mature->field[0], young);
  }

  void test_collect_young_stops_at_already_marked_objects() {
    ObjectMemory& om = *state->memory();
    Tuple *obj, *obj2;

    obj =  as<Tuple>(util_new_object(om));
    obj2 = as<Tuple>(util_new_object(om));

    obj2->field[1] = cTrue;
    obj->field[0] = obj2;
    obj2->field[0] = obj;

    om.write_barrier(obj, obj2);
    om.write_barrier(obj2, obj);

    Root r(roots, obj);

    om.collect_young(state, gc_data);

    obj = as<Tuple>(roots->front()->get());
    obj2 = as<Tuple>(obj->field[0]);

    TS_ASSERT_EQUALS(obj2->field[0], obj);
    TS_ASSERT_EQUALS(obj2->field[1], cTrue);
  }

  void test_valid_object_p() {
    ObjectMemory& om = *state->memory();
    Object* obj;

    obj = util_new_object(om);
    TS_ASSERT(om.valid_object_p(obj));

    obj->set_zone((gc_zone)0);
    TS_ASSERT(!om.valid_object_p(obj));
  }

  void test_xmalloc_causes_gc() {
    // Knows that the threshold is 10M
    state->shared().check_gc_p();

    int bytes = 1024 * 1024;
    int total = 110 * bytes;
    while(total >= 0) {
      XFREE(XMALLOC(bytes));
      total -= bytes;
    }

    TS_ASSERT_EQUALS(state->shared().check_gc_p(), true);
  }

};


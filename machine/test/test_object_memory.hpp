#include "machine/test/test.hpp"

#include <iostream>

#include "memory/gc.hpp"
#include "memory/root.hpp"
#include "memory/mark_sweep.hpp"

#include "object_utils.hpp"

#include "class/array.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestMemory : public CxxTest::TestSuite, public VMTest {
public:

  memory::GCData* gc_data;
  memory::Roots* roots;
  memory::VariableRootBuffers variable_buffers;

  void setUp() {
    create();
    roots = &state->globals().roots;
    gc_data = new memory::GCData(state->vm());
  }

  void tearDown() {
    if(gc_data) {
      delete gc_data;
    }
    destroy();
  }

  Tuple* util_new_object(Memory &om, size_t count = 3) {
    return Tuple::create(state, count);
  }

  /* TODO: young gen
  void xtest_new_object() {
    Memory& om = *state->memory();

    om.collect_young(state, gc_data);

    Tuple* obj = util_new_object(om);

    TS_ASSERT_EQUALS(obj->num_fields(), 3);
    TS_ASSERT_EQUALS(obj->zone(), YoungObjectZone);
  }
  */

  /* TODO: young gen
  void xtest_write_barrier() {
    Memory& om = *state->memory();
    Object* obj;
    Object* obj2;

    om.collect_young(state, gc_data);

    obj  = util_new_object(om);
    obj2 = util_new_object(om);
    TS_ASSERT_EQUALS(obj->remembered_p(), 0U);
    TS_ASSERT_EQUALS(obj2->remembered_p(), 0U);
    size_t start = om.remember_set()->size();

    obj->set_zone(MatureObjectZone);
    TS_ASSERT(obj2->young_object_p());

    om.write_barrier(obj, obj2);

    TS_ASSERT_EQUALS(om.remember_set()->size(), start + 1);
    TS_ASSERT_EQUALS(obj->remembered_p(), 1U);

    om.write_barrier(obj, obj2);
    TS_ASSERT_EQUALS(om.remember_set()->size(), start + 1);
  }
  */

  /* Causes a segfault when fails. */
  /* TODO: MemoryHeader
  void xtest_write_barrier_not_called_for_immediates() {
    Memory& om = *state->memory();
    Object* obj;
    Object* obj2;

    obj = util_new_object(om);
    obj->set_zone(MatureObjectZone);

    obj2 = cNil;

    om.write_barrier(obj, obj2);
    TS_ASSERT_EQUALS(obj->remembered_p(), 0U);
  }
  */

  /* TODO: young gen
  void xtest_collect_young() {
    Memory& om = *state->memory();
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
    memory::Root r(roots, obj);

    om.collect_young(state, gc_data);
    TS_ASSERT(obj->forwarded_p());
  }
  */

  /* TODO: young gen
  void xtest_collect_young_through_references() {
    Memory& om = *state->memory();
    Tuple *obj, *obj2, *obj3;

    om.collect_young(state, gc_data);

    obj =  as<Tuple>(util_new_object(om));
    obj2 = as<Tuple>(util_new_object(om));

    obj->field[0] = obj2;
    obj2->field[0] = cTrue;

    TS_ASSERT(obj->young_object_p());
    TS_ASSERT(obj2->young_object_p());

    memory::Root r(roots, obj);

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
  */

#define LARGE_OBJECT_BYTE_SIZE   30 * 1024 * 1024

  void test_new_large_object() {
    Memory& om = *state->memory();
    Tuple* obj;

  /* TODO: young gen
    size_t start = om.young_->bytes_used();
    */

    obj = util_new_object(om, LARGE_OBJECT_BYTE_SIZE);
    TS_ASSERT_EQUALS(obj->num_fields(), LARGE_OBJECT_BYTE_SIZE);
    TS_ASSERT_EQUALS(obj->region(), eLargeRegion);

  /* TODO: young gen
    TS_ASSERT_EQUALS(om.young_->bytes_used(), start);
    */
  }

  void test_collect_young_doesnt_move_mature_objects() {
    Memory& om = *state->memory();
    Object* obj;

    obj = util_new_object(om, LARGE_OBJECT_BYTE_SIZE);

    memory::Root r(roots, obj);

    om.collect_young(state, gc_data);

    TS_ASSERT_EQUALS(obj, roots->front()->get());
  }

  /* TODO: young gen
  void xtest_collect_young_uses_remember_set() {
    Memory& om = *state->memory();
    Tuple *young, *mature;

    om.collect_young(state, gc_data);

    young =  as<Tuple>(util_new_object(om));
    TS_ASSERT_EQUALS(young->zone(), YoungObjectZone);

    mature = as<Tuple>(util_new_object(om, LARGE_OBJECT_BYTE_SIZE));
    TS_ASSERT_EQUALS(mature->zone(), MatureObjectZone);

    young->field[0] = cTrue;
    mature->field[0] = young;

    om.write_barrier(mature, young);
    TS_ASSERT_EQUALS(mature->remembered_p(), 1U);

    om.collect_young(state, gc_data);

    TS_ASSERT(mature->field[0] != young);
    TS_ASSERT_EQUALS((as<Tuple>(mature->field[0])->field[0]), cTrue);
  }
  */

  /* TODO: young gen
  void xtest_collect_young_promotes_objects() {
    Memory& om = *state->memory();
    Object* young;

    om.collect_young(state, gc_data);

    young = util_new_object(om);

    memory::Root r(roots, young);

    TS_ASSERT_EQUALS(young->age(), 0U);
    om.collect_young(state, gc_data);
    TS_ASSERT_EQUALS(roots->front()->get()->age(), 1U);
    om.collect_young(state, gc_data);
    TS_ASSERT_EQUALS(roots->front()->get()->age(), 2U);
    om.collect_young(state, gc_data);

    TS_ASSERT(roots->front()->get()->mature_object_p());
  }
  */

  /* TODO: young gen
  void xtest_collect_young_resets_remember_set() {
    Memory& om = *state->memory();
    Tuple *young, *mature;

    om.collect_young(state, gc_data);

    young =  as<Tuple>(util_new_object(om));
    mature = as<Tuple>(util_new_object(om, LARGE_OBJECT_BYTE_SIZE));

    TS_ASSERT(mature->mature_object_p());
    TS_ASSERT(young->young_object_p());

    mature->field[0] = young;
    om.write_barrier(mature, young);

    TS_ASSERT_EQUALS(mature->remembered_p(), 1U);

    TS_ASSERT_EQUALS(young->age(), 0U);
    om.collect_young(state, gc_data);
    TS_ASSERT_EQUALS(mature->field[0]->age(), 1U);
    om.collect_young(state, gc_data);
    TS_ASSERT_EQUALS(mature->field[0]->age(), 2U);
    om.collect_young(state, gc_data);
  }
  */

  /* TODO: young gen
  void xtest_collect_young_uses_forwarding_pointers() {
    Memory& om = *state->memory();
    Tuple *obj, *obj2;

    obj =  as<Tuple>(util_new_object(om));
    obj2 = as<Tuple>(util_new_object(om));

    obj->field[0] = obj2;
    obj->field[1] = obj2;
    obj->field[2] = obj2;

    om.write_barrier(obj, obj2);

    memory::Root r(roots, obj);

    om.collect_young(state, gc_data);

    obj = as<Tuple>(roots->front()->get());

    obj2 = as<Tuple>(obj->field[0]);
    TS_ASSERT_EQUALS(obj2, obj->field[1]);
    TS_ASSERT_EQUALS(obj2, obj->field[2]);
  }
  */

  /* TODO: young gen
  void xtest_collect_young_copies_byte_bodies() {
    Memory& om = *state->memory();

    ByteArray* obj;

    obj = ByteArray::create(state, 3);
    obj->raw_bytes()[0] = 47;

    memory::Root r(roots, obj);

    om.collect_young(state, gc_data);

    obj = as<ByteArray>(roots->front()->get());
    TS_ASSERT_EQUALS(obj->raw_bytes()[0], static_cast<char>(47));
  }
  */

  void test_collect_full() {
    Memory& om = *state->memory();
    Object* mature;

    om.large_object_threshold = 10;

    mature = util_new_object(om, LARGE_OBJECT_BYTE_SIZE);

    TS_ASSERT_EQUALS(mature->region(), eLargeRegion);
    unsigned int mark = om.mark();
    TS_ASSERT(!mature->marked_p(mark));
    memory::Root r(roots, mature);

    om.collect_full(state);
    // marker thread cleans up gc_data
    gc_data = NULL;

    TS_ASSERT(mature->marked_p(mark));
  }

  void test_collect_full_marks_young_objects() {
    Memory& om = *state->memory();
    Tuple* young;
    Object* mature;

    young =  util_new_object(om);
    mature = util_new_object(om, LARGE_OBJECT_BYTE_SIZE);

    young->field[0] = mature; // dangerous, but ok in tests

    unsigned int mark = om.mark();
    TS_ASSERT(!young->marked_p(mark));
    memory::Root r(roots, young);

    om.collect_full(state);
    gc_data = NULL;

    TS_ASSERT(young->marked_p(mark));
  }

  /* Could segfault on failure due to infinite loop. */
  void test_collect_full_stops_at_already_marked_objects() {
    Memory& om = *state->memory();
    Tuple *young, *mature;

    om.large_object_threshold = 50;

    young =  as<Tuple>(util_new_object(om));
    mature = as<Tuple>(util_new_object(om, LARGE_OBJECT_BYTE_SIZE));

    young->field[0] = mature;
    mature->field[0] = young;

    om.write_barrier(young, mature);
    om.write_barrier(mature, young);

    memory::Root r(roots, young);

    om.collect_full(state);
    gc_data = NULL;

    mature = as<Tuple>(young->field[0]);

    TS_ASSERT_EQUALS(mature->field[0], young);
  }

  /* TODO: young gen
  void xtest_collect_young_stops_at_already_marked_objects() {
    Memory& om = *state->memory();
    Tuple *obj, *obj2;

    obj =  as<Tuple>(util_new_object(om));
    obj2 = as<Tuple>(util_new_object(om));

    obj2->field[1] = cTrue;
    obj->field[0] = obj2;
    obj2->field[0] = obj;

    om.write_barrier(obj, obj2);
    om.write_barrier(obj2, obj);

    memory::Root r(roots, obj);

    om.collect_young(state, gc_data);

    obj = as<Tuple>(roots->front()->get());
    obj2 = as<Tuple>(obj->field[0]);

    TS_ASSERT_EQUALS(obj2->field[0], obj);
    TS_ASSERT_EQUALS(obj2->field[1], cTrue);
  }
  */

  void test_valid_object_p() {
    Memory& om = *state->memory();
    Object* obj;

    obj = util_new_object(om);
    TS_ASSERT(om.valid_object_p(obj));
  }
};


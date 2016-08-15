#include "machine/test/test.hpp"

#include "memory/header.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestMemoryHeader : public CxxTest::TestSuite, public VMTest {
public:

  MemoryHeader h;

  void setUp() {
    MemoryHeader::initialize(&h, 0, MemoryHeader::eFirstRegion, InvalidType);
  }

  void test_memory_flag_is_64bit_machine_word() {
    TS_ASSERT_EQUALS(sizeof(MemoryFlags), sizeof(uintptr_t));
    TS_ASSERT_EQUALS(sizeof(MemoryFlags), 8);
  }

  void test_memory_flag_is_lock_free() {
    TS_ASSERT_EQUALS(h.header.is_lock_free(), true);
  }

  void test_memory_flag_forwarded() {
    TS_ASSERT_EQUALS(RBX_MEMORY_FORWARDED_P(h.header.load()), false);
    TS_ASSERT_EQUALS(h.forwarded_p(), false);

    h.header.store(RBX_MEMORY_SET_FORWARDED(h.header.load()));
    TS_ASSERT_EQUALS(RBX_MEMORY_FORWARDED_P(h.header.load()), true);
    TS_ASSERT_EQUALS(h.forwarded_p(), true);
    TS_ASSERT_EQUALS(h.header.load(), 0x1L);
  }

  void test_memory_flag_inflated() {
    TS_ASSERT_EQUALS(RBX_MEMORY_INFLATED_P(h.header.load()), false);
    TS_ASSERT_EQUALS(h.inflated_p(), false);

    h.header.store(RBX_MEMORY_SET_INFLATED(h.header.load()));
    TS_ASSERT_EQUALS(RBX_MEMORY_INFLATED_P(h.header.load()), true);
    TS_ASSERT_EQUALS(h.inflated_p(), true);
    TS_ASSERT_EQUALS(h.header.load(), 0x2L);
  }

  void test_memory_flag_thread_id() {
    TS_ASSERT_EQUALS(RBX_MEMORY_GET_THREAD_ID(h.header.load()), 0);

    h.header.store(RBX_MEMORY_SET_THREAD_ID(h.header.load(), 0xffff));

    TS_ASSERT_EQUALS(h.header.load(), 0x3ffcL);
    TS_ASSERT_EQUALS(h.thread_id(), 0xfff);
    TS_ASSERT_EQUALS(RBX_MEMORY_GET_THREAD_ID(h.header.load()), 0xfff);
  }

  void test_memory_flag_region() {
    TS_ASSERT_EQUALS(RBX_MEMORY_GET_REGION(h.header.load()), 0);

    h.header.store(RBX_MEMORY_SET_REGION(h.header.load(), 0x7));

    TS_ASSERT_EQUALS(h.header.load(), 0xc000L);
    TS_ASSERT_EQUALS(h.region(), 0x3);
    TS_ASSERT_EQUALS(RBX_MEMORY_GET_REGION(h.header.load()), 0x3);
  }

  void test_memory_flag_marked() {
    TS_ASSERT_EQUALS(h.marked_p(1), false);
    TS_ASSERT_EQUALS(h.marked_p(2), false);
    TS_ASSERT_EQUALS(h.marked_p(3), false);

    h.mark(2);
    TS_ASSERT_EQUALS(h.marked_p(1), false);
    TS_ASSERT_EQUALS(h.marked_p(2), true);
    TS_ASSERT_EQUALS(h.marked_p(3), false);

    h.mark(3);
    TS_ASSERT_EQUALS(h.header.load(), 0x30000L);
  }

  void test_memory_flag_scanned() {
    TS_ASSERT_EQUALS(RBX_MEMORY_SCANNED_P(h.header.load()), false);
    TS_ASSERT_EQUALS(h.scanned_p(), false);

    h.header.store(RBX_MEMORY_SET_SCANNED(h.header.load()));
    TS_ASSERT_EQUALS(RBX_MEMORY_SCANNED_P(h.header.load()), true);
    TS_ASSERT_EQUALS(h.scanned_p(), true);
    TS_ASSERT_EQUALS(h.header.load(), 0x40000L);
  }

  void test_memory_flag_referenced() {
    TS_ASSERT_EQUALS(RBX_MEMORY_GET_REFERENCED(h.header.load()), 0);

    h.referenced(static_cast<object_type>((1L << 4) - 1));

    TS_ASSERT_EQUALS(h.header.load(), 0x780000L);
    TS_ASSERT_EQUALS(h.referenced(), 0xf);
    TS_ASSERT_EQUALS(RBX_MEMORY_GET_REFERENCED(h.header.load()), 0xf);
  }

  void test_memory_flag_type_id() {
    TS_ASSERT_EQUALS(RBX_MEMORY_GET_TYPE_ID(h.header.load()), 0);

    h.type_id(static_cast<object_type>((1L << 16) - 1));

    TS_ASSERT_EQUALS(h.header.load(), 0x3fff800000L);
    TS_ASSERT_EQUALS(h.type_id(), 0x7fff);
    TS_ASSERT_EQUALS(RBX_MEMORY_GET_TYPE_ID(h.header.load()), 0x7fff);
  }

  void test_memory_flag_data() {
    TS_ASSERT_EQUALS(RBX_MEMORY_DATA_P(h.header.load()), false);
    TS_ASSERT_EQUALS(h.data_p(), false);

    h.header.store(RBX_MEMORY_SET_DATA(h.header.load()));
    TS_ASSERT_EQUALS(RBX_MEMORY_DATA_P(h.header.load()), true);
    TS_ASSERT_EQUALS(h.data_p(), true);
    TS_ASSERT_EQUALS(h.header.load(), 0x4000000000L);

    h.header.store(RBX_MEMORY_UNSET_DATA(h.header.load()));
    TS_ASSERT_EQUALS(h.header.load(), 0);
    TS_ASSERT_EQUALS(h.data_p(), false);
  }

  void test_memory_flag_frozen() {
    TS_ASSERT_EQUALS(RBX_MEMORY_FROZEN_P(h.header.load()), false);
    TS_ASSERT_EQUALS(h.frozen_p(), false);

    h.header.store(RBX_MEMORY_SET_FROZEN(h.header.load()));
    TS_ASSERT_EQUALS(RBX_MEMORY_FROZEN_P(h.header.load()), true);
    TS_ASSERT_EQUALS(h.frozen_p(), true);
    TS_ASSERT_EQUALS(h.header.load(), 0x8000000000L);

    h.header.store(RBX_MEMORY_UNSET_FROZEN(h.header.load()));
    TS_ASSERT_EQUALS(h.header.load(), 0);
    TS_ASSERT_EQUALS(h.frozen_p(), false);
  }

  void test_memory_flag_tainted() {
    TS_ASSERT_EQUALS(RBX_MEMORY_TAINTED_P(h.header.load()), 0);
    TS_ASSERT_EQUALS(h.tainted_p(), false);

    h.header.store(RBX_MEMORY_SET_TAINTED(h.header.load()));
    TS_ASSERT_EQUALS(h.tainted_p(), true);
    TS_ASSERT_EQUALS(h.header.load(), 0x10000000000L);

    h.header.store(RBX_MEMORY_UNSET_TAINTED(h.header.load()));
    TS_ASSERT_EQUALS(h.header.load(), 0);
    TS_ASSERT_EQUALS(h.tainted_p(), false);
  }

  void test_memory_flag_bias_locked() {
    TS_ASSERT_EQUALS(RBX_MEMORY_BIAS_LOCKED_P(h.header.load()), 0);

    h.header.store(RBX_MEMORY_SET_BIAS_LOCKED(h.header.load()));

    TS_ASSERT_EQUALS(h.header.load(), 0x20000000000L);
  }

  void test_memory_flag_locked_count() {
    TS_ASSERT_EQUALS(RBX_MEMORY_GET_LOCKED_COUNT(h.header.load()), 0);

    h.header.store(RBX_MEMORY_SET_LOCKED_COUNT(h.header.load(), 0xffff));

    TS_ASSERT_EQUALS(h.header.load(), 0x3c0000000000L);
  }

  void test_memory_flag_lock_inflated() {
    TS_ASSERT_EQUALS(h.lock_inflated_p(), false);

    h.header.store(RBX_MEMORY_SET_LOCK_INFLATED(h.header.load()));

    TS_ASSERT_EQUALS(h.header.load(), 0x400000000000L);
  }

  void test_memory_flag_object_id() {
    TS_ASSERT_EQUALS(h.object_id(), 0);

    h.header.store(RBX_MEMORY_SET_OBJECT_ID(h.header.load(), 0xffff));
    TS_ASSERT_EQUALS(h.object_id(), 0xfff);

    TS_ASSERT_EQUALS(h.header.load(), 0x7ff800000000000L);
  }

  void test_memory_flag_reserved() {
    TS_ASSERT_EQUALS(h.reserved(), 0);

    h.reserved(0xf);
    TS_ASSERT_EQUALS(h.reserved(), 0x7);

    TS_ASSERT_EQUALS(h.header.load(), 0x3800000000000000L);
  }

  void test_memory_flag_visited() {
    TS_ASSERT_EQUALS(h.visited_p(1), false);
    TS_ASSERT_EQUALS(h.visited_p(2), false);
    TS_ASSERT_EQUALS(h.visited_p(3), false);

    h.visited(2);
    TS_ASSERT_EQUALS(h.visited_p(1), false);
    TS_ASSERT_EQUALS(h.visited_p(2), true);
    TS_ASSERT_EQUALS(h.visited_p(3), false);

    h.visited(3);
    TS_ASSERT_EQUALS(h.header.load(), 0xc000000000000000L);
  }
};

#include "machine/test/test.hpp"

#include "memory/header.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestMemoryHeader : public CxxTest::TestSuite, public VMTest {
public:

  MemoryHeader h;

  void setUp() {
    MemoryHeader::initialize(&h, 0, eThreadRegion, InvalidType, false);
  }

  void test_memory_header_is_64bit_machine_word() {
    TS_ASSERT_EQUALS(sizeof(MemoryFlags), sizeof(uintptr_t));
    TS_ASSERT_EQUALS(sizeof(MemoryFlags), 8);
  }

  void test_memory_header_is_lock_free() {
    TS_ASSERT_EQUALS(h.header.is_lock_free(), true);
  }

  void test_memory_header_extended() {
    TS_ASSERT_EQUALS(h.extended_p(), false);

    h.header = 0x1L;
    TS_ASSERT_EQUALS(h.extended_p(), true);
  }

  void test_memory_header_forwarded() {
    TS_ASSERT_EQUALS(h.forwarded_p(), false);

    h.header = 0x2L;
    TS_ASSERT_EQUALS(h.forwarded_p(), true);
  }

  void test_memory_header_thread_id() {
    h.header = 0x3ffcL;

    TS_ASSERT_EQUALS(h.thread_id(), 0xfff);
    TS_ASSERT_EQUALS(MemoryHeader::max_thread_id(), 0xfff);
  }

  void test_memory_header_region() {
    h.header = 0xc000L;

    TS_ASSERT_EQUALS(static_cast<int>(h.region()), 3);
  }

  void test_memory_header_pinned() {
    TS_ASSERT_EQUALS(h.pinned_p(), false);

    h.header = 0x10000L;
    TS_ASSERT_EQUALS(h.pinned_p(), true);

    h.unset_pinned();
    TS_ASSERT_EQUALS(h.pinned_p(), false);

    h.set_pinned();
    TS_ASSERT_EQUALS(h.pinned_p(), true);
  }

  void test_memory_header_visited() {
    TS_ASSERT_EQUALS(h.visited_p(0), true);
    TS_ASSERT_EQUALS(h.visited_p(1), false);
    TS_ASSERT_EQUALS(h.visited_p(2), false);
    TS_ASSERT_EQUALS(h.visited_p(3), false);

    h.set_visited(2);
    TS_ASSERT_EQUALS(h.visited_p(0), false);
    TS_ASSERT_EQUALS(h.visited_p(1), false);
    TS_ASSERT_EQUALS(h.visited_p(2), true);
    TS_ASSERT_EQUALS(h.visited_p(3), false);

    h.set_visited(3);
    TS_ASSERT_EQUALS(h.header.load(), 0x60000L);
  }

  void test_memory_header_marked() {
    TS_ASSERT_EQUALS(h.marked_p(0), true);
    TS_ASSERT_EQUALS(h.marked_p(1), false);
    TS_ASSERT_EQUALS(h.marked_p(2), false);
    TS_ASSERT_EQUALS(h.marked_p(3), false);

    h.set_marked(2);
    TS_ASSERT_EQUALS(h.marked_p(0), false);
    TS_ASSERT_EQUALS(h.marked_p(1), false);
    TS_ASSERT_EQUALS(h.marked_p(2), true);
    TS_ASSERT_EQUALS(h.marked_p(3), false);

    h.set_marked(3);
    TS_ASSERT_EQUALS(h.header.load(), 0x180000L);
  }

  void test_memory_header_scanned() {
    TS_ASSERT_EQUALS(h.scanned_p(), false);

    h.header = 0x200000L;
    TS_ASSERT_EQUALS(h.scanned_p(), true);

    h.unset_scanned();
    TS_ASSERT_EQUALS(h.scanned_p(), false);

    h.set_scanned();
    TS_ASSERT_EQUALS(h.scanned_p(), true);
  }

  void test_memory_header_referenced() {
    TS_ASSERT_EQUALS(h.referenced(), 0);

    h.header = 0x3c00000L;

    TS_ASSERT_EQUALS(h.referenced(), 15);
  }

  void test_memory_header_add_reference() {
    for(int i = 0; i < 20; i++) {
      TS_ASSERT_EQUALS(h.referenced(), i);

      h.add_reference();

      TS_ASSERT_EQUALS(h.referenced(), i+1);

      if(i > 15) {
        TS_ASSERT(h.extended_p());
      }
    }
  }

  void test_memory_header_type_id() {
    TS_ASSERT_EQUALS(h.type_id(), 0);

    h.header = 0xffc000000;
    TS_ASSERT_EQUALS(h.type_id(), 0x3ffL);
  }

  void test_memory_header_data() {
    TS_ASSERT_EQUALS(h.data_p(), false);
    TS_ASSERT_EQUALS(h.object_p(), true);

    h.header = 0x1000000000L;
    TS_ASSERT_EQUALS(h.data_p(), true);
    TS_ASSERT_EQUALS(h.object_p(), false);
  }

  void test_memory_header_frozen() {
    TS_ASSERT_EQUALS(h.frozen_p(), false);

    h.header = 0x2000000000L;
    TS_ASSERT_EQUALS(h.frozen_p(), true);

    h.unset_frozen();
    TS_ASSERT_EQUALS(h.frozen_p(), false);

    h.set_frozen();
    TS_ASSERT_EQUALS(h.frozen_p(), true);
  }

  void test_memory_header_tainted() {
    TS_ASSERT_EQUALS(h.tainted_p(), false);

    h.header = 0x4000000000L;
    TS_ASSERT_EQUALS(h.tainted_p(), true);

    h.unset_tainted();
    TS_ASSERT_EQUALS(h.tainted_p(), false);

    h.set_tainted();
    TS_ASSERT_EQUALS(h.tainted_p(), true);
  }

  void test_memory_header_object_id() {
    TS_ASSERT_EQUALS(h.object_id(), 0);

    h.header = 0x3ffffc0000000000L;
    TS_ASSERT_EQUALS(h.object_id(), 0xfffffL);

    TS_ASSERT_EQUALS(MemoryHeader::max_object_id(), 0xfffffL);
  }

  void test_memory_header_set_object_id() {
    MemoryHeader::object_id_counter = 21;

    h.get_object_id();

    TS_ASSERT_EQUALS(h.object_id(), 0x15);

    MemoryHeader::object_id_counter = MemoryHeader::max_object_id() + 10;

    h.header = 0;
    h.get_object_id();

    TS_ASSERT(h.extended_p());
    TS_ASSERT_EQUALS(h.object_id(), MemoryHeader::max_object_id() + 10);
  }

  void test_memory_header_type_specific() {
    TS_ASSERT_EQUALS(h.type_specific(), 0);

    h.header = 0xc000000000000000L;
    TS_ASSERT_EQUALS(h.type_specific(), 0x3L);
  }
};

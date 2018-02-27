#include "machine/test/test.hpp"

#include "memory/header.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestMemoryHeader : public CxxTest::TestSuite, public VMTest {
public:

  MemoryHeader h;

  void setUp() {
    VMTest::setUp();
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
    TS_ASSERT_EQUALS(h.extended_header_p(), false);

    h.header = 0x1L;
    TS_ASSERT_EQUALS(h.extended_header_p(), true);
  }

  void test_memory_header_forwarded() {
    TS_ASSERT_EQUALS(h.forwarded_p(), false);

    h.header = 0x2L;
    TS_ASSERT_EQUALS(h.forwarded_p(), true);
  }

  void test_memory_header_thread_id() {
    h.header = 0x1ffcL;

    TS_ASSERT_EQUALS(h.thread_id(), 0x7ff);
    TS_ASSERT_EQUALS(MemoryHeader::max_thread_id(), 0x7ff);
  }

  void test_memory_header_region() {
    h.header = 0x6000L;

    TS_ASSERT_EQUALS(static_cast<int>(h.region()), 3);
  }

  void test_memory_header_pinned() {
    TS_ASSERT_EQUALS(h.pinned_p(), false);

    h.header = 0x80000L;
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
    TS_ASSERT_EQUALS(h.header.load(), 0x1800000L);
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
    TS_ASSERT_EQUALS(h.header.load(), 0x6000000L);
  }

  void test_memory_header_scanned() {
    TS_ASSERT_EQUALS(h.scanned_p(), false);

    h.header = 0x8000000L;
    TS_ASSERT_EQUALS(h.scanned_p(), true);

    h.unset_scanned();
    TS_ASSERT_EQUALS(h.scanned_p(), false);

    h.set_scanned();
    TS_ASSERT_EQUALS(h.scanned_p(), true);
  }

  void test_memory_header_referenced() {
    TS_ASSERT_EQUALS(h.referenced(), 0);

    h.header = 0x78000L;

    TS_ASSERT_EQUALS(h.referenced(), 15);
  }

  void test_memory_header_add_reference() {
    for(int i = 0; i < 20; i++) {
      TS_ASSERT_EQUALS(h.referenced(), i);

      h.add_reference(state);

      TS_ASSERT_EQUALS(h.referenced(), i+1);

      if(i > 15) {
        TS_ASSERT(h.extended_header_p());
      }
    }
  }

  void test_memory_header_type_id() {
    TS_ASSERT_EQUALS(h.type_id(), 0);

    h.header = 0x1ff0000000L;
    TS_ASSERT_EQUALS(h.type_id(), 0x1ffL);
  }

  void test_memory_header_data() {
    TS_ASSERT_EQUALS(h.data_p(), false);
    TS_ASSERT_EQUALS(h.object_p(), true);

    h.header = 0x2000000000L;
    TS_ASSERT_EQUALS(h.data_p(), true);
    TS_ASSERT_EQUALS(h.object_p(), false);
  }

  void test_memory_header_frozen() {
    TS_ASSERT_EQUALS(h.frozen_p(), false);

    h.header = 0x4000000000L;
    TS_ASSERT_EQUALS(h.frozen_p(), true);

    h.unset_frozen();
    TS_ASSERT_EQUALS(h.frozen_p(), false);

    h.set_frozen();
    TS_ASSERT_EQUALS(h.frozen_p(), true);
  }

  void test_memory_header_tainted() {
    TS_ASSERT_EQUALS(h.tainted_p(), false);

    h.header = 0x8000000000L;
    TS_ASSERT_EQUALS(h.tainted_p(), true);

    h.unset_tainted();
    TS_ASSERT_EQUALS(h.tainted_p(), false);

    h.set_tainted();
    TS_ASSERT_EQUALS(h.tainted_p(), true);
  }

  void test_memory_header_object_id() {
    TS_ASSERT_EQUALS(h.object_id(), 0);

    h.header = 0x3ffff80000000000L;
    TS_ASSERT_EQUALS(h.object_id(), 0x7ffffL);

    TS_ASSERT_EQUALS(MemoryHeader::max_object_id(), 0x7ffffL);
  }

  void test_memory_header_set_object_id() {
    MemoryHeader::object_id_counter = 21;

    h.assign_object_id();

    TS_ASSERT_EQUALS(h.object_id(), 0x15);

    MemoryHeader::object_id_counter = MemoryHeader::max_object_id() + 10;

    h.header = 0;
    h.assign_object_id();

    TS_ASSERT(h.extended_header_p());
    TS_ASSERT_EQUALS(h.object_id(), MemoryHeader::max_object_id() + 10);
  }

  void test_memory_header_type_specific() {
    TS_ASSERT_EQUALS(h.type_specific(), 0);

    h.header = 0xc000000000000000L;
    TS_ASSERT_EQUALS(h.type_specific(), 0x3L);
  }

  void test_memory_header_memory_handle() {
    String* str = String::create(state, "");
    MemoryHandle* handle = h.memory_handle(state, str);

    TS_ASSERT(handle);
    TS_ASSERT(h.extended_header_p());
    TS_ASSERT_EQUALS(h.memory_handle(state, str), handle);
  }

  void test_memory_header_lock() {
    h.set_thread_id(state->vm()->thread_id());

    TS_ASSERT_THROWS_ASSERT(h.unlock(state),
			    const RubyException &e,
			    TS_ASSERT(Exception::runtime_error_p(state, e.exception)));

    for(int i = 0; i < rubinius::MemoryHeader::max_locked_count(); i++) {
      h.lock(state);
      TS_ASSERT_EQUALS(locked_count_field.get(h.header), i+1);
    }

    TS_ASSERT(!h.extended_header_p());

    for(int i = rubinius::MemoryHeader::max_locked_count(); i > 0; i--) {
      TS_ASSERT_EQUALS(locked_count_field.get(h.header), i);
      h.unlock(state);
    }

    TS_ASSERT(!h.extended_header_p());

    TS_ASSERT_THROWS_ASSERT(h.unlock(state),
			    const RubyException &e,
			    TS_ASSERT(Exception::runtime_error_p(state, e.exception)));

    for(int i = 0; i < rubinius::MemoryHeader::max_locked_count() + 2; i++) {
      h.lock(state);
    }

    TS_ASSERT(h.extended_header_p());

    for(int i = 0; i < rubinius::MemoryHeader::max_locked_count() + 1; i++) {
      h.unlock(state);
    }
  }
};

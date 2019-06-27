#include "machine/test/test.hpp"

#include "memory/immix.hpp"

#include <cxxtest/TestSuite.h>

class TestImmix : public CxxTest::TestSuite, public VMTest {
public:

  memory::Immix* immix;

  void setUp() {
    VMTest::setUp();
    immix = new memory::Immix(state);
  }

  void tearDown() {
    VMTest::tearDown();
    delete immix;
  }

  void test_get_block() {
    memory::Block& block = immix->get_block();
    TS_ASSERT_EQUALS(block.size(), memory::cBlockSize);
    TS_ASSERT(block.address() != 0);
    TS_ASSERT_EQUALS(block.status(), memory::cFree);
    TS_ASSERT_EQUALS(block.lines_used(), 1U);
  }

  void test_Block_is_line_free() {
    memory::Block& block = immix->get_block();
    TS_ASSERT(block.is_line_free(1));
    block.mark_line(1);
    TS_ASSERT(block.is_line_free(1));
    block.copy_marks();
    TS_ASSERT(!block.is_line_free(1));
  }

  void test_Block_address_of_line() {
    memory::Block& block = immix->get_block();
    uintptr_t top = block.address();
    TS_ASSERT_EQUALS(block.address_of_line(0U), top);
    TS_ASSERT_EQUALS(block.address_of_line(1U), top + memory::cLineSize);
  }

  void test_SingleBlockAllocator_allocate() {
    memory::Block& block = immix->get_block();
    memory::SingleBlockAllocator alloc(block);
    Object* addr = alloc.allocate(state, 24);
    uintptr_t top  = block.first_address();
    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(addr), top);

    Object* another = alloc.allocate(state, 24);
    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(another), top + 24);
  }

  void test_SingleBlockAllocator_allocate_checks_mark_on_spill() {
    memory::Block& block = immix->get_block();
    uintptr_t top  = block.first_address();

    block.mark_line(2);
    block.copy_marks();
    memory::SingleBlockAllocator alloc(block);
    alloc.allocate(state, 96);

    Object* addr = alloc.allocate(state, 64);
    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(addr), top + (memory::cLineSize * 2));
  }

  void test_SingleBlockAllocator_allocate_spans_next_line() {
    memory::Block& block = immix->get_block();
    uintptr_t top = block.first_address();
    memory::SingleBlockAllocator alloc(block);

    int size = memory::cLineSize - sizeof(memory::BlockHeader) - 4;
    MemoryHeader* s_obj = alloc.allocate(state, size);

    TS_ASSERT_EQUALS(top, reinterpret_cast<uintptr_t>(s_obj));
    TS_ASSERT(sizeof(Object) > 4);

    Object* addr = alloc.allocate(state, sizeof(Object));

    TS_ASSERT(size < MemoryHeader::align(size));
    TS_ASSERT_EQUALS((reinterpret_cast<uintptr_t>(addr) - top), MemoryHeader::align(size));

    Object* addr2 = alloc.allocate(state, memory::cLineSize + 4);
    Object* addr3 = alloc.allocate(state, 4);

    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(addr2) - reinterpret_cast<uintptr_t>(addr),
        MemoryHeader::align(sizeof(Object)));
    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(addr3) - reinterpret_cast<uintptr_t>(addr2),
        MemoryHeader::align(memory::cLineSize + 4));
  }

  void test_SingleBlockAllocator_allocate_spans_lines() {
    memory::Block& block = immix->get_block();
    uintptr_t top = block.first_address();

    memory::SingleBlockAllocator alloc(block);
    alloc.allocate(state, 24);
    int size = (memory::cLineSize * 2) + 32;
    Object* big = alloc.allocate(state, size);
    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(big), top + 24);

    Object* addr2 = alloc.allocate(state, 24);
    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(addr2) - reinterpret_cast<uintptr_t>(big),
        MemoryHeader::align(size));
  }

  void test_SingleBlockAllocator_allocate_skips_marked_lines() {
    memory::Block& block = immix->get_block();
    uintptr_t top  = block.address();
    block.mark_line(0);
    block.mark_line(2);
    block.mark_line(4);
    block.mark_line(5);
    block.mark_line(7);
    block.copy_marks();

    memory::SingleBlockAllocator alloc(block);
    Object* addr = alloc.allocate(state, 24);
    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(addr), top + memory::cLineSize);

    Object* addr2 = alloc.allocate(state, 24);
    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(addr2) - reinterpret_cast<uintptr_t>(addr),
        MemoryHeader::align(24));

    Object* addr3 = alloc.allocate(state, 128);
    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(addr3) - top,
        MemoryHeader::align(memory::cLineSize * 3));

    Object* addr4 = alloc.allocate(state, 156);
    TS_ASSERT_EQUALS(reinterpret_cast<uintptr_t>(addr4) - top,
        MemoryHeader::align(memory::cLineSize * 8));
  }

  void test_SingleBlockAllocator_allocate_indicates_failure() {
    memory::Block& block = immix->get_block();

    for(uint32_t i = 0; i < memory::cLineTableSize; i++) {
      block.mark_line(i);
    }

    block.free_line(1);
    block.copy_marks();
    memory::SingleBlockAllocator alloc(block);
    Object* small = alloc.allocate(state, 24);
    TS_ASSERT(small);

    Object* addr = alloc.allocate(state, 156);
    TS_ASSERT(!addr);
  }

  void test_sweep_blocks_frees_empty_blocks() {
    immix->sweep_blocks();
    memory::Block& block = immix->get_block();

    immix->sweep_blocks();
    memory::Block& block2 = immix->get_block();
    TS_ASSERT_EQUALS(&block, &block2);
  }

  void test_sweep_blocks_sorts_blocks() {
    memory::Block& block  = immix->get_block();
    memory::Block& block2 = immix->get_block();
    memory::Block& block3 = immix->get_block();

    block.set_status(memory::cUnavailable);
    block2.set_status(memory::cRecyclable);
    immix->evacuate_block(block3);
    TS_ASSERT_EQUALS(block3.status(), memory::cEvacuate);

    immix->sweep_blocks();

    TS_ASSERT_EQUALS(block3.status(), memory::cFree);
  }

  void test_Block_update_stats() {
    memory::Block& block = immix->get_block();

    block.mark_line(0);
    block.mark_line(1);
    block.mark_line(3);
    block.mark_line(5);
    block.mark_line(7);
    block.mark_line(10);

    block.update_stats();
    TS_ASSERT_EQUALS(block.status(), memory::cRecyclable);
    TS_ASSERT_EQUALS(block.holes(), 5U);
    TS_ASSERT_EQUALS(block.lines_used(), 6U);
  }

  void test_Block_update_stats_finds_empty_blocks() {
    memory::Block& block = immix->get_block();

    block.set_status(memory::cRecyclable);
    block.update_stats();
    TS_ASSERT_EQUALS(block.status(), memory::cFree);
    TS_ASSERT_EQUALS(block.holes(), 1U);
    TS_ASSERT_EQUALS(block.lines_used(), 1U);
  }

  void test_Block_update_stats_finds_unavailable_blocks() {
    memory::Block& block = immix->get_block();

    for(uint32_t i = 0; i < memory::cLineTableSize; i++) {
      block.mark_line(i);
    }

    block.update_stats();
    TS_ASSERT_EQUALS(block.status(), memory::cUnavailable);
    TS_ASSERT_EQUALS(block.holes(), 0U);
    TS_ASSERT_EQUALS(block.lines_used(), memory::cLineTableSize);
  }

  void test_get_block_returns_recyclable_blocks() {
    immix->sweep_blocks();

    memory::Block& block  = immix->get_block();
    block.set_status(memory::cRecyclable);

    immix->sweep_blocks();

    memory::Block& block2 = immix->get_block();

    TS_ASSERT_EQUALS(&block2, &block);
  }

  void test_mark_address_updates_block() {
    memory::Block& block = immix->get_block();
    memory::SingleBlockAllocator alloc(block);
    Object* addr = alloc.allocate(state, 24);

    TS_ASSERT(block.is_line_free(1));
    immix->mark_address_of_object(state, 0, addr, alloc);
    block.copy_marks();
    TS_ASSERT(!block.is_line_free(1));
  }

  void test_mark_address_ignores_already_marked_objects() {
    /* TODO: GC
    memory::Block& block = immix->get_block();
    memory::SingleBlockAllocator alloc(block);
    Object* addr = alloc.allocate(state, 24);

    addr->set_marked(state->memory()->mark());

    TS_ASSERT(block.is_line_free(1));
    immix->mark_address_of_object(state, 0, addr, alloc);
    block.copy_marks();
    TS_ASSERT(block.is_line_free(1));
    */
  }

  void test_mark_address_returns_forwarding_pointer() {
    /* TODO: GC
    memory::Block& block = immix->get_block();
    memory::SingleBlockAllocator alloc(block);
    Object* addr = alloc.allocate(state, 24);

    memory::Block& block2 = immix->get_block();
    memory::SingleBlockAllocator alloc2(block2);
    Object* addr2 = alloc2.allocate(state, 24);

    immix->describer().set_forwarding_pointer(addr, addr2);

    Object* out = immix->mark_address_of_object(state, 0, addr, alloc);

    TS_ASSERT_EQUALS(addr->fwd, addr2);
    TS_ASSERT_EQUALS(out, addr2);
    */
  }

  void test_mark_address_can_move_objects() {
    /* TODO: GC
    memory::Block& block = immix->get_block();
    memory::SingleBlockAllocator alloc(block);
    Object* addr = alloc.allocate(state, sizeof(Object));

    memory::Block& dest = immix->get_block();
    memory::SingleBlockAllocator dest_alloc(dest);

    block.set_status(memory::cEvacuate);

    Object* redirect = immix->mark_address_of_object(state, 0, addr, dest_alloc);

    Object* fwd = immix->describer().forwarding_pointer(addr);
    TS_ASSERT_EQUALS(fwd, dest.first_address());
    TS_ASSERT_EQUALS(fwd, redirect);

    TS_ASSERT_EQUALS((unsigned)fwd.as<Object>()->magic, 0xdecafbad);
    */
  }

  void test_mark_address_marks_all_lines_for_object() {
    /* TOD: GC
    memory::Block& block = immix->get_block();
    memory::SingleBlockAllocator alloc(block);
    int size = memory::cLineSize - sizeof(memory::BlockHeader) - 4;
    alloc.allocate(state, size);
    TS_ASSERT(sizeof(Object) > 4);
    Object* addr = alloc.allocate(state, sizeof(Object));

    immix->mark_address_of_object(state, 0, addr, alloc);
    block.copy_marks();
    TS_ASSERT(!block.is_line_free(0));
    TS_ASSERT(!block.is_line_free(1));

    int big_size = memory::cLineSize * 3;
    Object* addr2 = alloc.allocate(state, big_size);

    immix->mark_address_of_object(state, 0, addr2, alloc);
    block.copy_marks();
    TS_ASSERT(!block.is_line_free(1));
    TS_ASSERT(!block.is_line_free(2));
    TS_ASSERT(!block.is_line_free(3));
    TS_ASSERT(!block.is_line_free(4));
    */
  }

  void test_BlockAllocator_reset_updates_block_stats() {
    memory::Block& block = immix->get_block();

    block.mark_line(1);
    block.mark_line(3);

    TS_ASSERT_EQUALS(block.lines_used(), 1U);
    memory::BlockAllocator& ba = immix->block_allocator();
    ba.reset();

    TS_ASSERT_EQUALS(block.lines_used(), 3U);
  }

  void test_BlockAllocator_get_free_block() {
    memory::BlockAllocator& ba = immix->block_allocator();
    ba.add_chunk();
    ba.current_chunk().get_block(0).set_status(memory::cRecyclable);
    memory::Block& block = ba.get_free_block();
    TS_ASSERT_EQUALS(block.status(), memory::cFree);
  }

  void test_BlockAllocator_uses_all_available_chunks_first() {
    /* TODO: GC
    memory::BlockAllocator& ba = immix->block_allocator();
    memory::Chunk *c1 = ba.add_chunk();
    memory::Chunk* c2 = ba.add_chunk();
    ba.reset();

    TS_ASSERT_EQUALS(&ba.current_chunk(), c1);

    for(uint32_t i = 0; i < memory::cBlocksPerChunk + 1; i++) {
      ba.get_block();
    }

    TS_ASSERT_EQUALS(&ba.current_chunk(), c2);
    */
  }

  void test_BlockAllocator_skips_unavailable_blocks() {
    /* TODO: GC
    memory::BlockAllocator& ba = immix->block_allocator();
    memory::Chunk* chunk = ba.add_chunk();
    ba.reset();
    chunk->get_block(0).set_status(memory::cUnavailable);

    memory::Block& block = ba.get_block();

    TS_ASSERT_EQUALS(&block, &chunk->get_block(1));
    */
  }

  void test_BlockAllocator_skips_evacuating_blocks() {
    /* TODO: GC
    memory::BlockAllocator& ba = immix->block_allocator();
    memory::Chunk* chunk = ba.add_chunk();
    ba.reset();
    chunk->get_block(0).set_status(memory::cEvacuate);

    memory::Block& block = ba.get_block();

    TS_ASSERT_EQUALS(&block, &chunk->get_block(1));
    */
  }

  void test_HoleFinder_find_hole_on_empty_block() {
    memory::Block& block = immix->get_block();
    memory::HoleFinder alloc;
    alloc.reset(&block);

    alloc.find_hole();

    TS_ASSERT_EQUALS(alloc.cursor(), block.first_address());
    TS_ASSERT_EQUALS(alloc.limit(),  block.address() + memory::cBlockSize);
    TS_ASSERT_EQUALS(alloc.hole_start_line(), memory::cLineTableSize);
  }

  void test_HoleFinder_find_hole_with_hole_in_middle() {
    memory::Block& block = immix->get_block();
    block.mark_line(11);
    block.copy_marks();

    memory::HoleFinder alloc;

    // .reset calls find_hole
    alloc.reset(&block);

    TS_ASSERT_EQUALS(alloc.cursor(), block.first_address());
    TS_ASSERT_EQUALS(alloc.limit(),  block.address_of_line(11U));
    TS_ASSERT_EQUALS(alloc.hole_start_line(), 11U);

    alloc.find_hole();
    TS_ASSERT_EQUALS(alloc.cursor(), block.address_of_line(12U));
    TS_ASSERT_EQUALS(alloc.limit(),  block.address() + memory::cBlockSize);
    TS_ASSERT_EQUALS(alloc.hole_start_line(), memory::cLineTableSize);
  }
};

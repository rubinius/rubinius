
#include <cxxtest/TestSuite.h>
#include "memory/address.hpp"
#include "memory/immix_collector.hpp"


struct SimpleObject {
  bool marked, body_checked;
  memory::Address fwd;
  int magic, size;
  SimpleObject* sub;
};

class SimpleObjectDescriber {
public:
  typedef SimpleObject Object;

  void set_forwarding_pointer(memory::Address from, memory::Address to) {
    from.as<SimpleObject>()->fwd = to;
  }

  memory::Address forwarding_pointer(memory::Address cur) {
    SimpleObject* obj = cur.as<SimpleObject>();
    if(obj->fwd.is_null()) return 0;
    return obj->fwd;
  }

  bool pinned(memory::Address addr) {
    return false;
  }

  memory::Address copy(memory::Address original, memory::ImmixAllocator& alloc) {
    bool collect;

    memory::Address copy_addr = alloc.allocate(sizeof(SimpleObject), collect);
    SimpleObject* copy = copy_addr.as<SimpleObject>();
    SimpleObject* orig = original.as<SimpleObject>();

    copy->fwd = 0;
    copy->magic = orig->magic;
    copy->sub = orig->sub;

    return copy;
  }

  bool mark_address(memory::Address addr, memory::MarkStack& ms, bool push = true) {
    SimpleObject* obj = addr.as<SimpleObject>();
    if(obj->marked) return false;

    obj->marked = true;
    if(push) ms.push_back(obj);
    return true;
  }

  void walk_pointers(memory::Address addr, memory::Marker<SimpleObjectDescriber>& mark) {
    SimpleObject* obj = addr.as<SimpleObject>();
    obj->body_checked = true;
    if(obj->sub) {
      mark.mark_address(obj->sub);
    }
  }

  int size(memory::Address addr) {
    SimpleObject* obj = addr.as<SimpleObject>();

    if(obj->size == 0) return sizeof(SimpleObject);

    return obj->size;
  }

  void added_chunk(int size) { }
  void last_block() { }
};

class TestImmixGC : public CxxTest::TestSuite {
public:

  typedef memory::GC<SimpleObjectDescriber> GC;

  GC* gc;

  void setUp() {
    gc = new GC;
  }

  void tearDown() {
    delete gc;
  }

  void test_get_block() {
    memory::Block& block = gc->get_block();
    TS_ASSERT_EQUALS(block.size(), memory::cBlockSize);
    TS_ASSERT(block.address() != 0);
    TS_ASSERT_EQUALS(block.status(), memory::cFree);
    TS_ASSERT_EQUALS(block.lines_used(), 1U);
  }

  void test_Block_is_line_free() {
    memory::Block& block = gc->get_block();
    TS_ASSERT(block.is_line_free(1));
    block.mark_line(1);
    TS_ASSERT(block.is_line_free(1));
    block.copy_marks();
    TS_ASSERT(!block.is_line_free(1));
  }

  void test_Block_address_of_line() {
    memory::Block& block = gc->get_block();
    memory::Address top = block.address();
    TS_ASSERT_EQUALS(block.address_of_line(0U), top);
    TS_ASSERT_EQUALS(block.address_of_line(1U), top + memory::cLineSize);
  }

  void test_SingleBlockAllocator_allocate() {
    bool collect;

    memory::Block& block = gc->get_block();
    memory::SingleBlockAllocator alloc(block);
    memory::Address addr = alloc.allocate(24, collect);
    memory::Address top  = block.first_address();
    TS_ASSERT_EQUALS(addr, top);

    memory::Address another = alloc.allocate(24, collect);
    TS_ASSERT_EQUALS(another, top + 24);
  }

  void test_SingleBlockAllocator_allocate_checks_mark_on_spill() {
    memory::Block& block = gc->get_block();
    memory::Address top  = block.first_address();

    bool collect;

    block.mark_line(2);
    block.copy_marks();
    memory::SingleBlockAllocator alloc(block);
    alloc.allocate(96, collect);

    memory::Address addr = alloc.allocate(64, collect);
    TS_ASSERT_EQUALS(addr, top + (memory::cLineSize * 2));
  }

  void test_SingleBlockAllocator_allocate_spans_next_line() {
    memory::Block& block = gc->get_block();
    memory::Address top = block.first_address();

    bool collect;

    memory::SingleBlockAllocator alloc(block);
    int size = memory::cLineSize - sizeof(memory::BlockHeader) - 4;
    alloc.allocate(size, collect);
    TS_ASSERT(sizeof(SimpleObject) > 4);
    memory::Address addr = alloc.allocate(sizeof(SimpleObject), collect);

    TS_ASSERT_EQUALS(addr, top + size);

    memory::Address addr2 = alloc.allocate(memory::cLineSize + 4, collect);
    memory::Address addr3 = alloc.allocate(4, collect);
    TS_ASSERT_EQUALS(addr2, addr + sizeof(SimpleObject));
    TS_ASSERT_EQUALS(addr3, addr2 + (memory::cLineSize + 4));
  }

  void test_SingleBlockAllocator_allocate_spans_lines() {
    memory::Block& block = gc->get_block();
    memory::Address top = block.first_address();

    bool collect;

    memory::SingleBlockAllocator alloc(block);
    alloc.allocate(24, collect);
    int size = (memory::cLineSize * 2) + 32;
    memory::Address big = alloc.allocate(size, collect);
    TS_ASSERT_EQUALS(big, top + 24);

    memory::Address addr2 = alloc.allocate(24, collect);
    TS_ASSERT_EQUALS(addr2, big + size);
  }

  void test_SingleBlockAllocator_allocate_skips_marked_lines() {
    memory::Block& block = gc->get_block();
    memory::Address top  = block.address();
    block.mark_line(0);
    block.mark_line(2);
    block.mark_line(4);
    block.mark_line(5);
    block.mark_line(7);
    block.copy_marks();

    bool collect;

    memory::SingleBlockAllocator alloc(block);
    memory::Address addr = alloc.allocate(24, collect);
    TS_ASSERT_EQUALS(addr, top + memory::cLineSize);

    memory::Address addr2 = alloc.allocate(24, collect);
    TS_ASSERT_EQUALS(addr2, addr + 24);

    memory::Address addr3 = alloc.allocate(128, collect);
    TS_ASSERT_EQUALS(addr3, top + (memory::cLineSize * 3));

    memory::Address addr4 = alloc.allocate(156, collect);
    TS_ASSERT_EQUALS(addr4, top + (memory::cLineSize * 8));
  }

  void test_SingleBlockAllocator_allocate_indicates_failure() {
    memory::Block& block = gc->get_block();

    for(uint32_t i = 0; i < memory::cLineTableSize; i++) {
      block.mark_line(i);
    }

    bool collect;

    block.free_line(1);
    block.copy_marks();
    memory::SingleBlockAllocator alloc(block);
    memory::Address small = alloc.allocate(24, collect);
    TS_ASSERT(!small.is_null());

    memory::Address addr = alloc.allocate(156, collect);
    TS_ASSERT(addr.is_null());
  }

  void test_sweep_blocks_frees_empty_blocks() {
    memory::Block& block = gc->get_block();

    gc->sweep_blocks();
    memory::Block& block2 = gc->get_block();
    TS_ASSERT_EQUALS(&block, &block2);
  }

  void test_sweep_blocks_sorts_blocks() {
    memory::Block& block  = gc->get_block();
    memory::Block& block2 = gc->get_block();
    memory::Block& block3 = gc->get_block();

    block.set_status(memory::cUnavailable);
    block2.set_status(memory::cRecyclable);
    gc->evacuate_block(block3);
    TS_ASSERT_EQUALS(block3.status(), memory::cEvacuate);

    gc->sweep_blocks();

    TS_ASSERT_EQUALS(block3.status(), memory::cFree);
  }

  void test_Block_update_stats() {
    memory::Block& block = gc->get_block();

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
    memory::Block& block = gc->get_block();

    block.set_status(memory::cRecyclable);
    block.update_stats();
    TS_ASSERT_EQUALS(block.status(), memory::cFree);
    TS_ASSERT_EQUALS(block.holes(), 1U);
    TS_ASSERT_EQUALS(block.lines_used(), 1U);
  }

  void test_Block_update_stats_finds_unavailable_blocks() {
    memory::Block& block = gc->get_block();

    for(uint32_t i = 0; i < memory::cLineTableSize; i++) {
      block.mark_line(i);
    }

    block.update_stats();
    TS_ASSERT_EQUALS(block.status(), memory::cUnavailable);
    TS_ASSERT_EQUALS(block.holes(), 0U);
    TS_ASSERT_EQUALS(block.lines_used(), memory::cLineTableSize);
  }

  void test_get_block_returns_recyclable_blocks() {
    memory::Block& block  = gc->get_block();
    block.set_status(memory::cRecyclable);

    gc->sweep_blocks();

    memory::Block& block2 = gc->get_block();

    TS_ASSERT_EQUALS(&block2, &block);
  }

  void test_mark_address_updates_block() {
    bool collect;

    memory::Block& block = gc->get_block();
    memory::SingleBlockAllocator alloc(block);
    memory::Address addr = alloc.allocate(24, collect);

    TS_ASSERT(block.is_line_free(1));
    gc->mark_address(addr, alloc);
    block.copy_marks();
    TS_ASSERT(!block.is_line_free(1));
  }

  void test_mark_address_ignores_already_marked_objects() {
    bool collect;

    memory::Block& block = gc->get_block();
    memory::SingleBlockAllocator alloc(block);
    memory::Address addr = alloc.allocate(24, collect);

    addr.as<SimpleObject>()->marked = true;

    TS_ASSERT(block.is_line_free(1));
    gc->mark_address(addr, alloc);
    block.copy_marks();
    TS_ASSERT(block.is_line_free(1));
  }

  void test_mark_address_returns_forwarding_pointer() {
    bool collect;

    memory::Block& block = gc->get_block();
    memory::SingleBlockAllocator alloc(block);
    memory::Address addr = alloc.allocate(24, collect);

    // Clear out fwd to be sure it's not set
    addr.as<SimpleObject>()->fwd = 0;

    memory::Block& block2 = gc->get_block();
    memory::SingleBlockAllocator alloc2(block2);
    memory::Address addr2 = alloc2.allocate(24, collect);

    gc->describer().set_forwarding_pointer(addr, addr2);

    memory::Address out = gc->mark_address(addr, alloc);

    TS_ASSERT_EQUALS(addr.as<SimpleObject>()->fwd, addr2);
    TS_ASSERT_EQUALS(out, addr2);
  }

  void test_mark_address_can_move_objects() {
    bool collect;

    memory::Block& block = gc->get_block();
    memory::SingleBlockAllocator alloc(block);
    memory::Address addr = alloc.allocate(sizeof(SimpleObject), collect);

    addr.as<SimpleObject>()->magic = 0xdecafbad;

    memory::Block& dest = gc->get_block();
    memory::SingleBlockAllocator dest_alloc(dest);

    block.set_status(memory::cEvacuate);

    memory::Address redirect = gc->mark_address(addr, dest_alloc);

    memory::Address fwd = gc->describer().forwarding_pointer(addr);
    TS_ASSERT_EQUALS(fwd, dest.first_address());
    TS_ASSERT_EQUALS(fwd, redirect);

    TS_ASSERT_EQUALS((unsigned)fwd.as<SimpleObject>()->magic, 0xdecafbad);
  }

  void test_mark_address_calls_describer() {
    bool collect;

    memory::Block& block = gc->get_block();
    memory::SingleBlockAllocator alloc(block);
    memory::Address addr = alloc.allocate(sizeof(SimpleObject), collect);

    SimpleObject* obj = addr.as<SimpleObject>();
    memory::Address addr2 = alloc.allocate(sizeof(SimpleObject), collect);
    obj->sub = addr2.as<SimpleObject>();

    obj->marked = false;

    gc->mark_address(addr, alloc);

    TS_ASSERT_EQUALS(obj->marked, true);
    TS_ASSERT_EQUALS(gc->mark_stack().size(), 1U);
    TS_ASSERT_EQUALS(gc->mark_stack()[0], addr);
  }

  void test_mark_address_marks_all_lines_for_object() {
    bool collect;

    memory::Block& block = gc->get_block();
    memory::SingleBlockAllocator alloc(block);
    int size = memory::cLineSize - sizeof(memory::BlockHeader) - 4;
    alloc.allocate(size, collect);
    TS_ASSERT(sizeof(SimpleObject) > 4);
    memory::Address addr = alloc.allocate(sizeof(SimpleObject), collect);

    gc->mark_address(addr, alloc);
    block.copy_marks();
    TS_ASSERT(!block.is_line_free(0));
    TS_ASSERT(!block.is_line_free(1));

    int big_size = memory::cLineSize * 3;
    memory::Address addr2 = alloc.allocate(big_size, collect);
    addr2.as<SimpleObject>()->size = big_size;

    gc->mark_address(addr2, alloc);
    block.copy_marks();
    TS_ASSERT(!block.is_line_free(1));
    TS_ASSERT(!block.is_line_free(2));
    TS_ASSERT(!block.is_line_free(3));
    TS_ASSERT(!block.is_line_free(4));
  }

  void test_process_mark_stack_enabled() {
    bool collect;

    memory::Block& block = gc->get_block();
    memory::SingleBlockAllocator alloc(block);
    memory::Address addr = alloc.allocate(sizeof(SimpleObject), collect);
    memory::Address addr2 = alloc.allocate(sizeof(SimpleObject), collect);

    SimpleObject* obj = addr.as<SimpleObject>();
    SimpleObject* sub = addr2.as<SimpleObject>();

    obj->marked = false;
    obj->sub = sub;
    obj->body_checked = false;

    sub->marked = false;
    sub->sub = 0;
    sub->body_checked = false;

    gc->mark_address(addr, alloc);
    TS_ASSERT_EQUALS(obj->marked, true);

    bool exit = false;
    gc->process_mark_stack(alloc, exit);
    TS_ASSERT_EQUALS(obj->body_checked, true);
    TS_ASSERT_EQUALS(sub->marked, true);
    TS_ASSERT_EQUALS(sub->body_checked, true);
  }

  void test_process_mark_stack_disabled() {
    bool collect;

    memory::Block& block = gc->get_block();
    memory::SingleBlockAllocator alloc(block);
    memory::Address addr = alloc.allocate(sizeof(SimpleObject), collect);
    memory::Address addr2 = alloc.allocate(sizeof(SimpleObject), collect);

    SimpleObject* obj = addr.as<SimpleObject>();
    SimpleObject* sub = addr2.as<SimpleObject>();

    obj->marked = false;
    obj->sub = sub;
    obj->body_checked = false;

    sub->marked = false;
    sub->sub = 0;
    sub->body_checked = false;

    gc->mark_address(addr, alloc, false);
    TS_ASSERT_EQUALS(obj->marked, true);

    bool exit = false;
    gc->process_mark_stack(alloc, exit);
    TS_ASSERT_EQUALS(obj->body_checked, false);
    TS_ASSERT_EQUALS(sub->marked, false);
    TS_ASSERT_EQUALS(sub->body_checked, false);
  }

  void test_BlockAllocator_reset_updates_block_stats() {
    memory::Block& block = gc->get_block();

    block.mark_line(1);
    block.mark_line(3);

    TS_ASSERT_EQUALS(block.lines_used(), 1U);
    memory::BlockAllocator& ba = gc->block_allocator();
    ba.reset();

    TS_ASSERT_EQUALS(block.lines_used(), 3U);
  }

  void test_BlockAllocator_get_free_block() {
    memory::BlockAllocator& ba = gc->block_allocator();
    ba.add_chunk();
    ba.current_chunk().get_block(0).set_status(memory::cRecyclable);
    memory::Block& block = ba.get_free_block();
    TS_ASSERT_EQUALS(block.status(), memory::cFree);
  }

  void test_BlockAllocator_uses_all_available_chunks_first() {
    memory::BlockAllocator& ba = gc->block_allocator();
    memory::Chunk *c1 = ba.add_chunk();
    memory::Chunk* c2 = ba.add_chunk();
    ba.reset();

    TS_ASSERT_EQUALS(&ba.current_chunk(), c1);

    for(uint32_t i = 0; i < memory::cBlocksPerChunk + 1; i++) {
      ba.get_block();
    }

    TS_ASSERT_EQUALS(&ba.current_chunk(), c2);
  }

  void test_BlockAllocator_skips_unavailable_blocks() {
    memory::BlockAllocator& ba = gc->block_allocator();
    memory::Chunk* chunk = ba.add_chunk();
    ba.reset();
    chunk->get_block(0).set_status(memory::cUnavailable);

    memory::Block& block = ba.get_block();

    TS_ASSERT_EQUALS(&block, &chunk->get_block(1));
  }

  void test_BlockAllocator_skips_evacuating_blocks() {
    memory::BlockAllocator& ba = gc->block_allocator();
    memory::Chunk* chunk = ba.add_chunk();
    ba.reset();
    chunk->get_block(0).set_status(memory::cEvacuate);

    memory::Block& block = ba.get_block();

    TS_ASSERT_EQUALS(&block, &chunk->get_block(1));
  }

  void test_HoleFinder_find_hole_on_empty_block() {
    memory::Block& block = gc->get_block();
    memory::HoleFinder alloc;
    alloc.reset(&block);

    alloc.find_hole();

    TS_ASSERT_EQUALS(alloc.cursor(), block.first_address());
    TS_ASSERT_EQUALS(alloc.limit(),  block.address() + memory::cBlockSize);
    TS_ASSERT_EQUALS(alloc.hole_start_line(), memory::cLineTableSize);
  }

  void test_HoleFinder_find_hole_with_hole_in_middle() {
    memory::Block& block = gc->get_block();
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

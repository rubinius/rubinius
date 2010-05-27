#ifndef RBX_UTIL_IMMIX
#define RBX_UTIL_IMMIX

#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <list>
#include <vector>

#include <sys/mman.h>

namespace immix {

  /* A wonderful little class that is used always as a value, never
   * a reference or pointer. It basically allows us the ability to
   * implement our own pointer logic. */
  struct Address {
    uintptr_t address_;

    Address(void* addr)
      : address_(reinterpret_cast<intptr_t>(addr))
    {}

    operator void*() {
      return reinterpret_cast<void*>(address_);
    }

    Address operator+(int change) {
      return Address(reinterpret_cast<void*>(address_ + change));
    }

    Address operator+=(int change) {
      address_ += change;
      return *this;
    }

    Address operator-(Address change) {
      return Address(reinterpret_cast<void*>(address_ - change.address_));
    }

    Address operator-(int change) {
      return Address(reinterpret_cast<void*>(address_ - change));
    }

    Address operator&(uintptr_t mask) {
      return Address(reinterpret_cast<void*>(address_ & mask));
    }

    bool is_null() {
      return address_ == 0;
    }

    static Address null() {
      return Address(0);
    }

    intptr_t as_int() {
      return address_;
    }

    template <typename T>
      T* as() {
        return reinterpret_cast<T*>(reinterpret_cast<void*>(address_));
      }
  };

  const int cBlockSize = 32768;
  const int cBlockMask = cBlockSize - 1;
  const int cLineBits  = 7;
  const int cLineSize  = 1 << cLineBits;
  const int cLineMask  = cLineSize - 1;
  const int cLineTableSize = cBlockSize / cLineSize;
  const int cChunkSize = 10 * 1024 * 1024;
  const int cBlocksPerChunk = cChunkSize / cBlockSize;
  const int cMediumObjectLimit = cLineSize * 4; // TODO calculate this

  enum BlockStatus {
    cFree,
    cRecyclable,
    cUnavailable,
    cEvacuate
  };

  typedef uint8_t LineEntry;

  class Block;

  struct BlockHeader {
    Block* block;
  };

  class Block {
    Address address_;
    BlockStatus status_;
    int holes_;
    int lines_used_;
    int objects_;
    int object_bytes_;
    LineEntry lines_[cLineTableSize];

  public:
    Block()
      : address_(0)
      , status_(cFree)
      , holes_(1)
      , lines_used_(1)
      , objects_(0)
      , object_bytes_(0)
    {
      clear_lines();
    }

    void clear_lines() {
      objects_ = 0;
      object_bytes_ = 0;
      memset(lines_, 0, sizeof(lines_));
      lines_[0] = 1; // always exclude the first line, it's got metadata
    }

    void set_address(Address addr) {
      address_ = addr;
    }

    static int size() {
      return immix::cBlockSize;
    }

    int holes() const {
      return holes_;
    }

    Address address() const {
      return address_;
    }

    Address first_address() {
      return address_ + cLineSize; // skip line 0
    }

    BlockStatus status() const {
      return status_;
    }

    void set_status(BlockStatus status) {
      status_ = status;
    }

    int lines_used() const {
      return lines_used_;
    }

    int objects() const {
      return objects_;
    }

    int object_bytes() const {
      return object_bytes_;
    }

    void mark_line(int line) {
      lines_[line] = 1;
    }

    void free_line(int line) {
      lines_[line] = 0;
    }

    bool is_line_free(int line) {
      return lines_[line] == 0;
    }

    int offset_of_line(int line) {
      return line * cLineSize;
    }

    Address address_of_line(int line) {
      return address_ + (line * cLineSize);
    }

    static Block* from_address(Address addr) {
      Address base = addr & ~cBlockMask;
      BlockHeader* header = base.as<BlockHeader>();
      return header->block;
    }

    static Address align(Address addr) {
      return addr & ~cBlockMask;
    }

    void mark_address(Address addr, int size) {
      Address offset = addr - address_;
      int line = offset.as_int() / cLineSize;
      mark_line(line);

      // the immix paper talks about doing conservative line marking
      // here. We're going to do accurate for now.

      int line_offset = (addr & cLineMask).as_int();
      int additional_lines = ((line_offset + size - 1) >> cLineBits);

      for(int i = 1; i <= additional_lines; i++) {
        mark_line(line + i);
      }

      // Also, track how many times this was called, ie, how many objects this
      // block contains.
      objects_++;

      // Also track how much exact space these objects take up
      object_bytes_ += size;
    }

    void update_stats() {
      holes_ = 0;
      lines_used_ = 0;
      bool in_hole = false;
      for(int i = 0; i < cLineTableSize; i++) {
        if(lines_[i] == 0) {
          if(!in_hole) holes_++;
          in_hole = true;
        } else {
          in_hole = false;
          lines_used_++;
        }
      }

      // 1 is always used for metadata
      if(lines_used_ <= 1) {
        status_ = cFree;
      } else if(holes_ >= 1) {
        status_ = cRecyclable;
      } else {
        status_ = cUnavailable;
      }
    }

    int bytes_from_lines() {
      return lines_used_ * cLineSize;
    }

    bool usable() {
      return status_ == cFree || status_ == cRecyclable;
    }

    const char* status_string() {
      switch(status_) {
      case cFree:
        return "free";
      case cRecyclable:
        return "recyclable";
      case cUnavailable:
        return "unavailable";
      case cEvacuate:
        return "evacuate";
      }

      return "unknown";
    }

    double fragmentation_ratio() {
      // We subtract the size of a pointer because thats not data available to
      // use for objects, so we shouldn't count it. 4 bytes in the grand scheme
      // doesn't probably matter for this, but we should be fair.
      return ((double)object_bytes_) / ((double)(cBlockSize - sizeof(BlockHeader*)));
    }
  };

  typedef std::vector<Block*> Blocks;

  class Chunk {
    Address system_base_;
    std::size_t system_size_;

    Address base_;
    Block blocks_[cBlocksPerChunk];

  public:
    Chunk()
      : system_base_(0)
      , base_(0)
    {
      base_ = mmap(0, cChunkSize, PROT_EXEC | PROT_READ | PROT_WRITE,
           MAP_ANON | MAP_PRIVATE, -1, 0);

      if(base_.as_int() == -1) {
        perror("mmap");
        ::abort();
      }

      // Best case scenario
      if(base_ == Block::align(base_)) {
        system_base_ = base_;
        system_size_ = cChunkSize;
      } else {
        assert(munmap(base_, cChunkSize) == 0);

        system_size_ = cChunkSize + cBlockSize;
        system_base_ = mmap(0, system_size_, PROT_EXEC | PROT_READ | PROT_WRITE,
            MAP_ANON | MAP_PRIVATE, -1, 0);

        base_ = Block::align(system_base_ + cBlockSize);
      }

      add_blocks();
    }

    void free() {
      if(munmap(system_base_, system_size_) != 0) {
        perror("munmap");
        ::abort();
      }
    }

    Address base() {
      return base_;
    }

    std::size_t size() {
      return system_size_;
    }

    Address last_address() {
      return system_base_ + system_size_;
    }

    void add_blocks() {
      assert(base_ == Block::align(base_));

      Address current = base_;

      for(int index = 0; index < cBlocksPerChunk; index++) {
        Block& block = blocks_[index];
        block.set_address(current);
        BlockHeader* header = current.as<BlockHeader>();
        header->block = &block;
        current += cBlockSize;
      }
    }

    Block& get_block(int index) {
      return blocks_[index];
    }

    void update_stats() {
      for(int i = 0; i < cBlocksPerChunk; i++) {
        blocks_[i].update_stats();
      }
    }

    bool contains_address(Address addr) {
      return addr > base_ && addr <= last_address();
    }
  };

  typedef std::vector<Chunk*> Chunks;

  class AllBlockIterator {
    Chunks& chunks_;
    size_t current_chunk_;
    size_t current_block_;

  public:
    AllBlockIterator(Chunks& chunks)
      : chunks_(chunks)
      , current_chunk_(0)
      , current_block_(0)
    {}

    Block* next() {
      if(current_chunk_ >= chunks_.size()) return NULL;

      Block* block = &chunks_[current_chunk_]->get_block(current_block_++);
      if(current_block_ >= (size_t)cBlocksPerChunk) {
        current_chunk_++;
        current_block_ = 0;
      }

      return block;
    }
  };

  class Triggers {
  public:
    virtual ~Triggers() { }
    virtual void added_chunk(int count) = 0;
    virtual void last_block() = 0;
  };

  class BlockAllocator {
    Triggers& triggers_;
    Chunks chunks_;
    Chunk* current_chunk_;
    size_t chunk_cursor_;
    size_t block_cursor_;

    // Used when allocating free-only blocks.
    size_t free_chunk_cursor_;
    size_t free_block_cursor_;

    // Stats
    size_t bytes_allocated_;
  public:

    BlockAllocator(Triggers& trig)
      : triggers_(trig)
      , current_chunk_(0)
      , chunk_cursor_(0)
      , block_cursor_(0)
      , free_chunk_cursor_(0)
      , free_block_cursor_(0)
      , bytes_allocated_(0)
    {}

    ~BlockAllocator() {
      for(Chunks::iterator i = chunks_.begin();
          i != chunks_.end();
          i++) {
        Chunk* chunk = *i;
        chunk->free();
      }
    }

    size_t& bytes_allocated() {
      return bytes_allocated_;
    }

    Chunks& chunks() {
      return chunks_;
    }

    Chunk& current_chunk() {
      return *current_chunk_;
    }

    Block& get_block() {
      if(current_chunk_ == 0) {
        add_chunk();
        return current_chunk_->get_block(0);
      }

      for(;;) {
        if(block_cursor_ >= (size_t)cBlocksPerChunk) {
          chunk_cursor_++;
          if(chunk_cursor_ >= chunks_.size()) {
            add_chunk();
          } else {
            block_cursor_ = 0;
          }

          current_chunk_ = chunks_[chunk_cursor_];
        }

        Block& block = current_chunk_->get_block(block_cursor_++);
        if(chunk_cursor_ == chunks_.size() - 1 &&
            block_cursor_ == (size_t)cBlocksPerChunk - 5) {
          triggers_.last_block();
        }

        if(block.usable()) return block;
      }
    }

    Block& get_free_block() {
      if(current_chunk_ == 0) {
        add_chunk();
        return current_chunk_->get_block(0);
      }

      for(int i = block_cursor_; i < cBlocksPerChunk; i++) {
        Block& block = current_chunk_->get_block(i);
        if(block.status() == cFree) return block;
      }

      add_chunk();
      return current_chunk_->get_block(0);
    }

    Chunk* add_chunk() {
      chunks_.push_back(new Chunk);
      Chunk* chunk = chunks_.back();
      chunk_cursor_ = chunks_.size() - 1;
      block_cursor_ = 1;
      current_chunk_ = chunk;

      bytes_allocated_ += chunk->size();

      triggers_.added_chunk(chunks_.size());
      return chunk;
    }

    void reset() {
      for(Chunks::iterator i = chunks_.begin();
          i != chunks_.end();
          i++) {
        Chunk* chunk = *i;
        chunk->update_stats();
      }
      chunk_cursor_ = 0;
      block_cursor_ = 0;
      current_chunk_ = chunks_[chunk_cursor_];
    }

  };

  class Allocator {
  public:
    virtual ~Allocator() {}
    virtual Address allocate(int bytes) = 0;
  };

  class HoleFinder {
  protected:
    Address cursor_;
    Address limit_;
    int hole_start_line_;
    Block* block_;

  public:
    HoleFinder()
      : cursor_(0)
      , limit_(0)
      , hole_start_line_(0)
      , block_(0)
    {}

    Address cursor() {
      return cursor_;
    }

    Address limit() {
      return limit_;
    }

    int hole_start_line() {
      return hole_start_line_;
    }

    Block& block() {
      return *block_;
    }

    bool reset(Block* block = 0) {
      if(block) block_ = block;
      hole_start_line_ = 0;
      return find_hole();
    }

    bool find_hole() {
      for(; hole_start_line_ < cLineTableSize; hole_start_line_++) {
        if(block_->is_line_free(hole_start_line_)) {
          cursor_ = block_->address_of_line(hole_start_line_);

          // Compensate for the header
          /*
          if(hole_start_line_ == 0) {
            cursor_ = block_->first_address();
          }
          */

          while(hole_start_line_ < cLineTableSize &&
                block_->is_line_free(hole_start_line_)) {
            hole_start_line_++;
          }

          limit_ = block_->address_of_line(hole_start_line_);
          return true;
        }
      }

      return false;
    }

    Address bump(int size) {
      Address alloc = cursor_;
      cursor_ += size;
      return alloc;
    }

  };

  class SingleBlockAllocator : public HoleFinder, public Allocator {
  public:

    SingleBlockAllocator(Block& block) {
      assert(reset(&block));
    }

    Address allocate(int size) {
      while(cursor_ + size > limit_) {
        if(!find_hole()) {
          return Address::null();
        }
      }

      return bump(size);
    }
  };

  class ExpandingAllocator : public HoleFinder, public Allocator {
    BlockAllocator& block_allocator_;

  public:
    ExpandingAllocator(BlockAllocator& ba)
      : block_allocator_(ba)
    {
      get_new_block();
    }

    Block& current_block() {
      return block();
    }

    void get_new_block(bool free_only = false) {
      for(;;) {
        Block* block = &block_allocator_.get_block();
        /*
         *
         * DISABLED FOR NOW.
        if(free_only) {
          block = &block_allocator_.get_free_block();
        } else {
          block = &block_allocator_.get_block();
        }
        */

        if(reset(block)) break;
      }
    }

    void resync_position() {
      if(!reset()) get_new_block();
    }

    Address allocate(int size) {
      while(cursor_ + size > limit_) {
        if(!find_hole()) {
          get_new_block(size >= cMediumObjectLimit);
        }
      }

      return bump(size);
    }
  };

  typedef std::vector<Address> MarkStack;

  template <typename Describer> class GC;

  template <typename Describer>
  class Marker {
    GC<Describer>* gc;
    Allocator& alloc;

  public:
    Marker(GC<Describer>* gc, Allocator& alloc)
      : gc(gc)
      , alloc(alloc)
    {}

    void mark_address(Address addr) {
      gc->mark_address(addr, alloc);
    }
  };

  typedef std::list<Block*> BlockList;

  template <typename Describer>
  class GC : public Triggers {
    BlockList evacuate_;
    BlockAllocator block_allocator_;

    Describer desc;
    MarkStack mark_stack_;

  public:
    GC()
      : block_allocator_(*this)
    {}

    Describer& describer() {
      return desc;
    }

    MarkStack& mark_stack() {
      return mark_stack_;
    }

    BlockAllocator& block_allocator() {
      return block_allocator_;
    }

    Block& get_block() {
      return block_allocator_.get_block();
    }

    void evacuate_block(Block& block) {
      block.set_status(cEvacuate);
      evacuate_.push_back(&block);
    }

    void sweep_blocks() {
      for(BlockList::const_iterator i = evacuate_.begin();
          i != evacuate_.end();
          i++) {
        Block* block = *i;
        if(block->status() == cEvacuate) {
          block->set_status(cFree);
        }
      }

      block_allocator_.reset();
    }

    Address mark_address(Address addr, Allocator& alloc) {
      Address fwd = desc.forwarding_pointer(addr);

      if(!fwd.is_null()) {
        addr = fwd;
      }

      // Returns false if addr is already marked, if so, we don't
      // do the block marking logic again.
      if(!desc.mark_address(addr, mark_stack_)) {
        return addr;
      }

      Block* block = Block::from_address(addr);
      if(block->status() == cEvacuate && !desc.pinned(addr)) {
        fwd = desc.copy(addr, alloc);
        desc.set_forwarding_pointer(addr, fwd);

        addr = fwd;
        block = Block::from_address(addr);
      }

      block->mark_address(addr, desc.size(addr));

      return addr;
    }

    void process_mark_stack(Allocator& alloc) {
      Marker<Describer> mark(this, alloc);

      // Use while() since mark_stack_ is modified as we walk it.
      while(!mark_stack_.empty()) {
        Address addr = mark_stack_.back();
        mark_stack_.pop_back();
        desc.walk_pointers(addr, mark);
      }
    }

    void added_chunk(int count) {
      desc.added_chunk(count);
    }

    void last_block() {
      desc.last_block();
    }

    void clear_lines() {
      AllBlockIterator iter(block_allocator_.chunks());

      while(Block* block = iter.next()) {
        block->clear_lines();
      }
    }

    size_t& bytes_allocated() {
      return block_allocator_.bytes_allocated();
    }

    bool allocated_address(Address addr) {
      Chunks& chunks = block_allocator_.chunks();
      for(Chunks::iterator i = chunks.begin();
          i != chunks.end();
          i++) {
        if((*i)->contains_address(addr)) return true;
      }

      return false;
    }
  };
}

#endif

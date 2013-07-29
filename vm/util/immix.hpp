#ifndef RBX_UTIL_IMMIX
#define RBX_UTIL_IMMIX

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <list>
#include <vector>

#include "util/address.hpp"
#include "util/gc_alloc.hpp"

using memory::Address;


namespace immix {

  /// Size of a Block; 32K, to match the size of a page of virtual memory
  const uint32_t cBlockSize = 32768;

  /// Mask bits, used to align blocks at cBlockSize boundaries
  const uintptr_t cBlockMask = cBlockSize - 1;

  /// Number of bits needed to hold the line size; used to derive cLineSize,
  /// and to calculate the number of lines an object of a given size would
  /// occupy.
  const uint32_t cLineBits  = 7;

  /// The size of a Line; should be a multiple of the processor cache line size,
  /// and sufficient to hold several typical objects; we use 128 bytes.
  /// @todo Check impact of different line sizes
  const uint32_t cLineSize  = 1 << cLineBits;

  /// Line mask used to convert an objects Address to the Address of the start
  /// of the line.
  const uintptr_t cLineMask  = cLineSize - 1;

  /// Each block consists of an array of lines, known as the line table.
  /// The line table array is sized to fill the Block.
  const uint32_t cLineTableSize = cBlockSize / cLineSize;

  /// Memory for blocks is allocated in chunks; these are set to be 10 MB, thus
  /// each chunk has space for 320 blocks.
  const uint32_t cChunkSize = 10 * 1024 * 1024;

  /// Number of Blocks that fit within a Chunk
  const uint32_t cBlocksPerChunk = cChunkSize / cBlockSize;

  const uint32_t cMaxObjectSize = cBlockSize - cLineSize; // one reserved line

  /// Objects above a certain number of lines in size are considered to be
  /// medium sized objects, which should be allocated only in free blocks.
  /// As the likelihood of finding this many contiguous free lines in a
  /// recycled block rapidly diminishes as the number of lines increases,
  /// we don't bother searching partially free blocks if the size of the
  /// object is at or above this number of lines.
  const uint32_t cMediumObjectLimit = cLineSize * 4; //< @todo calculate this

  /**
   * Enumeration of possible block states.
   */
  enum BlockStatus {
    cFree,          //< Block is completely free
    cRecyclable,    //< Block is in use, but has at least one free line
    cUnavailable,   //< Block is fully in use, with no free lines
    cEvacuate       //> Block is fragmented, and a candidate for evacuation
  };


  /// Each line requires a byte for marking
  typedef uint8_t LineEntry;

  class Block;

  /**
   * A pointer to the Block object that contains the metadata for the chunk of
   * memory managed by the Block. A BlockHeader is always stored at the start of
   * the memory managed by the Block, which is why the first line of the Block
   * memory is reserved.
   */

  struct BlockHeader {
    Block* block;
  };


  /**
   * Immix manages memory at several granularities; a Block is sized to 32K to
   * fit a page of virtual memory, with the block then sub-divided into lines.
   *
   * Block instances are used to manage these 32K slabs of memory obtained from
   * Chunks. Note that a Block instance does not reside in the memory it manages.
   *
   * The first line of a Block is always marked as in use, as it contains a
   * BlockHeader object that contains a pointer back to the Block object that
   * is managing that memory.
   */

  class Block {

    /// Address of the start of the memory managed by this Block
    Address address_;

    /// Status of the Block
    BlockStatus status_;

    /// Number of holes in the block from which allocations can be made.
    /// A Block starts with one hole the size of the free memory in the block.
    uint32_t holes_;

    /// Number of lines used in this Block
    uint32_t lines_used_;

    /// Number of objects stored in this Blocks memory
    uint32_t objects_;

    /// Number of bytes used by objects stored in this Blocks memory
    uint32_t object_bytes_;

    /// Map of in-use lines in the Block
    LineEntry lines_[cLineTableSize];
    LineEntry marks_[cLineTableSize];

  public:
    Block()
      : address_(0)
      , status_(cFree)
      , holes_(1)
      , lines_used_(1)
      , objects_(0)
      , object_bytes_(0)
    {
      memset(lines_, 0, sizeof(lines_));
      memset(marks_, 0, sizeof(marks_));
      marks_[0] = 1;
      lines_[0] = 1;
    }

    /**
     * Clears all lines in the Block, making the memory managed by this Block
     * available for allocation.
     */
    void clear_marks() {
      objects_ = 0;
      object_bytes_ = 0;
      memset(marks_, 0, sizeof(marks_));
      // Always exclude the first line, it's got metadata in the form of a
      // pointer back to this Block object managing that memory.
      marks_[0] = 1;
    }

    void copy_marks() {
      memcpy(lines_, marks_, sizeof(lines_));
    }

    /**
     * Overwrites memory inside the blocks that is not used according to
     * the lines found.
     * Is used for debugging purposes to see easier when an object
     * isn't properly marked.
     */
    void clear_memory() {
      Address start = address_;
      for(uint32_t i = 0; i < cLineTableSize; ++i) {
        if(!lines_[i]) {
          memset((void*)start.address_, 0xFF, cLineSize);
        }
        start += cLineSize;
      }
    }

    /**
     * Sets the location of the memory managed by this Block.
     */
    void set_address(Address addr) {
      address_ = addr;
    }

    /**
     * Returns the size of the memory managed by this Block. All Blocks have
     * a fixed size.
     */
    static uint32_t size() {
      return immix::cBlockSize;
    }

    /**
     * Returns the number of holes in the Blocks memory.
     */
    uint32_t holes() const {
      return holes_;
    }

    /**
     * Returns the Address of the start of the Block.
     */
    Address address() const {
      return address_;
    }

    /**
     * Returns the Address of the first allocation point in the Block.
     * This is not the same as the start of the Block, as the Block maintains
     * some metadata (i.e. a BlockHeader) in the first line.
     */
    Address first_address() const {
      return address_ + cLineSize; // skip line 0
    }

    /**
     * Returns the status of the Block.
     */
    BlockStatus status() const {
      return status_;
    }

    /**
     * Sets the status of the Block.
     */
    void set_status(BlockStatus status) {
      status_ = status;
    }

    /**
     * Returns the number of lines in use in the Block.
     */
    uint32_t lines_used() const {
      return lines_used_;
    }

    /**
     * Returns a count of the number of objects currently allocated in the
     * memory managed by this Block.
     */
    uint32_t objects() const {
      return objects_;
    }

    /**
     * Returns the number of bytes allocated to objects in this Block.
     */
    uint32_t object_bytes() const {
      return object_bytes_;
    }

    /**
     * Marks a line of memory as in use.
     */
    void mark_line(uint32_t line) {
      marks_[line] = 1;
    }

    /**
     * Marks a line of memory as free.
     */
    void free_line(uint32_t line) {
      marks_[line] = 0;
    }

    /**
     * Returns true if +line+ is currently free.
     */
    bool is_line_free(uint32_t line) const {
      return lines_[line] == 0;
    }

    /**
     * Returns the offset in bytes from the start of the block to the start of
     * the specified +line+.
     */
    static uint32_t offset_of_line(uint32_t line) {
      return line * cLineSize;
    }

    /**
     * Returns the memory Address of the start of the specified line.
     */
    Address address_of_line(uint32_t line) const {
      return address_ + (line * cLineSize);
    }

    /**
     * Given an Address +addr+ in memory managed by a Block, returns a pointer
     * to that Block.
     *
     * This method relies on the fact that the memory managed by a Block is
     * aligned at a cBlockSize boundary, and that a pointer to the managing
     * Block is stored at the start of the memory range managed by the Block.
     */
    static Block* from_address(Address addr) {
      Address base = addr & ~cBlockMask;
      BlockHeader* header = base.as<BlockHeader>();
      return header->block;
    }

    /**
     * Returns the Address at or below the supplied Address +addr+ that is
     * a multiple of cBlockSize.
     */
    static Address align(Address addr) {
      return addr & ~cBlockMask;
    }

    /**
     * Marks the chunk of memory starting at +addr+ and extending for +size+
     * bytes as being in use. This involves ensuring the line map records each
     * line occupied by the range as in use.
     */
    void mark_address(Address addr, uint32_t size) {
      // Mark the line containing +addr+ as in use
      size_t offset = addr - address_;
      uint32_t line = offset / cLineSize;
      mark_line(line);

      // Next, determine how many lines this object is occupying.
      // We're doing conservative marking here, according to the
      // immix paper. This means that for small objects we always
      // mark the current and the next page, only in case of a big
      // object we exactly determine the number of lines it uses.
      if(size <= cLineSize && line + 1 < cLineTableSize) {
        mark_line(line + 1);
      } else {
        uint32_t line_offset = (addr & cLineMask).as_int();
        uint32_t additional_lines = ((line_offset + size - 1) >> cLineBits);

        for(uint32_t i = 1; i <= additional_lines; i++) {
          mark_line(line + i);
        }
      }
      // Track how many times this was called, ie, how many objects this
      // block contains.
      objects_++;

      // Also track how much exact space these objects take up
      object_bytes_ += size;
    }

    /**
     * Recalculates line usage and the number of holes in this Block, using
     * the results to update the Block status.
     */
    void update_stats() {
      holes_ = 0;
      lines_used_ = 0;
      bool in_hole = false;
      for(uint32_t i = 0; i < cLineTableSize; i++) {
        if(marks_[i] == 0) {
          if(!in_hole) holes_++;
          in_hole = true;
        } else {
          in_hole = false;
          lines_used_++;
        }
      }

      // The first line is always used for metadata
      if(lines_used_ <= 1) {
        status_ = cFree;
      } else if(holes_ >= 1) {
        status_ = cRecyclable;
      } else {
        status_ = cUnavailable;
      }
    }

    /**
     * Returns the number of bytes of this Block's memory that are in use or
     * unavailable. This differs from object_bytes in that it includes the
     * cost of wasted bytes in lines that are only partially filled.
     */
    uint32_t bytes_from_lines() const {
      return lines_used_ * cLineSize;
    }

    /**
     * Returns true if the Block has free space available for allocations.
     */
    bool usable() const {
      return status_ == cFree || status_ == cRecyclable;
    }

    /**
     * Returns the status of the block as a string.
     */
    const char* status_string() const {
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

    /**
     * Returns the percentage of the block that is used for object storage.
     * This will rarely be 100%, even on fully occupied blocks, due to the
     * likelihood of wasted space on lines that are marked in use but not
     * fully occupied.
     */
    double fragmentation_ratio() const {
      // We subtract the size of the first line because thats not data available to
      // use for objects, so we shouldn't count it.
      return ((double)object_bytes_) / ((double)(cBlockSize - cLineSize));
    }
  };


  /**
   * A Chunk manages a slab of memory allocated from the virtual memory of the
   * O/S. This memory is then sub-divided into Blocks (which are in turn sub-
   * divided into lines).
   */

  class Chunk {

    /// Address of the start of the memory range managed by this Chunk.
    /// May be different than the base_ address is the O/S gives us
    /// memory that is not aligned to an exact multiple of cBlockSize.
    Address system_base_;

    /// Size of memory allocated to this Chunk. May be larger than cChunkSize
    /// if the memory is not cBlockSize aligned.
    std::size_t system_size_;

    /// Address of the first byte of memory assigned to a Block.
    Address base_;

    /// Contains the metadata for all Blocks in this Chunk.
    Block blocks_[cBlocksPerChunk];

  public:

    /**
     * Constructor; obtains a range of memory from the O/S, and ensures that
     * Blocks are always aligned on a cBlockSize boundary.
     */
    Chunk()
      : system_base_(0)
      , base_(0)
    {
      base_ = memory::gc_alloc(cChunkSize);

      if(base_ == Block::align(base_)) {
        // Best case scenario - returned memory block is aligned as needed
        system_base_ = base_;
        system_size_ = cChunkSize;
      } else {
        // Ask for a larger chunk so we can align it as needed ourselves
        memory::gc_free(base_, cChunkSize);
        system_size_ = cChunkSize + cBlockSize;
        system_base_ = memory::gc_alloc(system_size_);

        base_ = Block::align(system_base_ + cBlockSize);
      }

      add_blocks();
    }

    void free() {
      memory::gc_free(system_base_, system_size_);
    }

    Address base() const {
      return base_;
    }

    std::size_t size() const {
      return system_size_;
    }

    Address last_address() const {
      return system_base_ + system_size_;
    }

    /**
     * Fills the memory allocated to this Chunk with Blocks.
     */
    void add_blocks() {
      assert(base_ == Block::align(base_));

      Address current = base_;

      for(uint32_t index = 0; index < cBlocksPerChunk; index++) {
        Block& block = blocks_[index];
        block.set_address(current);
        BlockHeader* header = current.as<BlockHeader>();
        header->block = &block;
        current += cBlockSize;
      }
    }

    /**
     * Returns a reference to the +index+-th block.
     */
    Block& get_block(uint32_t index) {
      return blocks_[index];
    }

    /**
     * Updates the stats (and status) for all Blocks in this Chunk.
     */
    void update_stats() {
      for(uint32_t i = 0; i < cBlocksPerChunk; i++) {
        blocks_[i].update_stats();
      }
    }

    /**
     * Returns true if this Chunk contains the specified +addr+ Address.
     */
    bool contains_address(Address addr) const {
      return addr > base_ && addr <= last_address();
    }
  };

  typedef std::vector<Chunk*> Chunks;


  /**
   * Iterator class for traversing all the immix Blocks across all Chunks.
   */

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


  /**
   * Virtual class representing events the BlockAllocator will raise on the
   * GC.
   */

  class Triggers {
  public:
    virtual ~Triggers() { }

    /**
     * Callback called when the BlockAllocator allocates a new Chunk.
     * /param count The total number of chunks currently allocated.
     */
    virtual void added_chunk(uint32_t count) = 0;

    /**
     * Callback called when the BlockAllocator is approaching the end of the
     * list of available blocks.
     */
    virtual void last_block() = 0;
  };


  /**
   * Used to manage the allocation of Blocks. As Blocks are created in batches,
   * this object allocates new Chunks as necessary to satisfy requests for new
   * Blocks. To provide feedback to a garbage collector, the BlockAllocator
   * takes a Triggers object, which it will notify whenever it allocates a new
   * Chunk, and when it is near the end of available Blocks.
   */

  class BlockAllocator {

    /// Holds a reference to a Triggers object, on which the BlockAllocator
    /// will fire callbacks for chunks added, low memory, etc.
    Triggers& triggers_;

    /// Vector of Chunks managed by this BlockAllocator.
    Chunks chunks_;

    /// Pointer to the current Chunk
    Chunk* current_chunk_;

    /// Index into the chunks_ vector for the current Chunk being iterated
    size_t chunk_cursor_;

    /// Index of the current Block within the current Chunk
    size_t block_cursor_;

    // Stats
    size_t bytes_allocated_;

  public:

    BlockAllocator(Triggers& trig)
      : triggers_(trig)
      , current_chunk_(0)
      , chunk_cursor_(0)
      , block_cursor_(0)
      , bytes_allocated_(0)
    {}

    ~BlockAllocator() {
      for(Chunks::iterator i = chunks_.begin();
          i != chunks_.end();
          ++i) {
        Chunk* chunk = *i;
        chunk->free();
        delete chunk;
      }
    }

    /**
     * Returns a reference to the number of bytes currently allocated to
     * Chunks.
     */
    size_t& bytes_allocated() {
      return bytes_allocated_;
    }

    Chunks& chunks() {
      return chunks_;
    }

    Chunk& current_chunk() {
      return *current_chunk_;
    }

    /**
     * Returns a Block from which allocations can be made. This may be a new
     * Block that is totally free, or a recyclable block, which contains at
     * least one free line. If necessary, this will add a new Chunk in order
     * to satisfy the request.
     */
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

    /**
     * Returns an entirely free block, adding a new Chunk if necessary.
     */
    Block& get_free_block() {
      if(current_chunk_ == 0) {
        add_chunk();
        return current_chunk_->get_block(0);
      }

      for(uint32_t i = block_cursor_; i < cBlocksPerChunk; i++) {
        Block& block = current_chunk_->get_block(i);
        if(block.status() == cFree) return block;
      }

      add_chunk();
      return current_chunk_->get_block(0);
    }

    /**
     * Adds a new Chunk to the list of Chunks managed by this BlockAllocator,
     * from which Blocks can be allocated.
     */
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
          ++i) {
        Chunk* chunk = *i;
        chunk->update_stats();
      }
      chunk_cursor_ = 0;
      block_cursor_ = 0;
      current_chunk_ = chunks_[chunk_cursor_];
    }

  };


  /**
   * Locates holes (i.e. free space) within a Block, by scanning the line
   * map. If a hole is found, the Address of the start of the empty line
   * can be obtained from the +cursor+ property, while the +limit+
   * property identifies the Address of the start of the last empty line.
   *
   * @todo Improve encapsulation by having HoleFinder check the hole it
   * has found is large enough
   */
  class HoleFinder {
  protected:
    Address cursor_;
    Address limit_;
    uint32_t hole_start_line_;
    Block* block_;

  public:
    HoleFinder()
      : cursor_(0)
      , limit_(0)
      , hole_start_line_(0)
      , block_(0)
    {}

    /**
     * Returns the next available address from which allocations can be made.
     */
    Address cursor() const {
      return cursor_;
    }

    /**
     * Returns the first non-free byte past the current cursor position.
     */
    Address limit() const {
      return limit_;
    }

    /**
     * Returns the current line the seach is at.
     * Used for testing.
     */
    uint32_t hole_start_line() const {
      return hole_start_line_;
    }

    /**
     * Returns the block this HoleFinder is searching.
     */
    Block& block() {
      return *block_;
    }

    /**
     * Resets this HoleFinder to the start of the current or specified +block+,
     * and then attempts to locate the first hole (using find_hole).
     *
     * @returns true if a hole is found.
     */
    bool reset(Block* block = 0) {
      if(block) block_ = block;
      hole_start_line_ = 0;
      return find_hole();
    }

    /**
     * Searches from the last search line until it finds a hole or reaches the
     * end of the Block line map.
     *
     * @returns true if a hole is found, in which case the +cursor+ will be
     * positioned ready for the next allocation. Additionally, +limit+ will
     * identify the address one byte beyond the end of the hole.
     */
    bool find_hole() {
      for(; hole_start_line_ < cLineTableSize; hole_start_line_++) {
        if(block_->is_line_free(hole_start_line_)) {
          cursor_ = block_->address_of_line(hole_start_line_);

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

    /**
     * Bump allocates from the current hole.
     *
     * Note: Relies on caller to determine that +size+ is valid, and will fit
     * the current hole.
     */
    Address bump(uint32_t size) {
      Address alloc = cursor_;
      cursor_ += size;
      return alloc;
    }

  };


  /**
   * Pure virtual base class for allocators operating in the immix memory
   * space.
   */

  class Allocator {
  public:
    virtual ~Allocator() {}
    virtual Address allocate(uint32_t bytes) = 0;
  };


  /**
   * An immix Allocator that attempts to handle allocation requests out of
   * a single Block. If there is insufficient space available in the block,
   * the allocation fails.
   * This allocator is used for testing.
   */

  class SingleBlockAllocator : public HoleFinder, public Allocator {
  public:

    SingleBlockAllocator(Block& block) {
      reset(&block);
    }

    /**
     * Attempts to allocate a chunk of memory of the specified size.
     * As this allocator only works with a single block, it will fail
     * if the block is unable to accommodate the request.
     *
     * @returns the Address allocated, or a null address if no space is
     * available.
     */
    Address allocate(uint32_t size) {
      while(cursor_ + size > limit_) {
        if(!find_hole()) {
          return Address::null();
        }
      }

      return bump(size);
    }
  };


  /**
   * An immix Allocator that attempts to handle allocation requests, expanding
   * the number of blocks in use if necessary.
   */

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

    /**
     * Returns the next block containing free space.
     */
    void get_new_block(bool free_only = false) {
      for(;;) {
        /*
         *
         * DISABLED FOR NOW.
        if(free_only) {
          block = &block_allocator_.get_free_block();
        } else {
          block = &block_allocator_.get_block();
        }
        */
        Block* block = &block_allocator_.get_block();

        if(reset(block)) break;
      }
    }

    /*
     * Resets the allocator to the next free hole in the current block, or
     * the next block if the current block is full.
     * Used for testing.
     */
    void resync_position() {
      if(!reset()) get_new_block();
    }

    /**
     * Attempts to allocate space for an object of the specified +size+.
     * If unsuccessful at finding space in the current Block memory, a new
     * Block is obtained from the BlockAllocator.
     */
    Address allocate(uint32_t size) {
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

  /**
   * An object we can pass to the Describer, which can then use it to call
   * back to the immix GC to mark an address. Gets around the problem of
   * the Describer not knowing about the Allocator to pass the immix GC when
   */
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


  /**
   * A garbage collector class that understands the immix memory model, but
   * not the object memory layout. As such, this class depends on the user
   * supplying a Describer object, through which this garbage collector can
   * interact with the object representation.
   */

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

    /**
     * Sets a Block up for evacuation.
     */
    void evacuate_block(Block& block) {
      block.set_status(cEvacuate);
      evacuate_.push_back(&block);
    }

    /**
     * Converts evacuated Blocks back to free Blocks.
     *
     * @todo Does this need to check if an evacuated block is empty - what
     * about pinned objects?
     */
    void sweep_blocks() {
      for(BlockList::const_iterator i = evacuate_.begin();
          i != evacuate_.end();
          ++i) {
        Block* block = *i;
        if(block->status() == cEvacuate) {
          block->set_status(cFree);
        }
      }
#ifdef RBX_GC_DEBUG
      AllBlockIterator iter(block_allocator_.chunks());

      while(Block* block = iter.next()) {
        block->clear_memory();
      }
#endif

      block_allocator_.reset();
    }

    /**
     * Called to mark the object at Address +addr+ as live.
     *
     * Because of the interaction between the ImmixGC, immix:GC, and
     * ObjectDescriber, the process can get a little confusing! It starts
     * with ImmxGC::saw_object, which calls this method to handle the potential
     * movement of the seen object (e.g. if the current location is marked
     * for evacuation). Since this method does not actually know how to do the
     * marking of an object, it calls back to ObjectDescriber to handle this.
     */
    Address mark_address(Address addr, Allocator& alloc, bool push = true) {
      Address fwd = desc.forwarding_pointer(addr);

      if(!fwd.is_null()) {
        addr = fwd;
      }

      // Returns false if addr is already marked, if so, we don't
      // do the block marking logic again.
      if(!desc.mark_address(addr, mark_stack_, push)) {
        return addr;
      }

      // Find the Block the address relates to
      Block* block = Block::from_address(addr);
      if(block->status() == cEvacuate && !desc.pinned(addr)) {
        // Block is marked for evacuation, so copy the object to a new Block
        fwd = desc.copy(addr, alloc);
        desc.set_forwarding_pointer(addr, fwd);

        addr = fwd;
        block = Block::from_address(addr);
      }

      // Mark the line(s) in the Block that this object occupies as in use
      block->mark_address(addr, desc.size(addr));

      return addr;
    }

    /**
     * Calls the Describer to scan from each of the Addresses in the mark stack.
     */
    bool process_mark_stack(Allocator& alloc, int count = 0) {
      Marker<Describer> mark(this, alloc);

      if(mark_stack_.empty()) return false;

      // Use while() since mark_stack_ is modified as we walk it.
      while(!mark_stack_.empty()) {
        Address addr = mark_stack_.back();
        mark_stack_.pop_back();
        desc.walk_pointers(addr, mark);
        count--;
        if(count == 0) return true;
      }

      return true;
    }

    /**
     * Calls the describer to update addresses that might have been
     * forwarded by another GC.
     */
    size_t update_mark_stack(Allocator& alloc) {
      Marker<Describer> mark(this, alloc);

      for(MarkStack::iterator i = mark_stack_.begin(); i != mark_stack_.end(); ++i) {
        Address addr = desc.update_pointer(*i);
        if(!addr.is_null()) {
          addr = mark_address(addr, alloc, false);
        }
        *i = addr;
      }
      return mark_stack_.size();
    }

    /**
     * Notify the garbage collector that we have added a new Chunk.
     */
    void added_chunk(uint32_t count) {
      desc.added_chunk(count);
    }

    /**
     * Notify the garbage collector that we are nearing our last available
     * block.
     */
    void last_block() {
      desc.last_block();
    }

    /**
     * Called at the start of a collection; clears marks before a new cycle.
     * Following a call to this method, the garbage collector will trace
     * from all root objects, and live objects found in managed Blocks will
     * cause lines to be marked as in use.
     */
    void clear_marks() {
      AllBlockIterator iter(block_allocator_.chunks());

      while(Block* block = iter.next()) {
        block->clear_marks();
      }
    }

    /**
     * Called after a collection. Copies the marks to the current lines
     * allocations will be able to use the updated lines list for finding holes
     */
    void copy_marks() {
      AllBlockIterator iter(block_allocator_.chunks());

      while(Block* block = iter.next()) {
        block->copy_marks();
      }
    }

    /**
     * Returns the number of bytes of memory reserved for allocations by this
     * garbage collector.
     */
    size_t& bytes_allocated() {
      return block_allocator_.bytes_allocated();
    }

    /**
     * Returns true if the specified Address +addr+ is contained in any of the
     * Chunks managed by our BlockAllocator.
     */
    bool allocated_address(Address addr) {
      Chunks& chunks = block_allocator_.chunks();
      for(Chunks::iterator i = chunks.begin();
          i != chunks.end();
          ++i) {
        if((*i)->contains_address(addr)) return true;
      }

      return false;
    }
  };
}

#endif

#include "oop.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "gc/inflated_headers.hpp"

#include <iostream>

namespace rubinius {
  /**
   * Allocates a new InflatedHeader object for the specified obj ObjectHeader.
   *
   * /param obj The ObjectHeader that is to be inflated.
   * /returns the InflatedHeader representing the new inflated object header.
   */
  InflatedHeader* InflatedHeaders::allocate(ObjectHeader* obj) {
    bool needs_gc = false;
    InflatedHeader* header = allocator_->allocate(&needs_gc);
    header->set_object(obj);
    if(needs_gc) {
      state_->om->collect_mature_now = true;
    }
    return header;
  }

  /**
   * Scans the list of InflatedHeader objects checking to see which are in use.
   * Those that do not have the appropriate mark value set are cleared and
   * added back to the free list. Chunks that are completely unused are removed
   * from the linked list.
   *
   * /param mark The current value of the mark; only InflatedHeaders that bear
   *             this mark will be retained.
   */
  void InflatedHeaders::deallocate_headers(int mark) {

    std::vector<bool> chunk_marks(allocator_->chunks_.size(), false);
    for(std::vector<int>::size_type i = 0; i < allocator_->chunks_.size(); ++i) {
      InflatedHeader* chunk = allocator_->chunks_[i];

      for(size_t j = 0; j < allocator_->cChunkSize; j++) {
        InflatedHeader* header = &chunk[j];

        if(header->marked_p(mark)) {
          chunk_marks[i] = true;
          break;
        } else {
          header->clear();
        }
      }
    }

    allocator_->rebuild_freelist(&chunk_marks);
  }
}

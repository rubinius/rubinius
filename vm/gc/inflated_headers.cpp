#include "oop.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "gc/inflated_headers.hpp"

#include <iostream>

namespace rubinius {
  InflatedHeaders::~InflatedHeaders() {
    delete allocator_;
  }

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
    // Detect and free any full chunks first!
    for(std::vector<InflatedHeader*>::iterator i = allocator_->chunks_.begin();
        i != allocator_->chunks_.end();) {
      InflatedHeader* chunk = *i;

      bool used = false;

      for(size_t j = 0; j < allocator_->cChunkSize; j++) {
        InflatedHeader* header = &chunk[j];

        if(header->marked_p(mark)) {
          used = true;
          break;
        } else {
          header->clear();
        }
      }

      // No header was marked, so it's completely empty. Free it.
      if(!used) {
        delete[] chunk;
        i = allocator_->chunks_.erase(i);
      } else {
        ++i;
      }
    }

    allocator_->rebuild_freelist();
  }
}

#include "oop.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "gc/inflated_headers.hpp"

#include <iostream>

namespace rubinius {
  InflatedHeaders::~InflatedHeaders() {
    for(Chunks::iterator i = chunks_.begin();
        i != chunks_.end();
        ++i) {
      InflatedHeader* chunk = *i;
      delete[] chunk;
    }
  }

  /**
   * Allocates a new InflatedHeader object for the specified obj ObjectHeader.
   *
   * /param obj The ObjectHeader that is to be inflated.
   * /returns the InflatedHeader representing the new inflated object header.
   */
  InflatedHeader* InflatedHeaders::allocate(ObjectHeader* obj) {
    if(!free_list_) allocate_chunk();
    InflatedHeader* header = free_list_;
    free_list_ = header->next();

    in_use_++;
    header->set_object(obj);

    return header;
  }

  /**
   * Allocates a new chunk of storage for InflatedHeader objects, and then
   * adds each InflatedHeader slot to the free list.
   */
  void InflatedHeaders::allocate_chunk() {
    InflatedHeader* chunk = new InflatedHeader[cChunkSize];
    for(size_t i = 0; i < cChunkSize; i++) {
      chunk[i].clear();
      chunk[i].set_next(free_list_);
      free_list_ = &chunk[i];
    }

    chunks_.push_back(chunk);
    allocations_++;
    if(allocations_ >= cChunkLimit) {
      state_->om->collect_mature_now = true;
      allocations_ = 0;
    }

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
    for(Chunks::iterator i = chunks_.begin();
        i != chunks_.end();) {
      InflatedHeader* chunk = *i;

      bool used = false;

      for(size_t j = 0; j < cChunkSize; j++) {
        InflatedHeader* header = &chunk[j];

        if(header->marked_p(mark)) {
          used = true;
          break;
        }
      }

      // No header was marked, so it's completely empty. Free it.
      if(!used) {
        delete[] chunk;
        i = chunks_.erase(i);
      } else {
        ++i;
      }
    }

    // Ok, now, rebuild the free_list
    free_list_ = 0;
    in_use_ = 0;

    for(Chunks::iterator i = chunks_.begin();
        i != chunks_.end();
        ++i) {
      InflatedHeader* chunk = *i;

      for(size_t j = 0; j < cChunkSize; j++) {
        InflatedHeader* header = &chunk[j];

        if(!header->marked_p(mark)) {
          header->clear();
          header->set_next(free_list_);
          free_list_ = header;
        } else {
          in_use_++;
        }
      }
    }

  }
}

#include "oop.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "gc/inflated_headers.hpp"

#include <iostream>

namespace rubinius {

  InflatedHeader* InflatedHeaders::allocate(ObjectHeader* obj, uint32_t* index) {
    bool needs_gc = false;
    uintptr_t header_index = allocator_->allocate_index(&needs_gc);
    if(header_index > UINT32_MAX) {
      rubinius::bug("Rubinius can't handle more than 4G inflated headers active at the same time");
    }
    if(index) *index = (uint32_t)header_index;
    InflatedHeader* header = allocator_->from_index(header_index);
    header->set_flags(obj->flags());
    if(needs_gc) {
      state_->om->collect_mature_now = true;
    }
    atomic::memory_barrier();
    return header;
  }

  void InflatedHeaders::deallocate_headers(int mark) {
    std::vector<bool> chunk_marks(allocator_->chunks_.size(), false);
    for(std::vector<int>::size_type i = 0; i < allocator_->chunks_.size(); ++i) {
      InflatedHeader* chunk = allocator_->chunks_[i];

      for(size_t j = 0; j < allocator_->cChunkSize; j++) {
        InflatedHeader* header = &chunk[j];

        if(header->in_use_p()) {
          if(header->marked_p(mark)) {
            chunk_marks[i] = true;
          } else {
            header->clear();
          }
        }
      }
    }

    allocator_->rebuild_freelist(&chunk_marks);
  }
}

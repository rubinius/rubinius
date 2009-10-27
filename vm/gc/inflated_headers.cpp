#include "gc/inflated_headers.hpp"
#include "oop.hpp"

#include <iostream>

namespace rubinius {
  InflatedHeaders::~InflatedHeaders() {
    for(Chunks::iterator i = chunks_.begin();
        i != chunks_.end();
        i++) {
      InflatedHeader* chunk = *i;
      delete[] chunk;
    }
  }

  InflatedHeader* InflatedHeaders::allocate(ObjectHeader* obj) {
    if(!free_list_) allocate_chunk();
    InflatedHeader* header = free_list_;
    free_list_ = header->next();

    in_use_++;
    header->set_object(obj);

    return header;
  }

  void InflatedHeaders::allocate_chunk() {
    InflatedHeader* chunk = new InflatedHeader[cChunkSize];
    for(size_t i = 0; i < cChunkSize; i++) {
      chunk[i].clear();
      chunk[i].set_next(free_list_);
      free_list_ = &chunk[i];
    }

    chunks_.push_back(chunk);
  }

  void InflatedHeaders::deallocate_headers() {
    for(Chunks::iterator i = chunks_.begin();
        i != chunks_.end();
        i++) {
      InflatedHeader* chunk = *i;

      for(size_t j = 0; j < cChunkSize; j++) {
        InflatedHeader* header = &chunk[j];

        if(header->used_p() && !header->marked_p()) {
          in_use_--;
          header->clear();
          header->set_next(free_list_);
          free_list_ = header;
        }
      }
    }

    for(Chunks::iterator i = chunks_.begin();
        i != chunks_.end();
        i++) {
      InflatedHeader* chunk = *i;

      bool used = false;

      for(size_t j = 0; j < cChunkSize; j++) {
        InflatedHeader* header = &chunk[j];

        if(header->used_p()) {
          used = true;
          break;
        }
      }

      if(!used) {
        delete[] chunk;
        i = chunks_.erase(i);
      }
    }
  }
}

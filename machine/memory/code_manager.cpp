#include <string.h>

#include "vm.hpp"
#include "state.hpp"
#include "memory/code_manager.hpp"
#include "memory/code_resource.hpp"

namespace rubinius {
namespace memory {
  void CodeManager::Diagnostics::update() {
  }

  CodeManager::Chunk::Chunk(int size)
    : next(0)
  {
    resources = new CodeResource*[size];
    memset(resources, 0, sizeof(CodeResource*) * size);
  }

  CodeManager::Chunk::~Chunk() {
    delete[] resources;
  }

  CodeManager::CodeManager(SharedState* shared, int chunk_size)
    : shared_(shared)
    , chunk_size_(chunk_size)
    , first_chunk_(0)
    , last_chunk_(0)
    , current_chunk_(0)
    , current_index_(0)
    , diagnostics_(new Diagnostics())
  {
    first_chunk_ = new Chunk(chunk_size_);
    last_chunk_ = first_chunk_;
    current_chunk_ = first_chunk_;
  }

  CodeManager::~CodeManager() {
    Chunk* chunk = first_chunk_;

    while(chunk) {
      for(int i = 0; i < chunk_size_; i++) {
        if(CodeResource* cr = chunk->resources[i]) {
          delete cr;
          chunk->resources[i] = 0;
        }
      }

      Chunk* next = chunk->next;
      delete chunk;
      chunk = next;
    }
  }

  void CodeManager::add_chunk() {
    Chunk* c = new Chunk(chunk_size_);
    current_index_ = 0;
    last_chunk_->next = c;
    last_chunk_ = c;
    current_chunk_ = c;
  }

  void CodeManager::add_resource(CodeResource* cr, bool* collect_now) {
    utilities::thread::Mutex::LockGuard guard(mutex_);

    for(;;) {
      while(current_index_ < chunk_size_) {
        if(current_chunk_->resources[current_index_] == 0) {
          current_chunk_->resources[current_index_] = cr;
          return;
        }

        current_index_++;
      }

      // Move on to the next one or add a new one
      current_index_ = 0;
      current_chunk_ = current_chunk_->next;
      if(!current_chunk_) {
        add_chunk();
        *collect_now = true;
        diagnostics()->collections_++;
      }
    }
  }

  void CodeManager::sweep() {
    Chunk* chunk = first_chunk_;
    Chunk* prev  = NULL;

    State state(shared_->root_vm());

    while(chunk) {
      bool chunk_used = false;
      for(int i = 0; i < chunk_size_; i++) {
        if(CodeResource* cr = chunk->resources[i]) {
          if(!cr->marked()) {
            cr->cleanup(&state, this);
            delete cr;
            chunk->resources[i] = 0;
          } else {
            chunk_used = true;
            cr->clear_mark();

            diagnostics()->objects_++;
            diagnostics()->bytes_ += cr->size();
          }
        }
      }

      // Cleanup unused chunks. We can never cleanup the first
      // chunk in the list, but that will be always used anyway
      // with references to boot up code etc.
      if(!chunk_used && prev) {
        prev->next = chunk->next;
        // If we clean up the last chunk, set the last chunk
        // to the previous one we've seen.
        if(last_chunk_ == chunk) {
          last_chunk_ = prev;
        }
        // If we clean up the current chunk, set the current
        // to the previous one. The code will scan the chunk then
        // and might allocate a new one of the previous is
        // already completely full.
        if(current_chunk_ == chunk) {
          current_chunk_ = prev;
        }
        delete chunk;
        chunk = prev->next;
      } else {
        prev = chunk;
        chunk = chunk->next;
        diagnostics()->chunks_++;
      }
    }
  }

  void CodeManager::clear_marks() {
    Chunk* chunk = first_chunk_;

    while(chunk) {
      for(int i = 0; i < chunk_size_; i++) {
        if(CodeResource* cr = chunk->resources[i]) {
          cr->clear_mark();
        }
      }

      chunk = chunk->next;
    }
  }
}
}

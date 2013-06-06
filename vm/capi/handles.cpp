#include "builtin/nativemethod.hpp"
#include "objectmemory.hpp"
#include "gc/baker.hpp"
#include "capi/capi.hpp"
#include "capi/handles.hpp"

namespace rubinius {
  namespace capi {

    Handle* Handles::allocate(STATE, Object* obj) {
      bool needs_gc = false;
      Handle* handle = allocator_->allocate(&needs_gc);
      handle->set_object(obj);
      handle->validate();
      if(needs_gc) {
        state->memory()->collect_mature_now = true;
      }
      atomic::memory_barrier();
      return handle;
    }

    uintptr_t Handles::allocate_index(STATE, Object* obj) {
      bool needs_gc = false;
      uintptr_t handle_index = allocator_->allocate_index(&needs_gc);
      if(handle_index > UINT32_MAX) {
        rubinius::bug("Rubinius can't handle more than 4G C-API handles active at the same time");
      }
      Handle* handle = allocator_->from_index(handle_index);
      handle->set_object(obj);
      handle->validate();
      if(needs_gc) {
        state->memory()->collect_mature_now = true;
      }
      atomic::memory_barrier();

      if(handle_index > UINT32_MAX) {
        rubinius::bug("Rubinius can't handle more than 4G C-API handles active at the same time");
      }

      return handle_index;
    }

    bool Handles::validate(Handle* handle) {
      return allocator_->validate(handle);
    }

    Handles::~Handles() {
      for(std::vector<int>::size_type i = 0; i < allocator_->chunks_.size(); ++i) {
        Handle* chunk = allocator_->chunks_[i];

        for(size_t j = 0; j < allocator_->cChunkSize; j++) {
          Handle* handle = &chunk[j];
          if(handle->in_use_p()) {
            handle->clear();
          }
        }
      }
      delete allocator_;
    }

    void Handles::deallocate_handles(std::list<Handle*>* cached, unsigned int mark, BakerGC* young) {
      std::vector<bool> chunk_marks(allocator_->chunks_.size(), false);

      for(std::vector<int>::size_type i = 0; i < allocator_->chunks_.size(); ++i) {
        Handle* chunk = allocator_->chunks_[i];

        for(size_t j = 0; j < allocator_->cChunkSize; j++) {
          Handle* handle = &chunk[j];

          Object* obj = handle->object();

          if(!handle->in_use_p()) {
            continue;
          }

          // Strong references will already have been updated.
          if(!handle->weak_p()) {
            chunk_marks[i] = true;
            continue;
          }

          if(young) {
            if(obj->young_object_p()) {
              // A weakref pointing to a valid young object
              //
              // TODO this only works because we run prune_handles right after
              // a collection. In this state, valid objects are only in current.
              if(young->in_current_p(obj)) {
                chunk_marks[i] = true;
              // A weakref pointing to a forwarded young object
              } else if(obj->forwarded_p()) {
                handle->set_object(obj->forward());
                chunk_marks[i] = true;
              // A weakref pointing to a dead young object
              } else {
                handle->clear();
              }
            } else {
              // Not a young object, so won't be GC'd so mark
              // chunk as still active
              chunk_marks[i] = true;
            }

          // A weakref pointing to a dead mature object
          } else if(!obj->marked_p(mark)) {
            handle->clear();
          } else {
            chunk_marks[i] = true;
          }
        }
      }

      // Cleanup cached handles
      for(std::list<Handle*>::iterator it = cached->begin(); it != cached->end();) {
        Handle* handle = *it;

        if(handle->in_use_p()) {
          ++it;
        } else {
          it = cached->erase(it);
        }
      }

      allocator_->rebuild_freelist(&chunk_marks);
    }
  }
}

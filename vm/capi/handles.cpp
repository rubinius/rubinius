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
      return handle;
    }

    void Handles::deallocate_handles(std::list<Handle*>* cached, int mark, BakerGC* young) {

      std::vector<bool> chunk_marks(allocator_->chunks_.size(), false);

      size_t i = 0;

      for(std::vector<Handle*>::iterator it = allocator_->chunks_.begin();
          it != allocator_->chunks_.end(); ++it) {
        Handle* chunk = *it;

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
        ++i;
      }

      // Cleanup cached handles
      for(std::list<Handle*>::iterator i = cached->begin(); i != cached->end();) {
        Handle* handle = *i;
        if(handle->in_use_p()) {
          ++i;
        } else {
          i = cached->erase(i);
        }
      }

      i = 0;
      for(std::vector<Handle*>::iterator it = allocator_->chunks_.begin();
          it != allocator_->chunks_.end();) {
        // No header was marked, so it's completely empty. Free it.
        if(!chunk_marks[i]) {
          Handle* chunk = *it;
          delete[] chunk;
          it = allocator_->chunks_.erase(it);
        } else {
          ++it;
        }
        ++i;
      }

      allocator_->rebuild_freelist();
    }
  }
}

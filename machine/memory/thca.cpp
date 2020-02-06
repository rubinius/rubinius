#include "memory.hpp"

#include "memory/immix.hpp"
#include "memory/thca.hpp"

#include <mutex>

namespace rubinius {
  namespace memory {
    size_t OpenTHCA::region_size = 16384;

    Object* THCA::allocate(STATE, intptr_t bytes, object_type type) {
      return state->memory()->main_heap()->allocate(state, bytes, type);
    }

    bool OpenTHCA::allocate_region(STATE) {
      Object* region = nullptr;

      state->set_checkpoint();

      {
        std::lock_guard<locks::spinlock_mutex> guard(state->memory()->allocation_lock());

        region = state->memory()->main_heap()->first_region()->allocate_region(
            state, region_size, &available_);
      }

      if(region) {
        address_ = reinterpret_cast<uintptr_t>(region);
        ++regions_;

        return true;
      }

      return false;
    }

    Object* OpenTHCA::allocate_object(STATE, intptr_t bytes, object_type type) {
      if(available_ >= bytes) {
        Object* obj = reinterpret_cast<Object*>(address_);

        MemoryHeader::initialize(obj, state->thread_id(), eFirstRegion, type, false);

        available_ -= bytes;
        address_ += bytes;

        ++objects_;

        return obj;
      }

      return nullptr;
    }

    Object* OpenTHCA::allocate(STATE, intptr_t bytes, object_type type) {
      bytes = MemoryHeader::align(bytes);

      if(bytes <= cMediumObjectLimit) {
        if(Object* obj = allocate_object(state, bytes, type)) {
          return obj;
        } else if(allocate_region(state)) {
          if(Object* obj = allocate_object(state, bytes, type)) return obj;
        }
      }

      return THCA::allocate(state, bytes, type);
    }

    Object* ClosedTHCA::allocate(STATE, intptr_t bytes, object_type type) {
      return state->memory()->main_heap()->allocate(state, bytes, type);
    }

    Object* IsolatedTHCA::allocate(STATE, intptr_t bytes, object_type type) {
      return state->memory()->main_heap()->allocate(state, bytes, type);
    }
  }
}

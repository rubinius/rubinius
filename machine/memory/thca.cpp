#include "memory.hpp"

#include "memory/immix.hpp"
#include "memory/thca.hpp"

namespace rubinius {
  namespace memory {
    size_t OpenTHCA::region_size = 16384;

    Object* THCA::allocate(STATE, native_int bytes, object_type type) {
      return state->memory()->main_heap()->allocate(state, bytes, type);
    }

    bool OpenTHCA::allocate_region(STATE) {
      Object* region = nullptr;

      state->vm()->set_checkpoint();

      {
        utilities::thread::SpinLock::LockGuard guard(state->memory()->allocation_lock());

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

    Object* OpenTHCA::allocate_object(STATE, native_int bytes, object_type type) {
      if(available_ >= bytes) {
        Object* obj = reinterpret_cast<Object*>(address_);

        MemoryHeader::initialize(obj, state->vm()->thread_id(), eFirstRegion, type, false);

        available_ -= bytes;
        address_ += bytes;

        ++objects_;

        return obj;
      }

      return nullptr;
    }

    Object* OpenTHCA::allocate(STATE, native_int bytes, object_type type) {
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

    Object* ClosedTHCA::allocate(STATE, native_int bytes, object_type type) {
      return state->memory()->main_heap()->allocate(state, bytes, type);
    }

    Object* IsolatedTHCA::allocate(STATE, native_int bytes, object_type type) {
      return state->memory()->main_heap()->allocate(state, bytes, type);
    }
  }
}

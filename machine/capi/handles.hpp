#ifndef RBX_CAPI_HANDLES_HPP
#define RBX_CAPI_HANDLES_HPP

#include "vm.hpp"
#include "state.hpp"

#include "memory/root.hpp"
#include "memory/allocator.hpp"

#include "capi/handle.hpp"

#include "diagnostics.hpp"

#include <vector>
#include <stdint.h>

namespace rubinius {
  namespace capi {
    class Handles {
      memory::Allocator<Handle>* allocator_;

      diagnostics::Handles* diagnostic_;

    public:

      Handles();
      ~Handles();

      Handle* allocate(STATE, Object* obj);

      uintptr_t allocate_index(STATE, Object* obj);

      Handle* find_index(uintptr_t index) {
        return allocator_->from_index(index);
      }

      bool validate(Handle* handle);

      void deallocate_handles(std::list<Handle*>* cached, unsigned int mark, /* BakerGC */ void* young);

      void flush_all(NativeMethodEnvironment* env);

      memory::Allocator<Handle>* allocator() const {
        return allocator_;
      }

      int size() const {
        return allocator_->in_use_;
      }

      diagnostics::Handles* diagnostic() {
        return diagnostic_;
      }
    };
  }
}

#endif

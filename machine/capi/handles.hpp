#ifndef RBX_CAPI_HANDLES_HPP
#define RBX_CAPI_HANDLES_HPP

#include "diagnostics.hpp"
#include "vm.hpp"
#include "state.hpp"
#include "memory/root.hpp"
#include "memory/allocator.hpp"
#include "capi/handle.hpp"

#include <vector>
#include <stdint.h>

namespace rubinius {
  namespace capi {
    class Handles {
    public:
      class Diagnostics : public diagnostics::MemoryDiagnostics {
      public:
        int64_t collections_;

        Diagnostics()
          : diagnostics::MemoryDiagnostics()
          , collections_(0)
        { }

        void update();
      };

    private:
      memory::Allocator<Handle>* allocator_;

      Diagnostics* diagnostics_;

    public:

      Handles()
        : allocator_(new memory::Allocator<Handle>())
        , diagnostics_(new Diagnostics())
      {}

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

      Diagnostics* diagnostics() {
        return diagnostics_;
      }
    };
  }
}

#endif

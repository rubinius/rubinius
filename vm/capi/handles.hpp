#ifndef RBX_CAPI_HANDLES_HPP
#define RBX_CAPI_HANDLES_HPP

#include "diagnostics.hpp"
#include "vm.hpp"
#include "state.hpp"
#include "gc/root.hpp"
#include "util/allocator.hpp"
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

        void log();
      };

    private:
      Allocator<Handle>* allocator_;

      Diagnostics diagnostics_;

    public:

      Handles()
        : allocator_(new Allocator<Handle>())
        , diagnostics_(Diagnostics())
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

      Allocator<Handle>* allocator() const {
        return allocator_;
      }

      int size() const {
        return allocator_->in_use_;
      }

      Diagnostics& diagnostics() {
        return diagnostics_;
      }
    };
  }
}

#endif

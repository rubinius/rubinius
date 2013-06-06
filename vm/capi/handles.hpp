#ifndef RBX_CAPI_HANDLES_HPP
#define RBX_CAPI_HANDLES_HPP

#include "vm.hpp"
#include "gc/root.hpp"
#include "util/allocator.hpp"
#include "capi/handle.hpp"

#include <vector>

namespace rubinius {
  class BakerGC;

  namespace capi {

    class Handles {

    private:
      Allocator<Handle>* allocator_;

    public:

      Handles()
        : allocator_(new Allocator<Handle>())
      {}

      ~Handles();

      Handle* allocate(STATE, Object* obj);

      uintptr_t allocate_index(STATE, Object* obj);

      Handle* find_index(uintptr_t index) {
        return allocator_->from_index(index);
      }

      bool validate(Handle* handle);

      void deallocate_handles(std::list<Handle*>* cached, unsigned int mark, BakerGC* young);

      void flush_all(NativeMethodEnvironment* env);

      Allocator<Handle>* allocator() const {
        return allocator_;
      }

      int size() const {
        return allocator_->in_use_;
      }

    };
  }
}

#endif

#ifndef RBX_MEMORY_MAIN_HEAP_HPP
#define RBX_MEMORY_MAIN_HEAP_HPP

#include "object_types.hpp"

#include "memory/code_manager.hpp"
#include "memory/heap.hpp"

namespace rubinius {
  class Configuration;

  namespace memory {
    class Immix;
    class LargeRegion;

    class MainHeap : public Heap {
      Immix* first_region_;
      LargeRegion* third_region_;
      CodeManager& code_manager_;

    public:
      MainHeap(Configuration* configuration, CodeManager& cm);
      virtual ~MainHeap();

      Immix* first_region() {
        return first_region_;
      }

      LargeRegion* third_region() {
        return third_region_;
      }

      Object* allocate(STATE, intptr_t bytes, object_type type);

      void collect_start(STATE);
      void collect_references(STATE, std::function<void (STATE, Object**)> f);
      void collect_finish(STATE);
    };
  }
}

#endif

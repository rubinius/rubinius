#ifndef RBX_MEMORY_MAIN_HEAP_HPP
#define RBX_MEMORY_MAIN_HEAP_HPP

#include "memory/code_manager.hpp"
#include "memory/heap.hpp"

namespace rubinius {
  namespace memory {
    class Immix;
    class LargeRegion;

    class MainHeap : public Heap {
      Immix* first_region_;
      LargeRegion* third_region_;
      CodeManager& code_manager_;

    public:
      MainHeap(STATE, CodeManager& cm);
      virtual ~MainHeap();

      Immix* first_region() {
        return first_region_;
      }

      LargeRegion* third_region() {
        return third_region_;
      }

      void collect_start(STATE);
      void collect_references(STATE, std::function<void (STATE, Object**)> f);
      void collect_finish(STATE);
    };
  }
}

#endif

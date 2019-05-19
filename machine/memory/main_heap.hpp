#ifndef RBX_MEMORY_MAIN_HEAP_HPP
#define RBX_MEMORY_MAIN_HEAP_HPP

#include "memory/code_manager.hpp"
#include "memory/heap.hpp"

namespace rubinius {
  namespace memory {
    class ImmixGC;
    class MarkSweepGC;

    class MainHeap : public Heap {
      ImmixGC* immix_;
      MarkSweepGC* mark_sweep_;
      CodeManager& code_manager_;

    public:
      MainHeap(STATE, ImmixGC* immix, MarkSweepGC* ms, CodeManager& cm)
        : Heap()
        , immix_(immix)
        , mark_sweep_(ms)
        , code_manager_(cm)
      {
      }
      virtual ~MainHeap() { }

      void collect_start(STATE, GCData* data);
      void collect_references(STATE, std::function<Object* (STATE, Object*)> f);
      void collect_finish(STATE, GCData* data);
    };
  }
}

#endif

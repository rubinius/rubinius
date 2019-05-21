#ifndef RBX_MEMORY_HEAP_HPP
#define RBX_MEMORY_HEAP_HPP

namespace rubinius {
  namespace memory {
    class GCData;

    class Heap {
    public:
      Heap() { }
      virtual ~Heap() { }

      virtual void collect_start(STATE, GCData* data) = 0;
      virtual void collect_references(STATE, std::function<Object* (STATE, void*, Object*)> f) = 0;
      virtual void collect_finish(STATE, GCData* data) = 0;
    };
  }
}

#endif

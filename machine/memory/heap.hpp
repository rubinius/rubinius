#ifndef RBX_MEMORY_HEAP_HPP
#define RBX_MEMORY_HEAP_HPP

namespace rubinius {
  namespace memory {
    class Heap {
    public:
      Heap() { }
      virtual ~Heap() { }

      virtual void collect_start(STATE) = 0;
      virtual void collect_references(STATE, std::function<void (STATE, Object**)> f) = 0;
      virtual void collect_finish(STATE) = 0;
    };
  }
}

#endif

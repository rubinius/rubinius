#ifndef RBX_GC_ROOT_BUFFER_HPP
#define RBX_GC_ROOT_BUFFER_HPP

#include "linkedlist.hpp"

namespace rubinius {
  class RootBuffer;
  class Object;

  class RootBuffers : public LinkedList {
  public:
    RootBuffers()
      : LinkedList()
    {}

    RootBuffer* front() {
      return reinterpret_cast<RootBuffer*>(head());
    }

    typedef LinkedList::Iterator<RootBuffers, RootBuffer> Iterator;
  };

  class RootBuffer : public LinkedList::Node {
    Object** buffer_;
    RootBuffers& roots_;
    int size_;

  public:
    RootBuffer(RootBuffers& roots, Object** buffer, int size)
      : buffer_(buffer)
      , roots_(roots)
      , size_(size)
    {
      roots.add(this);
    }

    ~RootBuffer() {
      roots_.remove(this);
    }

    int size() {
      return size_;
    }

    Object** buffer() {
      return buffer_;
    }

  };
}

#endif

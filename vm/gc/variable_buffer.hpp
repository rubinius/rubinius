#ifndef RBX_GC_VARIABLE_BUFFER_HPP
#define RBX_GC_VARIABLE_BUFFER_HPP

#include "linkedlist.hpp"

namespace rubinius {
  class VariableRootBuffer;
  class Object;

  class VariableRootBuffers : public LinkedList {
  public:
    VariableRootBuffers()
      : LinkedList()
    {}

    VariableRootBuffer* front() {
      return reinterpret_cast<VariableRootBuffer*>(head());
    }

    typedef LinkedList::Iterator<VariableRootBuffers, VariableRootBuffer> Iterator;
  };

  class VariableRootBuffer : public LinkedList::Node {
    Object*** buffer_;
    VariableRootBuffers& roots_;
    int size_;

  public:
    VariableRootBuffer(VariableRootBuffers& roots, Object*** buffer, int size)
      : buffer_(buffer)
      , roots_(roots)
      , size_(size)
    {
      roots.add(this);
    }

    ~VariableRootBuffer() {
      roots_.remove(this);
    }

    int size() {
      return size_;
    }

    Object*** buffer() {
      return buffer_;
    }

  };
}

#endif

#ifndef RBX_MEMORY_THCA_HPP
#define RBX_MEMORY_THCA_HPP

#include "object_types.hpp"

namespace rubinius {
  class Object;

  namespace memory {
    /* Thread-local Heap Container & Allocator
     *
     * The THCA instances are a mechanisms for the three heap types associated
     * with a thread to provide an interface to allocating managed memory
     * objects.
     */
    class THCA {
    public:
      THCA() { }
      virtual ~THCA() { }

      virtual Object* allocate(STATE, native_int bytes, object_type type) = 0;
    };

    class OpenTHCA : public THCA {
    public:
      OpenTHCA()
        : THCA()
      { }
      virtual ~OpenTHCA() { }

      virtual Object* allocate(STATE, native_int bytes, object_type type);
    };

    class ClosedTHCA : public THCA {
    public:
      ClosedTHCA()
        : THCA()
      { }
      virtual ~ClosedTHCA() { }

      virtual Object* allocate(STATE, native_int bytes, object_type type);
    };

    class IsolatedTHCA : public THCA {
    public:
      IsolatedTHCA()
        : THCA()
      { }
      virtual ~IsolatedTHCA() { }

      virtual Object* allocate(STATE, native_int bytes, object_type type);
    };
  }
}

#endif

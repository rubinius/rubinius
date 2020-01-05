#ifndef RBX_MEMORY_THCA_HPP
#define RBX_MEMORY_THCA_HPP

#include "object_types.hpp"

#include "logger.hpp"

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
    protected:
      uintptr_t address_;
      size_t available_;
      uint64_t objects_;
      uint64_t regions_;

    public:
      THCA()
        : address_(0)
        , available_(0)
        , objects_(0)
        , regions_(0)
      { }

      virtual ~THCA() {
        logger::info("THCA allocated: regions: %ld, objects: %ld", regions_, objects_);
      }

      virtual Object* allocate(STATE, intptr_t bytes, object_type type);
      virtual void collect(STATE) {
        address_ = 0;
        available_ = 0;
      }
    };

    class OpenTHCA : public THCA {
      static size_t region_size;

    public:
      OpenTHCA()
        : THCA()
      { }
      virtual ~OpenTHCA() { }

      virtual Object* allocate(STATE, intptr_t bytes, object_type type);
      Object* allocate_object(STATE, intptr_t bytes, object_type type);
      bool allocate_region(STATE);
    };

    class ClosedTHCA : public THCA {
    public:
      ClosedTHCA()
        : THCA()
      { }
      virtual ~ClosedTHCA() { }

      virtual Object* allocate(STATE, intptr_t bytes, object_type type);
    };

    class IsolatedTHCA : public THCA {
    public:
      IsolatedTHCA()
        : THCA()
      { }
      virtual ~IsolatedTHCA() { }

      virtual Object* allocate(STATE, intptr_t bytes, object_type type);
    };
  }
}

#endif

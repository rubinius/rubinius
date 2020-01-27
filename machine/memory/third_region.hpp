#ifndef RBX_MEMORY_LARGE_REGION_HPP
#define RBX_MEMORY_LARGE_REGION_HPP

#include "memory/root.hpp"

#include "diagnostics.hpp"

#include <stdint.h>
#include <list>

namespace rubinius {
  class Configuration;
  class Object;
  class Memory;
  struct CallFrame;

  namespace memory {
    class LargeRegion {
    public:
    private:
        diagnostics::LargeRegion* diagnostic_;

    public:
      /* Data members */
      std::list<Object*> entries;
      int    collection_threshold;
      int    next_collection_bytes;

      /* Prototypes */

      LargeRegion(Configuration* configuration);
      virtual ~LargeRegion();
      void   free_objects();
      Object* allocate(STATE, size_t bytes);
      Object* move_object(Object* orig, size_t bytes, bool& collect_now);
      Object* copy_object(Object* obj, bool& collect_now);
      void   sweep(STATE);
      void   free_object(Object* obj, bool fast = false);

      void profile(STATE);

      // TODO: GC
      // ObjectPosition validate_object(Object* obj);

      diagnostics::LargeRegion* diagnostic() {
        return diagnostic_;
      }
    };
  }
}

#endif

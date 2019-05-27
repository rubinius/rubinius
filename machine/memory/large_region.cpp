#include "memory.hpp"
#include "object_utils.hpp"
#include "shared_state.hpp"
#include "state.hpp"

#include "memory/collector.hpp"
#include "memory/large_region.hpp"

#include "class/tuple.hpp"
#include "class/string.hpp"
#include "class/class.hpp"
#include "class/symbol.hpp"

#include "configuration.hpp"

#include "diagnostics/collector.hpp"
#include "diagnostics/memory.hpp"
#include "diagnostics/timing.hpp"

#include "logger.hpp"

#include <iostream>
#include <algorithm>

namespace rubinius {
  namespace memory {

    LargeRegion::LargeRegion(STATE)
      : diagnostic_(new diagnostics::LargeRegion())
      , collection_threshold(state->shared().config.gc_marksweep_threshold)
      , next_collection_bytes(collection_threshold)
    {}

    LargeRegion::~LargeRegion() {
      free_objects();

      delete diagnostic_;
      diagnostic_ = nullptr;
    }

    void LargeRegion::free_objects() {
      std::list<Object*>::iterator i;

      for(i = entries.begin(); i != entries.end(); ++i) {
        free_object(*i, true);
      }
    }

    Object* LargeRegion::allocate(STATE, size_t bytes) {
      void* mem = malloc(bytes);
      if(!mem) return NULL;

      Object* obj = reinterpret_cast<Object*>(mem);

      diagnostic()->objects++;
      diagnostic()->bytes += bytes;

      state->memory()->shared().memory_metrics()->large_objects++;
      state->memory()->shared().memory_metrics()->large_bytes += bytes;

      entries.push_back(obj);

      next_collection_bytes -= bytes;
      if(next_collection_bytes < 0) {
        next_collection_bytes = collection_threshold;

        state->memory()->shared().collector_metrics()->large_set++;
        state->collector()->collect_requested(state,
            "collector: large object space triggered collection request");
      }

      return obj;
    }

    void LargeRegion::free_object(Object* obj, bool fast) {
      free(reinterpret_cast<void*>(obj));
    }

    Object* LargeRegion::move_object(Object* orig, size_t bytes,
                                     bool& collect_now)
    {
      /* TODO: GC
      Object* obj = allocate(bytes, collect_now);
      memcpy(obj, orig, bytes);

      orig->set_forwarded(obj);

      return obj;
      */
      return orig;
    }

    void LargeRegion::sweep(STATE) {
      std::list<Object*>::iterator i;

      for(i = entries.begin(); i != entries.end();) {
        Object* obj = *i;
        if(obj->marked_p(state->memory()->mark())) {
          ++i;
        } else {
          free_object(obj);
          i = entries.erase(i);
        }
      }
    }

    /* TODO: GC
    ObjectPosition LargeRegion::validate_object(Object* obj) {
      std::list<Object*>::iterator i;

      for(i = entries.begin(); i != entries.end(); ++i) {
        if(*i == obj) return cMatureObject;
      }

      return cUnknown;
    }
    */
  }
}

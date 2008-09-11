#ifndef RBX_CONTEXT_CACHE_HPP
#define RBX_CONTEXT_CACHE_HPP

#include "prelude.hpp"

/* Implements a simple context cache based on having context of a couple
 * sizes only. This implementation also support 'huge' contexts by
 * always just heap allocating them. */

#define CountContextCaches 2

/* These numbers were taken from Squeak */
#define SmallContextSize   16
#define SmallContextCache  0
#define LargeContextSize   56
#define LargeContextCache  1


namespace rubinius {
  class MethodContext;

  class ContextCache {
  public:

    /* A stack depth, indicates how many more contexts we can recycle.
     * This is reset to 0 if it's determined that context recycle needs
     * to be disabled for all current contexts. */
    int reclaim;

    /* A simple array of MethodContext's. The contexts are treated like
     * linked lists, chaining to the next context off their home field.
     * This lets us store any number of contexts quickly in a couple
     * of different caches. Each cache contains context of all the same
     * size. */
    MethodContext* caches[CountContextCaches];


    void reset();
    MethodContext* get(size_t which);
    void add(STATE, size_t which, MethodContext* ctx);

  };
}

#endif

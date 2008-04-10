#ifndef RBX_PRELUDE_H
#define RBX_PRELUDE_H
#define FALSE 0
#define TRUE 1

#include <stdint.h>
#include <vector>

#define STATE rubinius::VM *state
#define G(whatever) state->globals.whatever

namespace rubinius {
  typedef intptr_t native_int;

  /* Forward declaration so it can be used in prototypes */
  class VM;

  class Object;
  typedef Object* OBJECT;
  typedef std::vector<OBJECT> ObjectArray;

  typedef void (*cleanup_function)(STATE, OBJECT);

};


#endif

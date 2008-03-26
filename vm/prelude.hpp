#ifndef RBX_PRELUDE_H
#define RBX_PRELUDE_H
#define FALSE 0
#define TRUE 1

#include <stdint.h>
#include <vector>

namespace rubinius {
  typedef intptr_t native_int;

  /* Forward declaration so it can be used in prototypes */
  class VM;

  class Object;
  typedef Object* OBJECT;
  typedef std::vector<OBJECT> ObjectArray;
};

#define STATE rubinius::VM *state

#endif

#ifndef RBX_JIT_STATE_H
#define RBX_JIT_STATE_H

/* This structure is used by the interpreter to hold context
 * information. It may go away soon, being folded into MethodContext */

namespace rubinius {
  class Object;
}

struct jit_state {
  rubinius::Object** stack;
  rubinius::Object** stack_top;
};

#endif

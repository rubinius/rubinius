#ifndef RBX_JIT_STATE_H
#define RBX_JIT_STATE_H

struct jit_state {
  rubinius::Object** stack;
};

#endif

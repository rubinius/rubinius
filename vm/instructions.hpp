#ifndef RBX_INSTR
#define RBX_INSTR

#include "vm/vmmethod.hpp"

namespace rubinius {
  namespace instructions {
    struct Implementation {
      void* address;
      const char* name;
    };

    const Implementation* implementation(int op);

    enum Status {
      Unchanged,
      MightReturn,
      Terminate
    };

    Status check_status(int op);
    int find_superop(opcode* stream);
    int reverse_superop(opcode superop);
  }
}

#endif

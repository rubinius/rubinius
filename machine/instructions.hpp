#ifndef RBX_INSTR
#define RBX_INSTR

#include "machine_code.hpp"

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

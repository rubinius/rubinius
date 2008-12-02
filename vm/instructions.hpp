#ifndef RBX_INSTR
#define RBX_INSTR

namespace rubinius {
  namespace instructions {
    void* implementation(int op);

    enum Status {
      Unchanged,
      MightReturn,
      Terminate
    };

    Status check_status(int op);
  }
}

#endif

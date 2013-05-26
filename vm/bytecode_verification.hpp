#ifndef RBX_BYTECODE_VERIFICATION
#define RBX_BYTECODE_VERIFICATION

#include <stdint.h>
#include <list>

namespace rubinius {
  class CompiledCode;
  class State;
  class VM;
  class Tuple;

  class BytecodeVerification {
    CompiledCode* method_;

    Tuple* ops_;
    native_int total_;

    int max_stack_allowed_;
    int max_stack_seen_;
    int32_t* stack_;

    native_int locals_;
    int max_stack_local_;

    const char* fail_reason_;
    int fail_ip_;

    void fail(const char* reason, int ip) {
      fail_reason_ = reason;
      fail_ip_ = ip;
    }

    struct Section {
      int sp;
      int ip;

      Section(int sp, int ip)
        : sp(sp), ip(ip)
      {}
    };

  public:
    BytecodeVerification(CompiledCode* code);
    ~BytecodeVerification();

    bool verify(STATE);
    bool verify_from(STATE, int sp, int ip, std::list<Section>& ips);

    const char* failure_reason() const {
      return fail_reason_;
    }
    int failure_ip() const {
      return fail_ip_;
    }
  };
}
#endif

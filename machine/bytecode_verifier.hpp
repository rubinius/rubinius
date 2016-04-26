#ifndef RBX_BYTECODE_VERIFICATION
#define RBX_BYTECODE_VERIFICATION

#include <stdint.h>
#include <list>

namespace rubinius {
  class CompiledCode;
  class Fixnum;
  class Object;
  class Tuple;
  class State;
  class VM;

  class BytecodeVerifier {
    CompiledCode* method_;

    Tuple* ops_;
    native_int total_;

    int max_stack_allowed_;
    int max_stack_seen_;
    int32_t* stack_;

    native_int locals_;
    int max_stack_local_;

    struct Section {
      int sp;
      int ip;

      Section(int sp, int ip)
        : sp(sp), ip(ip)
      {}
    };

  public:
    BytecodeVerifier(CompiledCode* code);
    ~BytecodeVerifier();

    void verify(STATE);
    void verify_from(STATE, int sp, int ip, std::list<Section>& ips);

    void verify_width(STATE, int ip);
    Fixnum* verify_opcode(STATE, int ip);
    Fixnum* verify_argument(STATE, int ip);
    Object* verify_object(STATE, int index, int ip);
    Symbol* verify_symbol(STATE, int index, int ip);
    Object* verify_symbol_or_nil(STATE, int index, int ip);
    Object* verify_code(STATE, int index, int ip);

    NORETURN(void fail(STATE, const char* reason, int ip));
  };
}
#endif

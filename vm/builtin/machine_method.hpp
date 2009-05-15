#ifndef RBX_BUILTIN_MACHINE_METHOD
#define RBX_BUILTIN_MACHINE_METHOD

#include "assembler/code_map.hpp"
#include "assembler/relocation.hpp"

namespace rubinius {
  class JITCompiler;
  class VMMethod;

  class MachineMethod : public Object {
  public:
    const static object_type type = MachineMethodType;

  private:
    VMMethod* vmmethod_;
    size_t code_size_;
    CodeMap* virtual2native_;
    AddressComments* comments_;
    assembler::Relocation** relocations_;

    void* function_;
    void* jit_data_;

  public:
    static void init(STATE);
    static MachineMethod* create(STATE, VMMethod* vmm, JITCompiler& jit);
    static MachineMethod* create(STATE, VMMethod* vmm);

    void* function() {
      return reinterpret_cast<void*>(function_);
    }

    void set_function(void* p) {
      function_ = p;
    }

    // Used for debugging. Gives us a place to break on before entering jit'd code
    static Object* run_code(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args);

    // Ruby.primitive :machine_method_show
    Object* show(STATE);

    // Ruby.primitive :machine_method_activate
    Object* activate();

    void* resolve_virtual_ip(int ip);
  };
}

#endif

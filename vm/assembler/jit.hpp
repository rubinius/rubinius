#include "assembler/assembler_x86.hpp"
#include "assembler/operations.hpp"
#include "assembler/code_map.hpp"

namespace rubinius {
  class VMMethod;
  class MachineMethod;

  class JITCompiler {
  private: // data
    // indicates if ebx contains the current stack top
    bool stack_cached_;
    uint8_t* buffer_;

    assembler_x86::AssemblerX86 a;
    operations::StackOperations s;
    operations::ObjectOperations ops;

    // Contains the mapping between virtual ip and native ip
    CodeMap virtual2native;

  public:
    JITCompiler();
    ~JITCompiler();

    assembler_x86::AssemblerX86& assembler() {
      return a;
    }

    CodeMap& code_map() {
      return virtual2native;
    }

    void compile(VMMethod*);
    void show();

    static ExecuteStatus slow_plus_path(VMMethod* const vmm, Task* const task,
        MethodContext* const ctx);

    static ExecuteStatus slow_minus_path(VMMethod* const vmm, Task* const task,
        MethodContext* const ctx);

    static ExecuteStatus slow_equal_path(VMMethod* const vmm, Task* const task,
        MethodContext* const ctx);

    static ExecuteStatus slow_nequal_path(VMMethod* const vmm, Task* const task,
        MethodContext* const ctx);

    static ExecuteStatus check_interrupts(VMMethod* const vmm, Task* const task,
      MethodContext* const ctx);

  private:

    // Emit code to check %eax and determine if a new context was
    // installed.
    void maybe_return(int i, uintptr_t **last_imm, assembler_x86::AssemblerX86::NearJumpLocation& fin);

    // Pull the stack pointer into ebx if it's not there already
    void cache_stack(bool force = false);

    // Save ebx back into the MethodContext if it's currently cached
    void uncache_stack(bool force = false);
  };

}

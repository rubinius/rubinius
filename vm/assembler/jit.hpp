#include "assembler/assembler_x86.hpp"
#include "assembler/operations.hpp"
#include "assembler/code_map.hpp"

namespace rubinius {
  class VMMethod;
  class VM;

  class JITCompiler {
  private: // data
    // indicates if ebx contains the current stack top
    bool stack_cached_;

    bool own_buffer_;
    uint8_t* buffer_;

    assembler_x86::AssemblerX86 a;
    operations::StackOperations s;
    operations::ObjectOperations ops;

    // Contains the mapping between virtual ip and native ip
    CodeMap virtual2native;

    // Contains comments about addresses
    AddressComments comments_;

  public:
    const static int cFlagUnwoundTo = (1 << 0);
    const static int cRecordV2N     = (1 << 1);

    JITCompiler();
    JITCompiler(uint8_t* buffer);

    assembler_x86::AssemblerX86& assembler() {
      return a;
    }

    CodeMap& code_map() {
      return virtual2native;
    }

    AddressComments& comments() {
      return comments_;
    }

    void compile(VM*, VMMethod*);
    void show();

    void** create_interpreter(VM*);

    void emit_fast_math(assembler_x86::AssemblerX86::NearJumpLocation& done, bool add);
    void emit_fast_equal(assembler_x86::AssemblerX86::NearJumpLocation& done, bool equal);
    void emit_fast_compare(assembler_x86::AssemblerX86::NearJumpLocation& done, bool less);
    void emit_opcode(opcode op, assembler_x86::AssemblerX86::NearJumpLocation& fin);

   static Object * slow_plus_path(STATE, VMMethod* const vmm, CallFrame* const call_frame);

   static Object * slow_minus_path(STATE, VMMethod* const vmm, CallFrame* const call_frame);

   static Object * slow_equal_path(STATE, VMMethod* const vmm, CallFrame* const call_frame);

   static Object * slow_nequal_path(STATE, VMMethod* const vmm, CallFrame* const call_frame);

   static Object * slow_lt_path(STATE, VMMethod* const vmm, CallFrame* const call_frame);

   static Object * slow_gt_path(STATE, VMMethod* const vmm, CallFrame* const call_frame);
//
//    static ExecuteStatus check_interrupts(VMMethod* const vmm, Task* const task,
//      MethodContext* const ctx);

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

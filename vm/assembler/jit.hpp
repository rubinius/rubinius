#include "assembler_x86.hpp"
#include "operations.hpp"

namespace rubinius {
  class VMMethod;

  class JITCompiler {
    // indicates if ebx contains the current stack top
    bool stack_cached_;

    assembler_x86::AssemblerX86 a;
    operations::StackOperations s;
    operations::ObjectOperations ops;

  public:
    JITCompiler();
    void compile(VMMethod*);

  private:
    // Pull the stack pointer into ebx if it's not there already
    void cache_stack();

    // Save ebx back into the MethodContext if it's currently cached
    void uncache_stack();
  };
}

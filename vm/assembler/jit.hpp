#include "assembler_x86.hpp"
#include "operations.hpp"

#include <map>

namespace rubinius {
  class VMMethod;

  class JITCompiler {
  public: // Types
    typedef std::map<rubinius::opcode, void*> CodeMap;

  private: // data
    // indicates if ebx contains the current stack top
    bool stack_cached_;

    assembler_x86::AssemblerX86 a;
    operations::StackOperations s;
    operations::ObjectOperations ops;

    // Contains the mapping between virtual ip and native ip
    CodeMap virtual2native;

  public:
    JITCompiler();
    void compile(VMMethod*);

    static void slow_plus_path();

  private:
    // Pull the stack pointer into ebx if it's not there already
    void cache_stack();

    // Save ebx back into the MethodContext if it's currently cached
    void uncache_stack();
  };
}

#include <iostream>
#include "environment.hpp"
#include "object.hpp"
#include "type_info.hpp"
#include "compiled_file.hpp"
#include "builtin/compiled_code.hpp"
#include "llvm.hpp"

#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Function.h>
#include <llvm/CallingConv.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>
#include <llvm/InlineAsm.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/TypeSymbolTable.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/CallGraph.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/LinkAllPasses.h>
#include <llvm/ModuleProvider.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/ADT/DenseMap.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>

#include <iostream>
#include <fstream>

using namespace std;
using namespace rubinius;

int main(int argc, char** argv) {
  if(argc != 3) {
    cout << "Usage: file.rbc out.bc" << endl;
    return 1;
  }

  string input(argv[1]);
  string output(argv[2]);

  Environment env(argc, argv);

  ifstream stream(input.c_str());

  CompiledFile* cf = CompiledFile::load(stream);
  if(cf->magic != "!RBIX") throw std::runtime_error("Invalid file");

  CompiledCode* meth = as<CompiledCode>(cf->body(env.state));

  delete cf;

  VMLLVMMethod* lm = new VMLLVMMethod(env.state, meth);
  lm->compile(env.state);

  /* Copied from llvm-extract */
  llvm::Module* mod = env.state->llvm_module();
  llvm::PassManager passes;
  passes.add(new llvm::TargetData(mod));
  std::vector<llvm::GlobalValue*> GVs;

  GVs.push_back(lm->function);

  passes.add(llvm::createGVExtractionPass(GVs));
  passes.add(llvm::createDeadTypeEliminationPass());
  passes.add(llvm::createStripDeadPrototypesPass());

  ofstream out(output.c_str());

  passes.add(llvm::CreateBitcodeWriterPass(out));
  passes.run(*mod);

  cout << "Wrote to: " << output << endl;
}

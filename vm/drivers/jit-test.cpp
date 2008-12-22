#include "prelude.hpp"
#include "object_utils.hpp"
#include "jit.hpp"
#include "environment.hpp"

#include "builtin/compiledmethod.hpp"
#include "compiled_file.hpp"

#include <iostream>
#include <fstream>

using namespace rubinius;
using namespace std;

int main(int argc, char **argv) {
  JITCompiler compiler;
  VM state;

  std::ifstream stream(argv[1]);
  if(!stream) {
    cout << "Can't open ' " << argv[1] << "'\n";
    return 1;
  }

  CompiledFile* cf = CompiledFile::load(stream);
  if(cf->magic != "!RBIX") {
    cout << "Invalid file.\n";
  }

  VMMethod* vmm = as<CompiledMethod>(cf->body(&state))->formalize(&state, false);

  delete cf;

  MachineMethod* mm = compiler.compile(vmm);
  mm->show();
  return 0;
}

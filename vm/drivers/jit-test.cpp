#include "prelude.hpp"
#include "object_utils.hpp"
#include "vmmethod.hpp"
#include "jit.hpp"
#include "environment.hpp"
#include "config_parser.hpp"

#include "builtin/compiledmethod.hpp"
#include "builtin/machine_method.hpp"
#include "compiled_file.hpp"

#include <iostream>
#include <fstream>

using namespace rubinius;
using namespace std;

int main(int argc, char **argv) {
  JITCompiler compiler;
  VMManager * manager = new VMManager;
  SharedState * shared = manager->create_shared_state();
  ConfigParser * config = new ConfigParser;
  shared->user_config = config;
  VM * state = shared->new_vm();

  state->initialize(VM::default_bytes);
  state->boot();

  state->global_lock().lock();

  std::ifstream stream(argv[1]);
  if(!stream) {
    cout << "Can't open ' " << argv[1] << "'\n";
    return 1;
  }

  CompiledFile* cf = CompiledFile::load(stream);
  if(cf->magic != "!RBIX") {
    cout << "Invalid file.\n";
  }

  VMMethod* vmm = as<CompiledMethod>(cf->body(state))->formalize(state, false);

  delete cf;

  compiler.compile(state, vmm);
  MachineMethod* mm = MachineMethod::create(state, vmm, compiler);
  mm->show();
  return 0;
}

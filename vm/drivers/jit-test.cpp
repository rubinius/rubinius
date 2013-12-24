#include "prelude.hpp"
#include "object_utils.hpp"
#include "machine_code.hpp"
#include "jit.hpp"
#include "environment.hpp"
#include "config_parser.hpp"

#include "builtin/compiled_code.hpp"
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

  MachineCode* mcode = as<CompiledCode>(cf->body(state))->formalize(state, false);

  delete cf;

  compiler.compile(state, mcode);
  MachineMethod* mm = MachineMethod::create(state, mcode, compiler);
  mm->show();
  return 0;
}

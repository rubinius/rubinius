#include <iostream>
#include <fstream>

#include <sys/stat.h>

#include "vm/environment.hpp"
#include "vm/oop.hpp"
#include "vm/type_info.hpp"
#include "vm/exception.hpp"

#include "builtin/task.hpp"

using namespace std;
using namespace rubinius;

static void load_runtime_kernel(Environment& env, std::string root) {
  std::string dirs = root + "/index";
  std::ifstream stream(dirs.c_str());
  if(!stream) {
    std::cout << "It appears that " << root << "/index is missing.\n";
    exit(1);
  }

  while(!stream.eof()) {
    std::string line;

    stream >> line;
    stream.get(); // eat newline

    // skip empty lines
    if(line.size() == 0) continue;

    env.load_directory(root + "/" + line);
  }
}

int main(int argc, char** argv) {
  Environment env;
  env.load_argv(argc, argv);

  try {
    const char* runtime = getenv("RBX_RUNTIME");

    if(!runtime) {
      struct stat st;

      runtime = RBA_PATH;
      if(stat(runtime, &st) == -1 || !S_ISDIR(st.st_mode)) {
        Assertion::raise("set RBX_RUNTIME to runtime (or equiv)");
      }
    }

    std::string root = std::string(runtime);

    env.load_platform_conf(root);

    load_runtime_kernel(env, std::string(root));

    std::string loader = root + "/loader.rbc";

    env.enable_preemption();
    env.run_file(loader);
    return 0;

  } catch(Assertion &e) {
    std::cout << "VM Assertion:" << std::endl;
    std::cout << "  " << e.reason << std::endl;
    e.print_backtrace();

    std::cout << "Ruby backtrace:" << std::endl;
    env.state->print_backtrace();
  } catch(RubyException &e) {
    // Prints Ruby backtrace, and VM backtrace if captured
    e.show(env.state);
  } catch(TypeError &e) {
    std::cout << "Type Error detected:" << std::endl;
    TypeInfo* wanted = env.state->find_type(e.type);

    if(!e.object->reference_p()) {
      std::cout << "  Tried to use non-reference value " << e.object;
    } else {
      TypeInfo* was = env.state->find_type(e.object->obj_type);
      std::cout << "  Tried to use object of type " <<
        was->type_name << " (" << was->type << ")";
    }

    std::cout << " as type " << wanted->type_name << " (" <<
      wanted->type << ")" << std::endl;

    e.print_backtrace();

    std::cout << "Ruby backtrace:" << std::endl;
    env.state->print_backtrace();
  } catch(std::runtime_error& e) {
    std::cout << "Runtime exception: " << e.what() << std::endl;
  } catch(VMException &e) {
    std::cout << "Unknown VM exception detected." << std::endl;
    e.print_backtrace();
  } catch(std::string e) {
    std::cout << e << std::endl;
  } catch(...) {
    std::cout << "Unknown exception detected." << std::endl;
  }
}

#include <iostream>
#include "environment.hpp"
#include "object.hpp"
#include "type_info.hpp"
#include "builtin/task.hpp"

#include <iostream>
#include <fstream>

using namespace std;
using namespace rubinius;

static void load_root(Environment& env, std::string root) {
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

    std::string path = root + "/" + line;
    std::cout << "Loading directory: " << path << "\n";
    env.load_directory(path);

    std::cout << "Loading platform.conf: " << root << "\n";
    env.load_platform_conf(root);
  }
}

int main(int argc, char** argv) {
  Environment env;
  env.load_argv(argc, argv);

  try {
    char* e = getenv("ROOT");
    if(e) {
      load_root(env, std::string(e));
    }

    if(argc < 2) {
      std::cout << "Usage: file.rbc" << std::endl;
      return 1;
    }

    std::string str(argv[1]);
    std::cout << "Loading: " << str << std::endl;

    env.run_file(str);
    return 0;

  } catch(ObjectBoundsExceeded *e) {
    TypeInfo* info = env.state->find_type(e->obj->obj_type); // HACK use object

    std::cout << "Bounds of object exceeded:" << std::endl;
    std::cout << "  type: " << info->type_name << ", fields: " <<
      e->obj->field_count << ", accessed: " << e->index << std::endl;
    e->print_backtrace();
  } catch(Assertion *e) {
    std::cout << "VM Assertion:" << std::endl;
    std::cout << "  " << e->reason << std::endl;
    e->print_backtrace();
  } catch(TypeError *e) {
    std::cout << "Type Error detected:" << std::endl;
    TypeInfo* wanted = env.state->find_type(e->type);

    if(!e->object->reference_p()) {
      std::cout << "  Tried to use non-reference value " << e->object;
    } else {
      TypeInfo* was = env.state->find_type(e->object->obj_type);
      std::cout << "  Tried to use object of type " <<
        was->type_name << " (" << was->type << ")";
    }

    std::cout << " as type " << wanted->type_name << " (" <<
      wanted->type << ")" << std::endl;

    e->print_backtrace();
  } catch(std::runtime_error& e) {
    std::cout << "Runtime exception: " << e.what() << std::endl;
  } catch(ArgumentError *e) {
    std::cout << "Argument error: expected " << e->expected << ", given " <<
      e->given << std::endl;
    env.state->print_backtrace();
  } catch(VMException *e) {
    std::cout << "Unknown VM exception detected." << std::endl;
    e->print_backtrace();
  } catch(std::string e) {
    std::cout << e << std::endl;
  } catch(...) {
    std::cout << "Unknown exception detected." << std::endl;
  }
}

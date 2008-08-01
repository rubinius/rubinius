#include <iostream>
#include "environment.hpp"
#include "object.hpp"
#include "type_info.hpp"

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
    std::cout << "Bounds of object exceeded:" << std::endl;
    std::cout << "  type: " << (int)e->obj->obj_type << ", fields: " << 
      e->obj->field_count << ". Accessed: " << e->index << std::endl;
  } catch(Assertion *e) {
    std::cout << "VM Assertion:" << std::endl;
    std::cout << "  " << e->reason << std::endl;
  } catch(TypeError *e) {
    std::cout << "Type Error detected:" << std::endl;
    TypeInfo* was = env.state->find_type(e->object->obj_type);
    TypeInfo* wanted = env.state->find_type(e->type);
    std::cout << "  Tried to use object of type " <<
      was->type_name << " (" << was->type << ")" <<
      " as type " << wanted->type_name << " (" << wanted->type << ")" << std::endl;
  } catch(std::runtime_error& e) {
    std::cout << "Runtime exception: " << e.what() << std::endl;
  } catch(VMException *e) {
    std::cout << "Unknown VM exception detected." << std::endl;
  } catch(std::string e) {
    std::cout << e << std::endl;
  } catch(...) {
    std::cout << "Unknown exception detected." << std::endl;
  }
}

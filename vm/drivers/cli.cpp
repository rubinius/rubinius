#include "environment.hpp"

using namespace rubinius;

int main(int argc, char** argv) {
  Environment env;
  env.load_argv(argc, argv);
  /*
  char* e = getenv("DIR");
  if(!e) return 1;
  */
  if(argc < 2) {
    std::cout << "Usage: file.rbc" << std::endl;
    return 1;
  }

  std::string str(argv[1]);
  std::cout << "Loading: " << str << std::endl;
  try {
    env.run_file(str);
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

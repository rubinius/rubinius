#include "environment.hpp"

using namespace rubinius;

int main(int argc, char** argv) {
  Environment env;
  env.load_argv(argc, argv);
  char* e = getenv("DIR");
  if(!e) return 1;
  std::string str(e);
  std::cout << "Loading: " << str << std::endl;
  try {
    env.load_directory(str);
  } catch(std::runtime_error& e) {
    std::cout << "Runtime exception: " << e.what() << std::endl;
  } catch(...) {
    std::cout << "Unknown exception detected." << std::endl;
  }
}

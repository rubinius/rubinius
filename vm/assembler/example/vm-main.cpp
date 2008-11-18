#include "vm.hpp"

#include <iostream>

int main(int argc, char** argv) {
  int end = 15;
  int top = 6;

  int program[] = {
    RegisterVM::set, 1, 900000000,
    RegisterVM::set, 0, 0,
    RegisterVM::jump_if_equal, 0, 1, end,
    RegisterVM::add, 0, 1,
    RegisterVM::jump, top,
    RegisterVM::show, 0,
    RegisterVM::exit
  };

  RegisterVM vm;
  if(argc == 2 && argv[1][0] == 'j') {
    std::cout << "running with jit.\n";
    RegisterVM::Function func = vm.compile(program, 18);

    int registers[32];
    func(registers);
  } else {
    std::cout << "running with interpretter.\n";
    vm.run(program);
  }
}

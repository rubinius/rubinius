#include "machine.hpp"

int main(int argc, char** argv) {
  rubinius::Machine machine(argc, argv);

  machine.boot();

  return machine.halt();
}

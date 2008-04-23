#include "vm.hpp"

int main() {
  rubinius::VM vm;
  vm.collect();
  return 0;
}

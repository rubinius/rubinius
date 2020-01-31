#include "vm.hpp"
#include "thread_state.hpp"

int main() {
  rubinius::VM vm;
  vm.collect();
  return 0;
}

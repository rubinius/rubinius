#include <stdint.h>

int main(int argc, char **argv) {
  /* 0 is false to C, but true to shell. I hate unix sometimes. */
  /* Me too. */
  return sizeof(uintptr_t) != 8;
}

int main(int argc, char **argv) {
  /* 0 is false to C, but true to shell. I hate unix sometimes. */
  return sizeof(int) == sizeof(void*);
}

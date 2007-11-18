int main(int argc, char **argv) {
  char *data = "\x12\x34\x56\x78";
  int *idata = (int*)data;

  /* 0 is false to C, but true to shell. I hate unix sometimes. */
  return !(*idata == 0x12345678);
}

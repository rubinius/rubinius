#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  char** new_args = calloc(argc+2, sizeof(char*));
  size_t i;
  setenv("BUILDDIR", BUILDDIR, 1);
  setenv("PROGNAME", argv[0], 1);
  new_args[0] = "ruby";
  new_args[1] = BUILDDIR "/run-dev.rb";
  for(i = 1; i < argc; i++) {
    new_args[i + 1] = argv[i];
  }
  new_args[argc + 1] = NULL;
  execvp("ruby", new_args);
}

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  struct stat *st, sb;
  int i, total = argc > 1 ? atoi(argv[1]) : 1000;
  
  if(argc > 1) {
    st = malloc(sizeof(struct stat));
  } else {
    st = &sb;
  }
  
  for(i = 0; i < total; i++) {
    stat("./spec/ruby/1.8/core/true/and_spec.rb", st);
  }
  
  return 0;
}

#include "shotgun.h"
#include "machine.h"

int main(int argc, char **argv) {
  char *path;
  machine m;
  
  if(argc <= 2) {
    printf("Please pass in a file to run.\n");
    return 1;
  }
  
  path = argv[1];
    
  m = machine_new();
  machine_setup_standard_io(m);
  machine_setup_ruby(m, argv[0]);
  /* move argc and argv around so that the kernel and rubinius
     don't show up. */
  machine_config_env(m);
  machine_setup_argv(m, argc-1, argv+1);
  machine_setup_env(m);
  
  if(!machine_run_file(m, path)) {
    printf("Unable to run %s\n", path);
    return 1;
  }
  
  machine_collect(m);
  machine_emit_memory(m);
  
  // object_memory_print_stats(m->s->om);
    
  return 0;
}

#define MAIN 1

#include "shotgun.h"
#include "machine.h"
#include <sys/stat.h>
#include <string.h>

void *__main_address;

static char kernel_path[PATH_MAX+1];

static char *find_kernel(char *rubinius_path) {
  char *env;
  struct stat st = {0,};
  env = getenv("KERNEL");
  if(env)
  {
      strncpy (&kernel_path[0], env, PATH_MAX );
  }
  else {
      int len;
      
      /* Get dirname of rubinius */
      strncpy (&kernel_path[0], rubinius_path, PATH_MAX );
      dirname ( kernel_path );
      len = PATH_MAX - strlen ( kernel_path );
      strncat ( &kernel_path[0], "/../runtime/kernel.rba", len );
      if (stat( kernel_path, &st) != 0)
      {
          strncpy (&kernel_path[0], rubinius_path, PATH_MAX );
          dirname ( kernel_path );
          len = PATH_MAX - strlen ( kernel_path );
          strncat ( &kernel_path[0], "/../runtime/kernel.rbc", len );          
      }
  }
  
  return kernel_path;
}

int main(int argc, char **argv) {
  char *kernel;
  machine m;
  struct stat st;
  int offset = 0;
  int flag;
    
  /* Setup the global that contains the address of the 
     frame pointer for main. This is so the missing
     backtrace knows where to stop looking for return address. */
  __main_address = __builtin_frame_address(0);
  
  kernel = find_kernel( argv[0] );
  if(!kernel) {
    printf("Unable to find a kernel to load!\n");
    return 1;
  }
  
  if(stat(kernel, &st) < 0) {
    printf("Kernel '%s' not found.\n", kernel);
    return 1;
  }
  
  m = machine_new();
  machine_save_args(m, argc, argv);
  machine_setup_standard_io(m);
  machine_setup_ruby(m, argv[0]);
  /* move argc and argv around so that the kernel and rubinius
     don't show up. */
  machine_config_env(m);
  machine_setup_argv(m, argc-offset, argv+offset);
  machine_setup_env(m);
  
  if(strstr(kernel, ".rba")) {
    flag = machine_load_archive(m, kernel);
  } else {
    flag = machine_run_file(m, kernel);
  }
  
  if(!flag) {
    printf("Unable to run %s\n", kernel);
    return 1;
  }
  
  // machine_emit_memory(m);
  
  // object_memory_print_stats(m->s->om);
    
  return 0;
}

#define MAIN 1

#include "shotgun.h"
#include "machine.h"
#include <sys/stat.h>
#include <libgen.h>
#include <string.h>

void *__main_address;

/* WATCH OUT: pointer returned by find_kernel should be free'd by called */

static char *find_kernel(char *rubinius_path) {
  char *env;
  char *dir;
  char *rb_path = NULL;
  char kernel_path[PATH_MAX+1];  
  struct stat st = {0,};
  
  env = getenv("KERNEL");

  if(env)  {
      strncpy (&kernel_path[0], env, PATH_MAX );
  }
  else
  {    
      /* We need to get the dir part of the rubinius path */
      /* as dirname can modify its parameter (at least if we keep POSIX */
      /* compliance), we need to dup it first (at think about releasing */
      rb_path = strdup ( rubinius_path );
      dir = dirname ( rb_path );

      snprintf (&kernel_path[0], PATH_MAX, "%s/../runtime/kernel.rba", dir);
      
      /* Is kernel.rba available ? */
      if (stat( kernel_path, &st) != 0)
      {
          /* No ? so let's build a potential path to kernel.rbc  */
          snprintf (&kernel_path[0], PATH_MAX, "%s/../runtime/kernel.rbc", dir);         
      }
  }

  if ( rb_path != NULL )
  {
      free (rb_path);
      rb_path = NULL;
  }
  
  /* Let's dup kernel_path (called needs to think about freeing it) */
  rb_path = strdup ( &kernel_path[0]);
  
  return rb_path;
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
    free (kernel);
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
    free (kernel);
    return 1;
  }
  
  // machine_emit_memory(m);
  
  // object_memory_print_stats(m->s->om);
    
  free (kernel);
  return 0;
}

#define MAIN 1

#include "shotgun.h"
#include "machine.h"
#include <sys/stat.h>
#include <string.h>
#include <setjmp.h>

#include "config.h"
#include "subtend/ruby.h"

void *__main_address;

/* TODO incorporate system paths calculated at compile time. */
#ifdef CONFIG_RBAPATH
char *search_path[] = {"runtime", CONFIG_RBAPATH, NULL};
#else
char *search_path[] = {"runtime", NULL};
#endif

static char *search_for(char *evs, char *file) {
  char *env;
  char path[PATH_MAX];
  struct stat _st;
  int i;

  #define file_exists_p(file) stat(file, &_st) == 0
  
  env = getenv(evs);
  if(env) {
    if(file_exists_p(env)) return strdup(env);
    return NULL;
  }
  
  for(i = 0; search_path[i]; i++) {
    snprintf(path, PATH_MAX, "%s/%s", search_path[i], file);
    if(file_exists_p(path)) {
      return strdup(path);
    }
  }
  
  return NULL;
}

static void load_libs() {
  subtend_get_global(0);
}

int main(int argc, char **argv) {
  machine m;
  char *archive;
  int offset = 0;
  int flag;
  
  /* Setup the global that contains the address of the 
     frame pointer for main. This is so the missing
     backtrace knows where to stop looking for return address. */
  __main_address = __builtin_frame_address(0);
  
  m = machine_new();
  /* We sure to setup the bottom of the stack so it can be properly saved. */
  m->s->stack_bottom = (unsigned long*)&m;
  machine_save_args(m, argc, argv);
  machine_setup_standard_io(m);
  machine_setup_ruby(m, argv[0]);
  machine_setup_config(m);
  /* move argc and argv around so that the kernel and rubinius
     don't show up. */
  machine_config_env(m);
  machine_setup_from_config(m);
  machine_setup_argv(m, argc-offset, argv+offset);
  machine_setup_env(m);
  
  /* Load the bootstrap. */
  
  archive = search_for("BOOTSTRAP", "bootstrap.rba");
  if(!archive) {
    printf("Unable to find a bootstrap (bootstrap.rba) to load!\n");
    return 1;
  }
    
  flag = machine_load_archive(m, archive);
  
  if(!flag) {
    printf("Unable to run %s\n", archive);
    return 1;
  }
  
  /* Load the core. */

  archive = search_for("CORE", "core.rba");
  if(!archive) {
    printf("Unable to find a core (core.rba) to load!\n");
    return 1;
  }
    
  flag = machine_load_archive(m, archive);
  
  if(!flag) {
    printf("Unable to run %s\n", archive);
    return 1;
  }
  
  // machine_emit_memory(m);
  
  // object_memory_print_stats(m->s->om);
    
  return 0;
}

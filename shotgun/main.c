#include <string.h>
#include <setjmp.h>
#include <sys/stat.h>

#include "shotgun/config.h"
#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/machine.h"
#include "shotgun/lib/subtend/ruby.h"

/* TODO incorporate system paths calculated at compile time. */
static const char *search_path[] = {
  "runtime",
#ifdef CONFIG_RBAPATH
  CONFIG_RBAPATH,
#endif
  NULL
};

static const char *search_for(const char *evs, const char *file) {
  char *env;
  static char path[PATH_MAX];
  struct stat _st;
  int i;

  #define file_exists_p(file) stat(file, &_st) == 0
  
  env = getenv(evs);
  if(env) {
    if(file_exists_p(env)) return env;
    return NULL;
  }
  
  for(i = 0; search_path[i]; i++) {
    snprintf(path, PATH_MAX, "%s/%s", search_path[i], file);
    if(file_exists_p(path)) {
      return path;
    }
  }
  
  return NULL;
}

int main(int argc, char **argv) {
  machine m;
  const char *archive;
  int offset = 0;
  int flag;
    
  m = machine_new();
  machine_save_args(m, argc, argv);
  machine_setup_standard_io(m);
  machine_setup_ruby(m, argv[0]);
  machine_setup_config(m);
  /* move argc and argv around so that the kernel and rubinius
     don't show up. */
  machine_config_env(m);
  
  archive = search_for("RBX_PLATFORM_CONF", "platform.conf");
  if(archive) {
    machine_parse_config_file(m, archive);
  }

  machine_migrate_config(m);
  machine_setup_from_config(m);
  machine_setup_argv(m, argc-offset, argv+offset);
  
  /* Load the bootstrap. */
  
  archive = search_for("RBX_BOOTSTRAP", "bootstrap");
  if(!archive) {
    printf("Unable to find a bootstrap to load!\n");
    return 1;
  }
    
  flag = machine_load_bundle(m, archive);
  
  if(!flag) {
    printf("Unable to run %s\n", archive);
    return 1;
  }
  
  /* Load the platform. */
  
  archive = search_for("RBX_PLATFORM", "platform");
  if(!archive) {
    printf("Unable to find a platform to load!\n");
    return 1;
  }
    
  flag = machine_load_bundle(m, archive);
  
  if(!flag) {
    printf("Unable to run %s\n", archive);
    return 1;
  }
  
  /* Load the core. */

  archive = search_for("RBX_CORE", "core");
  if(!archive) {
    printf("Unable to find a core to load!\n");
    return 1;
  }
    
  flag = machine_load_bundle(m, archive);
   
  if(!flag) {
    printf("Unable to run %s\n", archive);
    return 1;
  }
  
  /* Load the loader.rbc */
  
  archive = search_for("RBX_LOADER", "loader.rbc");
  if(!archive) {
    printf("Unable to find loader.rbc to load!\n");
    return 1;
  }
  
  flag = machine_run_file(m, archive);
  
  if(!flag) {
    printf("Unable to run %s\n", archive);
    return 1;
  }
  
  /* Done! */
  
  if(m->s->gc_stats) {
    printf("[GC M %6dK total]\n", m->s->om->ms->allocated_bytes);
  }
  
  
  // machine_emit_memory(m);
  
  // object_memory_print_stats(m->s->om);
    
  return 0;
}

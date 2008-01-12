#include <string.h>
#include <setjmp.h>
#include <sys/stat.h>

#include "shotgun/config.h"
#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/machine.h"
#include "shotgun/lib/subtend/ruby.h"
#include "shotgun/lib/environment.h"

/* TODO incorporate system paths calculated at compile time. */
static const char *search_path[] = {
  "runtime",
#ifdef CONFIG_RBAPATH
  CONFIG_RBAPATH,
#endif
  NULL
};

static char *search_for(const char *evs, const char *file) {
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
      return strdup(path);
    }
  }
  
  return NULL;
}

int main(int argc, char **argv) {
  environment e;
  machine m;
  char *archive;
 
  environment_at_startup();

  e = environment_new();
  m = machine_new();
  environment_add_machine(e, m);
  environment_setup_thread(e, m);

  machine_setup_normal(m, argc, argv);

  /* We build up the environment information by looking around
   * the filesystem now. */
 
  /* Find the platform config */
  e->platform_config = search_for("RBX_PLATFORM_CONF", "platform.conf");
  
  /* Find the bootstrap. */
  archive = search_for("RBX_BOOTSTRAP", "bootstrap");
  if(!archive) {
    printf("Unable to find a bootstrap to load!\n");
    return 1;
  }
  e->bootstrap_path = archive;
  
  /* Find the platform. */
  archive = search_for("RBX_PLATFORM", "platform");
  if(!archive) {
    printf("Unable to find a platform to load!\n");
    return 1;
  }
  
  e->platform_path = archive;
  
  /* Find the core. */
  archive = search_for("RBX_CORE", "core");
  if(!archive) {
    printf("Unable to find a core to load!\n");
    return 1;
  }
  
  e->core_path = archive;
  
  /* Load the loader.rbc */
  archive = search_for("RBX_LOADER", "loader.rbc");
  if(!archive) {
    printf("Unable to find loader.rbc to load!\n");
    return 1;
  }
  
  e->loader_path = archive;

  environment_load_machine(e, m);
    
  /* Done! */
  if(m->s->gc_stats) {
    printf("[GC M %6dK total]\n", m->s->om->ms->allocated_bytes);
  }
  
  return 0;
}

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/environment.h"
#include "shotgun/lib/config_hash.h"

#include <pthread.h>
#include <signal.h>
#include <event.h>

static pthread_key_t global_key;

#define lock(e) pthread_mutex_lock(&e->mutex)
#define unlock(e) pthread_mutex_unlock(&e->mutex)

struct event_base *_rbx_base_finder() {
  machine m = environment_current_machine();
  return m->s->event_base;
}

void environment_at_startup() {
  pthread_key_create(&global_key, NULL);
  evsignal_set_base_finder(_rbx_base_finder);
}

environment environment_new() {
  environment e = ALLOC_N(struct rubinius_environment, 1);
  e->machines = ht_vconfig_create(11);
  e->machine_id = 1;

  return e;
}

void environment_setup_thread(environment e, machine m) {
  struct rubinius_global *global = ALLOC_N(struct rubinius_global, 1);
  
  global->e = e;
  global->m = m;

  pthread_setspecific(global_key, (const void*)global);
}

environment environment_current() {
  struct rubinius_global *global;
  global = (struct rubinius_global*)pthread_getspecific(global_key);
  return global->e;
}

machine environment_current_machine() {
  struct rubinius_global *global;
  global = (struct rubinius_global*)pthread_getspecific(global_key);
  return global->m;
}

void environment_add_machine(environment e, machine m) {
  int *key;

  lock(e);
  m->pthread = pthread_self();
  m->id = e->machine_id++;

  key = ALLOC_N(int, 1);
  *key = m->id;

  ht_vconfig_insert(e->machines, key, (void*)m);
  unlock(e);
}

int environment_del_machine(environment e, machine m) {
  machine old;

  lock(e);
  old = ht_vconfig_remove(e->machines, &m->id);
  unlock(e);

  assert(old == m);
  return TRUE;
}

int environment_join_machine(environment e, int id) {
  machine m;
  void *ret;

  lock(e);
  m = ht_vconfig_search(e->machines, &id);
  unlock(e);

  if(!m) return FALSE;

  if(pthread_join(m->pthread, &ret) == 0) {
    return TRUE;
  }

  return FALSE;
}

void environment_exit_machine() {
  // machine m = environment_current_machine();
  // environment e = environment_current();

  pthread_exit(NULL);
}

int environment_load_machine(environment e, machine m) {
  if(e->platform_config) {
    machine_parse_config_file(m, e->platform_config);
  }

  machine_migrate_config(m);
  
  if(!machine_load_bundle(m, e->bootstrap_path)) {
    printf("Problem encountered while loading %s\n", e->bootstrap_path);
    return FALSE;
  }

  if(!machine_load_bundle(m, e->platform_path)) {
    printf("Problem encountered while loading %s\n", e->platform_path);
    return FALSE;
  }

  if(!machine_load_bundle(m, e->core_path)) {
    printf("Problem encountered while loading %s\n", e->core_path);
    return FALSE;
  }

  if(!machine_run_file(m, e->loader_path)) {
    printf("Unclean exit from %s\n", e->core_path);
    return FALSE;
  }

  return TRUE;
}

struct thread_args {
  environment e;
  machine m;
};

void *_environment_spawn(void *input) {
  sigset_t set;
  struct thread_args *args = (struct thread_args*)input;
  
  environment_setup_thread(args->e, args->m);
  
  /* For now, we mask all signals except VTALRM in sub machines. */
  sigfillset(&set);
  //sigdelset(&set, SIGVTALRM);
  pthread_sigmask(SIG_SETMASK, &set, NULL);

  environment_load_machine(args->e, args->m);
  XFREE(args);
  return NULL;
}

void environment_start_thread(environment e, machine m) {
  struct thread_args *args = ALLOC_N(struct thread_args, 1);

  args->e = e;
  args->m = m;

  pthread_create(&m->pthread, NULL, _environment_spawn, (void*)args);
}

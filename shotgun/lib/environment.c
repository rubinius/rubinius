#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/environment.h"
#include "shotgun/lib/config_hash.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/tuple.h"

#include <pthread.h>
#include <signal.h>
#include <ev.h>
#include <unistd.h>

static pthread_key_t global_key;

#define lock(e) pthread_mutex_lock(&e->mutex)
#define unlock(e) pthread_mutex_unlock(&e->mutex)

void environment_at_startup() {
  pthread_key_create(&global_key, NULL);
}

static void _find_child_waiters(int key, void *mach) {
  machine m = mach;
  cpu_find_waiters(m->s);
}


static void _child_ev_cb(EV_P_ struct ev_signal *w, int revents) {
  environment e = (environment)w->data;

  // call for all VMs/states
  ht_vconfig_each(e->machines, _find_child_waiters);
}

environment environment_new() {
  environment e = ALLOC_N(struct rubinius_environment, 1);
  e->machines = ht_vconfig_create(11);
  e->machine_id = 1;

  e->messages = ht_vconfig_create(11);
  e->sig_event_base = ev_default_loop(EVFLAG_FORKCHECK);
  return e;
}

static void _fork_event_loops(int key, void *mach) {
  machine m = mach;
  ev_loop_fork(m->s->event_base);
  ev_loop(m->s->event_base, EVLOOP_NONBLOCK);
}

void environment_fork() {
  environment e = environment_current();

  ev_default_fork();
  ev_loop(e->sig_event_base, EVLOOP_NONBLOCK);
  ht_vconfig_each(e->machines, _fork_event_loops);
}

void environment_setup_thread(environment e, machine m) {
  struct rubinius_global *global = ALLOC_N(struct rubinius_global, 1);

  global->e = e;
  global->m = m;

  pthread_setspecific(global_key, (const void*)global);

  ev_signal_init(&e->sig_ev, _child_ev_cb, SIGCHLD);
  e->sig_ev.data = e;
  ev_signal_start(e->sig_event_base, &e->sig_ev);
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
    machine_destroy(m);
    return TRUE;
  }

  return FALSE;
}

void environment_exit_machine() {
  machine m = environment_current_machine();
  environment e = environment_current();
  STATE;

  state = m->s;

  if(m->parent_id) {
    environment_send_message(e, m->parent_id, 
        tuple_new2(state, 2, SYM("machine_exited"), I2N(m->id)));
  }

  pthread_exit(NULL);
}

int environment_load_machine(environment e, machine m) {
  if(e->platform_config) {
    machine_parse_config_file(m, e->platform_config);
  }

  machine_migrate_config(m);

  if(m->s->excessive_tracing) {
    printf("[ Loading bootstrap bundle %s]\n", e->bootstrap_path);
  }

  if(!machine_load_bundle(m, e->bootstrap_path)) {
    printf("Problem encountered while loading bootstrap %s\n", e->bootstrap_path);
    return FALSE;
  }

  if(m->s->excessive_tracing) {
    printf("[ Loading platform bundle %s]\n", e->platform_path);
  }

  if(!machine_load_bundle(m, e->platform_path)) {
    printf("Problem encountered while loading platform %s\n", e->platform_path);
    return FALSE;
  }

  if(m->s->excessive_tracing) {
    printf("[ Loading core bundle %s]\n", e->core_path);
  }

  if(!machine_load_bundle(m, e->core_path)) {
    printf("Problem encountered while loading core %s\n", e->core_path);
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

static const char magic[] = "!";

void environment_send_message(environment e, int id, OBJECT msg) {
  ptr_array cur;
  bstring data;
  machine m;
  int *key;

  /* Marshal the data before the lock. */
  m = environment_current_machine();
  data = cpu_marshal_to_bstring(m->s, msg, 0);
  
  lock(e);

  cur = ht_vconfig_search(e->messages, &id);
  if(!cur) {
    cur = ptr_array_new(8);
    key = ALLOC_N(int, 1);
    *key = id;
    ht_vconfig_insert(e->messages, key, cur);
  }

  ptr_array_append(cur, (xpointer)data);
  
  m = ht_vconfig_search(e->machines, &id);

  /* write the magic byte, to let the machine know there are
   * messages for it. */
  write(m->message_write_fd, magic, 1);

  unlock(e);
}

OBJECT environment_get_message(environment e, int id) {
  ptr_array cur;
  bstring data;
  machine m;

  lock(e);

  cur = ht_vconfig_search(e->messages, &id);
  if(!cur) goto error;
  if(ptr_array_length(cur) == 0) goto error;
  data = ptr_array_remove_index_ordered(cur, 0);
  if(!data) goto error;
  
  /* Now that we're sure we've got what we need, we unlock.. */
  unlock(e);

  /* and unmarshal. */
  m = environment_current_machine();
  return cpu_unmarshal(m->s, (uint8_t*)bdata(data), (int)blength(data), 0);
error:
  unlock(e);
  return Qnil;
}

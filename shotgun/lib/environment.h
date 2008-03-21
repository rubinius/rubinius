#ifndef RBS_ENVIRON_H
#define RBS_ENVIRON_H

#include <ev.h>
#include "shotgun/lib/machine.h"

/*
 Rubinius environment stores load paths,
 platform configuration, list of spawned machines,
 event loop
 and synchronization mutex.

 One environment is automatically created on
 VM start.

 Each environment lives in it's own pthread
 */
struct rubinius_environment {
  pthread_mutex_t mutex;
  struct hashtable *machines;
  char *platform_config;
  char *bootstrap_path;
  char *platform_path;
  char *core_path;
  char *loader_path;

  int machine_id;

  struct hashtable *messages;
  struct ev_loop *sig_event_base;
  struct ev_signal sig_ev;
};

typedef struct rubinius_environment *environment;

struct rubinius_global {
  environment e;
  machine m;
};

environment environment_new();
void environment_at_startup();
void environment_setup_thread(environment e, machine m);
environment environment_current();
machine environment_current_machine();
void environment_add_machine(environment e, machine m);
int environment_del_machine(environment e, machine m);
int environment_load_machine(environment e, machine m);
void environment_start_thread(environment e, machine m);
void environment_exit_machine();
int environment_join_machine(environment e, int id);
void environment_fork();

void environment_send_message(environment e, int id, OBJECT msg);
OBJECT environment_get_message(environment e, int id);


#endif

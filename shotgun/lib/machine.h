#ifndef RBS_MACHINE_H
#define RBS_MACHINE_H

#include <signal.h>
#include <pthread.h>

#include "shotgun/lib/shotgun.h"

struct rubinius_machine {
  int id;
  int parent_id;
  pthread_t pthread;
  int sub;
  int message_read_fd;
  int message_write_fd;
  rstate s;
  cpu c;
  struct sigaction error_report;
  char *interpreter;
  int argc;
  char **argv;
  int show_config;
  ucontext_t g_firesuit;
  /* work around a bug in 10.5's libc versus header files */
#if __DARWIN_UNIX03
  _STRUCT_MCONTEXT __system_mc;
#endif
  int g_use_firesuit;
  int g_access_violation;
  int g_firesuit_arg;
};

typedef struct rubinius_machine *machine;

machine machine_new();
void machine_destroy(machine m);
OBJECT machine_load_file(machine m, const char *path);
int machine_run_file(machine m, const char *path);
OBJECT machine_load_archive(machine m, const char *path);
int machine_load_directory(machine m, const char *prefix);
int machine_load_bundle(machine m, const char *path);
void machine_set_const(machine m, const char *str, OBJECT val);
void machine_setup_standard_io(machine m);
int *machine_setup_piped_io(machine m);
void machine_collect(machine m);
void machine_setup_ruby(machine m, char *name);
void machine_setup_argv(machine m, int argc, char **argv);
void machine_config_env(machine m);
void machine_emit_memory(machine m);
void machine_save_args(machine m, int argc, char **argv);
void machine_setup_config(machine m);
void machine_setup_from_config(machine m);
void machine_parse_configs(machine m, const char *config);
void machine_migrate_config(machine m);
void machine_parse_config_file(machine m, const char *path);
void machine_setup_normal(machine m, int argc, char **argv);
int *machine_setup_thread(machine m, int argc, char **argv);
const char *_inspect(OBJECT obj);
void machine_print_callstack(machine m);
void machine_print_callstack_limited(machine m, int maxlev);

#endif

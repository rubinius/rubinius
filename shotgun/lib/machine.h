#ifndef RBS_MACHINE_H
#define RBS_MACHINE_H

#include <signal.h>
#include <pthread.h>

typedef struct rubinius_machine *machine;

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/environment.h"

/*
 Rubinius supports multiple VM (MVM) feature: machines may be spawned off other machines,
 every machine has an identifier which is incremented when new machine is added to environment.

 Machines pass messages to each other thus concurrency is cooperative like in Erlang.
 Incoming and outcoming messages are passed using stream descriptors.
 Incoming messages queue can be accessed in Ruby as MESSAGE_IO constant.

 Each VM operates in a separate pthread. Spawned VMs are known as "inferior VMs",
 this is reflected by VM_INFERIOR constant value in Ruby. VMs has name and keep
 arguments (notably argc/argv) passed on run.

 To carry VM context around Rubinius uses rstate structure. It contains a variety of things
 from object memory to stack frames state and so forth.

 If SIGSEGV/SIGBUS/SIGABRT happens during the execution it is handled by special context
 known as firesuite. Error message and type are accessible and shown in VM backtrace.

 To force Rubinius VM use print additional information set show_config flag to 1.

 sub : flag that indicates whether it is an inferior VM: i.e. spawn by another VM. In Ruby
 you can get this using VM_INFERIOR constant

 */
struct rubinius_machine {
  int id;
  int parent_id;
  pthread_t pthread;
  int sub;
  int message_read_fd;
  int message_write_fd;
  /* VM state: carried around to keep global VM context */
  rstate s;
  cpu c;
  struct sigaction error_report;
  char *interpreter;
  int argc;
  char **argv;
  int show_config;
  /* work around a bug in 10.5's libc versus header files */
#if defined(__APPLE__) && defined(HAS_UCONTEXT) /* patch for tiger */
  _STRUCT_MCONTEXT __system_mc;
#endif

/* these members relate to segfaults handling so they get reported correctly and VM exit gracefully(ish) */
  ucontext_t g_firesuit;
  int g_use_firesuit;
  int g_access_violation;
  int g_firesuit_arg;
  char *g_firesuit_message;
  pthread_t sampler_thread;
  unsigned int sampler_active;
};

machine machine_new(environment e);
void machine_destroy(machine m);
OBJECT machine_load_file(machine m, const char *path);
int machine_run_file(machine m, const char *path);
int machine_load_archive(machine m, const char *path);
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
void _source_location(int *line_number, const char **filename);
void machine_print_callstack(machine m);
void machine_print_callstack_limited(machine m, int maxlev);

#endif

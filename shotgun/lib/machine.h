#include "shotgun.h"
#include <signal.h>

struct rubinius_machine {
  rstate s;
  cpu c;
  struct sigaction error_report;
  char *interpreter;
  int argc;
  char **argv;
  int show_config;
};

typedef struct rubinius_machine *machine;

machine machine_new();
OBJECT machine_load_file(machine m, const char *path);
int machine_run_file(machine m, const char *path);
OBJECT machine_load_archive(machine m, const char *path);
int machine_load_directory(machine m, const char *prefix);
int machine_load_bundle(machine m, const char *path);
void machine_set_const(machine m, const char *str, OBJECT val);
void machine_setup_standard_io(machine m);
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
const char *_inspect(OBJECT obj);

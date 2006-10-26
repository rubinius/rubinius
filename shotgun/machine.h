#include "cpu.h"

struct rubinius_machine {
  rstate s;
  cpu c;
};

typedef struct rubinius_machine *machine;

machine machine_new();
OBJECT machine_load_file(machine m, char *path);
int machine_run_file(machine m, char *path);
void machine_set_const(machine m, char *str, OBJECT val);
void machine_setup_standard_io(machine m);
void machine_collect(machine m);
void machine_setup_ruby(machine m, char *name);
void machine_setup_argv(machine m, int argc, char **argv);
void machine_setup_env(machine m);
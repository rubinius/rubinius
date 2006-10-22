#include "rubinius.h"
#include "string.h"
#include "io.h"
#include "hash.h"
#include <unistd.h>
#include <stdlib.h>
#include "machine.h"
#include <string.h>

machine current_machine;

machine machine_new() {
  machine m;
  m = malloc(sizeof(struct rubinius_machine));
  m->s = rubinius_state_new();
  m->c = cpu_new(m->s);
  cpu_initialize(m->s, m->c);
  cpu_bootstrap(m->s);
  cpu_setup_top_scope(m->s, m->c);
  cpu_initialize_context(m->s, m->c);
  
  current_machine = m;
  
  return m;
}

void machine_collect(machine m) {
  GPtrArray *roots;
  roots = g_ptr_array_sized_new(NUM_OF_GLOBALS + 10);
  memcpy(roots->pdata, m->s->global, sizeof(struct rubinius_globals));
  roots->len = NUM_OF_GLOBALS;
  cpu_add_roots(m->s, m->c, roots);
  object_memory_collect(m->s->om, roots);
  memcpy(m->s->global, roots->pdata, sizeof(struct rubinius_globals));
  cpu_update_roots(m->s, m->c, roots, NUM_OF_GLOBALS);

  g_ptr_array_free(roots, 0);  
}

OBJECT machine_load_file(machine m, char *path) {
  gchar *data;
  GIOChannel *io;
  GError *err;
  gsize sz;
  
  err = NULL;
  
  io = g_io_channel_new_file(path, "r", &err);
  g_io_channel_set_encoding(io, NULL, &err);
  g_io_channel_read_to_end(io, &data, &sz, &err);
  
  return cpu_unmarshal(m->s, data);
}

int machine_run_file(machine m, char *path) {
  OBJECT meth, name;
  
  meth = machine_load_file(m, path);
  if(!RTEST(meth)) {
    printf("Unable to load '%s'.\n", path);
    return FALSE;
  }
  
  name = string_to_sym(m->s, string_new(m->s, "__script__"));
  cpu_goto_method(m->s, m->c, m->c->main, meth, 0, name);
  cpu_run(m->s, m->c);
  return TRUE;
}

void machine_set_const(machine m, char *str, OBJECT val) {
  OBJECT con, sym;
  con = module_get_constants(m->s->global->object);
  sym = string_to_sym(m->s, string_new(m->s, str));
  hash_set(m->s, con, sym, val);
}

void machine_setup_standard_io(machine m) {
  machine_set_const(m, "STDIN", io_new(m->s, 0));
  machine_set_const(m, "STDOUT", io_new(m->s, 1));
  machine_set_const(m, "STDERR", io_new(m->s, 2));
}

char *_inspect(OBJECT obj) {
  return rbs_inspect(current_machine->s, obj);
}

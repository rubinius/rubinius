#include "shotgun.h"
#include "string.h"
#include "io.h"
#include "hash.h"
#include <unistd.h>
#include <stdlib.h>
#include "machine.h"
#include <string.h>
#include "array.h"
#include <sys/param.h>

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
  
  machine_set_const(m, "MAIN", m->c->main);
  current_machine = m;
  
  return m;
}

void machine_collect(machine m) {
  state_collect(m->s, m->c);
}

void machine_tenure_collect(machine m) {
  m->s->om->tenure_now = 1;
  state_collect(m->s, m->c);
}

void machine_emit_memory(machine m) {
  char *fd;
  FILE *fobj;
  
  fd = getenv("MEMORYFD");
  if(!fd) return;
  
  fobj = fdopen(atoi(fd), "w");
  
  object_memory_emit_details(m->s, m->s->om, fobj);
}

OBJECT machine_load_file(machine m, char *path) {
  return cpu_unmarshal_file(m->s, path);
}

void machine_show_exception(machine m, OBJECT exc) {
  printf("An exception occurred:\n");
  printf(" => %s (%s)\n", string_as_string(m->s, NTH_FIELD(exc, 0)), rbs_inspect(m->s, HEADER(exc)->klass));
}

int machine_run_file(machine m, char *path) {
  OBJECT meth;
  
  meth = machine_load_file(m, path);
  if(!RTEST(meth)) {
    printf("Unable to load '%s'.\n", path);
    return FALSE;
  }
  
  cpu_run_script(m->s, m->c, meth);
  cpu_run(m->s, m->c);
  if(RTEST(m->c->exception)) {
    machine_show_exception(m, m->c->exception);
    return TRUE;
  }
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

void machine_setup_ruby(machine m, char *name) {
  char buf[MAXPATHLEN];
  char wd[MAXPATHLEN];
  /* 
    HACK: this should be replaced by normal ruby code.  
      C sucks - Ryan Davis
  */
  if(name[0] != '/') {
    getcwd(wd, MAXPATHLEN);
    snprintf(buf, MAXPATHLEN, "%s/%s", wd, name);
    name = buf;
  }
  machine_set_const(m, "RUBY_BIN_PATH", string_new(m->s, name));
}

void machine_setup_argv(machine m, int argc, char **argv) {
  OBJECT ary;
  int i;
  
  machine_set_const(m, "ARG0", string_new(m->s, argv[0]));
  
  ary = array_new(m->s, argc - 1);
  for(i = 0; i < argc - 1; i++) {
    array_set(m->s, ary, i, string_new(m->s, argv[i+1]));
  }
  
  machine_set_const(m, "ARGV", ary);
}

extern char **environ;

void machine_config_env(machine m) {
  if(getenv("RDEBUG")) {
    debug_enable();
  }
}

void machine_setup_env(machine m) {
  char *cur, *name, **total;
  OBJECT val, key, hash;
  
  hash = hash_new(m->s);
  int i;
  total = environ;
  do {
    cur = *total++;
    if(!cur) break;
    name = cur;
    i = 0;
    while(*cur && *cur != '=') {
      i++; cur++;
    }
    key = string_new2(m->s, name, i);
    val = string_new(m->s, cur+1);
    hash_set(m->s, hash, key, val);
  } while(cur);
  
  machine_set_const(m, "ENV", hash);
}

char *_inspect(OBJECT obj) {
  if(SYMBOL_P(obj)) {
    return rbs_symbol_to_cstring(current_machine->s, obj);
  }
  return rbs_inspect(current_machine->s, obj);
}

void _class(OBJECT obj) {
  rbs_show_classes(current_machine->s, obj);
}

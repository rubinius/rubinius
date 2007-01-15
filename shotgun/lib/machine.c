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
#include <signal.h>

#include "missing/backtrace.h"

machine current_machine;

static int _recursive_reporting = 0;

#define SYM2STR(st, sym) string_byte_address(st, rbs_symbol_to_string(st, sym))

void machine_print_callstack(machine m) {
  OBJECT context;
  char *modname;
  char *methname;
  char *filename;
  
  context = m->c->active_context;
  
  while(RTEST(context)) {
    if(RTEST(methctx_get_module(context))) {
      modname = SYM2STR(m->s, module_get_name(methctx_get_module(context)));
    } else {
      modname = "<none>";
    }
    
    if(RTEST(methctx_get_name(context))) {
      methname = SYM2STR(m->s, methctx_get_name(context));
    } else {
      methname = "<none>";
    }
    
    if(RTEST(methctx_get_method(context))) {
      filename = SYM2STR(m->s, cmethod_get_file(methctx_get_method(context)));
    } else {
      filename = "<unknown>";
    }
    
    printf("%10p %s#%s+%ld in %s\n",
      (void*)context, modname, methname,
      FIXNUM_TO_INT(methctx_get_ip(context)),
      filename
    );
    context = methctx_get_sender(context);
  }
}

void machine_print_stack(machine m) {
  unsigned int i, start, end;
  
  i = m->c->sp;
  start = (i < 5 ? 0 : i - 5);
  end =   (i + 5 > NUM_FIELDS(m->c->stack) ? NUM_FIELDS(m->c->stack) : i + 5);
  for(i = start; i < end; i++) {
    if(i == m->c->sp) {
      printf("%4d => ", i);
    } else {
      printf("%4d    ", i);
    }
    printf("%s\n", rbs_inspect(m->s, NTH_FIELD(m->c->stack, i)));
  }
  
}

void _machine_error_reporter(int sig, siginfo_t *info, void *ctx) {
  
  char *signame;
  void *bt[128];
  int bt_size;
  
  if(_recursive_reporting) {
    exit(-2);
  }
  
  _recursive_reporting = 1;
  
  switch(sig) {
    case SIGSEGV:
      signame = "Segmentation fault (SIGSEGV)";
      break;
    case SIGBUS:
      signame = "Bus violation (SIGBUS)";
      break;
    case SIGABRT:
      signame = "Software abort (SIGABRT)";
      break;
    default:
      signame = "<UNKNOWN>";
  }
    
  printf("\nAn error has occured: %s\n\n", signame);
  printf("C backtrace:\n");
  bt_size = backtrace(bt, 3);
  
	backtrace_symbols_fd(bt, bt_size, 1);
	printf("\nRuby backtrace:\n");
	machine_print_callstack(current_machine);
	printf("\nRuby stack:\n");
	machine_print_stack(current_machine);
  
  exit(-2);
}

void machine_setup_signals(machine m) {
	m->error_report.sa_sigaction = _machine_error_reporter;
	sigemptyset(&m->error_report.sa_mask);
	m->error_report.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &m->error_report, NULL);
	sigaction(SIGBUS, &m->error_report, NULL);
	sigaction(SIGABRT, &m->error_report, NULL);
}

machine machine_new() {
  machine m;
  m = malloc(sizeof(struct rubinius_machine));
  m->s = rubinius_state_new();
  m->c = cpu_new(m->s);
  machine_setup_signals(m);
  cpu_initialize(m->s, m->c);
  cpu_bootstrap(m->s);
  cpu_setup_top_scope(m->s, m->c);
  cpu_initialize_context(m->s, m->c);
  
  machine_set_const(m, "MAIN", m->c->main);
  current_machine = m;
  
  return m;
}

void state_collect(STATE, cpu c);

void machine_collect(machine m) {
  state_collect(m->s, m->c);
}

void machine_tenure_collect(machine m) {
  m->s->om->tenure_now = 1;
  state_collect(m->s, m->c);
}

/*
void machine_emit_memory(machine m) {
  char *fd;
  FILE *fobj;
  
  fd = getenv("MEMORYFD");
  if(!fd) return;
  
  fobj = fdopen(atoi(fd), "w");
  
  object_memory_emit_details(m->s, m->s->om, fobj);
}
*/

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

void machine_save_args(machine m, int argc, char **argv) {
  char **na;
  na = calloc(argc, sizeof(char*));
  memcpy(na, argv, argc);
  m->argc = argc;
  m->argv = na;
}

void machine_restart_debugging(machine m) {
  char **args;
  
  args = calloc(m->argc + 3, sizeof(char*));
  memcpy(args + 2, m->argv, m->argc);
  args[0] = "gdb";
  args[1] = "--args";
  args[m->argc + 2] = NULL;
  
  execvp("gdb", args);
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

/*
void _class(OBJECT obj) {
  rbs_show_classes(current_machine->s, obj);
}
*/

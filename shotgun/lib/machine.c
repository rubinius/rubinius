#include "shotgun.h"
#include "string.h"
#include "io.h"
#include "hash.h"
#include <unistd.h>
#include <stdlib.h>
#include "machine.h"
#include <string.h>
#include "array.h"
#include "archive.h"
#include <sys/param.h>
#include <signal.h>

#include "config.h"

/* Backtrace support */
#ifdef  __linux__ 
#include <execinfo.h>
#elif defined(__FreeBSD__) 
#include <execinfo.h>
#elif defined(__OpenBSD__) 
#include <execinfo.h>
#endif

/* *BSD dl support */
#ifdef __FreeBSD__
#include <dlfcn.h>
#elif defined(__OpenBSD__)
#include <dlfcn.h>
#endif

#include "subtend.h"
#include "subtend/nmc.h"

machine current_machine;

ucontext_t g_firesuit;
int g_use_firesuit;
int g_access_violation;

static int _recursive_reporting = 0;

#define SYM2STR(st, sym) string_byte_address(st, rbs_symbol_to_string(st, sym))

void machine_print_callstack_limited(machine m, int maxlev);

void machine_print_callstack(machine m) {
    machine_print_callstack_limited(m, -1);
}

void machine_print_callstack_limited(machine m, int maxlev) {
  OBJECT context;
  char *modname;
  char *methname;
  char *filename;
  
  context = m->c->active_context;
  
  while(RTEST(context) && maxlev--) {
    if (NUM_FIELDS(context) == 7) { /* FIXME: print_callstack does not know about block contexts */
        printf("Skipping Block Context\n");
        context = blokctx_get_sender(context);
        continue;
    }
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
  cpu_flush_sp(m->c);
  i = m->c->sp;
  start = (i < 5 ? 0 : i - 5);
  end =   (i + 5 > m->c->stack_size) ? m->c->stack_size : i + 5;
  for(i = start; i < end; i++) {
    if(i == m->c->sp) {
      printf("%4d => ", i);
    } else {
      printf("%4d    ", i);
    }
    printf("%s\n", rbs_inspect_verbose(m->s, m->c->stack_top[i]));
  }
  
}

/* Cleans up the code that uses it for access specific addresses. */
static inline unsigned _mem_at(unsigned long addr) {
  return *(unsigned long*)(addr);
}

#if defined (__i486__)
#define X8632
#endif


#ifdef __ppc__

#include <dlfcn.h>
#include <ucontext.h>
#define BETTER_BT 1

void machine_gather_ppc_frames(ucontext_t *ctx, unsigned long *frames, int *count) {
  unsigned long sp, pc;
  int i;
  
  /* Populate the initial value of the stack pointer from
     the context (probably generated from a signal) */
  sp = ctx->mc.sp;
  for(i = 0; i < *count && sp; i++) {
    /* The address of the routine is stored 8 bytes past
       the stark pointer on PPC. */
    pc = _mem_at(sp + 8);
    /* We subtract 4 since pc points to the next instruction, and
     * we want the current one. */
    frames[i] = pc - 4;
    
    if(sp == (unsigned long)__main_address) break;
    /* The saved stack pointer is the first thing pushed
       on the stack by the caller. */
    sp = _mem_at(sp);
  }
  
  *count = i + 1;
}

void machine_show_ppc_backtrace(ucontext_t *ctx) {
  unsigned long sp, sym, offset;
  Dl_info info;
  
  /* Populate the initial value of the stack pointer from
     the context (probably generated from a signal) */
  sp = ctx->mc.sp;
  while(sp) {
    /* The address of the routine is stored 8 bytes past
       the stark pointer on PPC. */
    sym = _mem_at(sp + 8);
    if(!dladdr((void*)sym, &info)) {
      return;
    } else {
      if (info.dli_sname == NULL)
        info.dli_sname = "???";
      if (info.dli_saddr == NULL) {
        offset = 0;
      } else {
        offset = sym - (unsigned long)info.dli_saddr;        
      }
      
      fprintf(stdout, "%10p <%s+%d> at %s\n",
            (void*)sym, info.dli_sname, (int)offset, info.dli_fname);
      
      if(sp == (unsigned long)__main_address) return;
      /* The saved stack pointer is the first thing pushed
         on the stack by the caller. */
      sp = _mem_at(sp);
    }
  }
}

/* Only for 32bit x86 */
#elif defined(X8632)
#include <dlfcn.h>
#include <ucontext.h>
#define BETTER_BT 1

void machine_gather_x86_frames(ucontext_t *ctx, unsigned long *frames, int *count) {
  unsigned long sp, pc;
  int i;
  
  /* Populate the initial value of the stack pointer.
     Different OS's have different structures for ucontext,
     so we use #ifdef's here to figure out where to get sp from. */
  #ifdef __linux__
  /* sp = ctx->uc_mcontext.gregs[REG_EBP]; */
  sp = 0;
  #elif __APPLE__
  sp = ctx->uc_mcontext.mc_ebp;
  #endif
  
  for(i = 0; i < *count && sp; i++) {
    pc = _mem_at(sp + 4);
    /* We subtract 4 since pc points to the next instruction, and
     * we want the current one. */
    frames[i] = pc - 4;
    
    if(sp == (unsigned long)__main_address) break;
    /* The saved stack pointer is the first thing on the stack at
       the current pointer. */
    sp = _mem_at(sp);
  }
  
  *count = i + 1;
}

#endif

#ifdef __linux__
void machine_resolve_address(unsigned long pc, 
                    char *function, int fs, char *loc, int ls) {
  FILE *io;
  char cmd[1025];

  memset(cmd, 0, 1025);

  snprintf(cmd, 1024, "addr2line -f -e %s %p", current_machine->interpreter, (void*)pc);
  io = popen(cmd, "r");
  fgets(function, fs, io);
  function[strlen(function)-1] = 0;
  fgets(loc, ls, io);
  loc[strlen(loc)-1] = 0;
  pclose(io);
}
#elif defined(__APPLE__)
void machine_resolve_address(unsigned long pc, 
                    char *function, int fs, char *loc, int ls) {
  FILE *io;
  char cmd[1025];
  int line;
  char buf[1025];

  memset(cmd, 0, 1025);

  snprintf(cmd, 1024, "atos -o %s %p", current_machine->interpreter, (void*)pc);
  io = popen(cmd, "r");
  fgets(cmd, 1025, io);
  if(cmd[0] == '0') {
    strncpy(function, "unknown", fs);
    strncpy(loc, "??:0", ls);
  } else {
    line = sscanf(cmd, "_%s (in %s (%s\n", function, buf, loc);
    loc[strlen(loc)-1] = 0;
  }  
  pclose(io);
}
#else

void machine_resolve_address(unsigned long pc,
                  char *function, int fs, char *loc, int ls) {
  int offset;
  Dl_info info;
  if(!dladdr((void*)pc, &info)) {
    strncpy(function, "<unknown>", fs);
    strncpy(loc, "??:0", ls);
  } else {
    if (info.dli_sname == NULL)
      info.dli_sname = "??";
    if (info.dli_saddr == NULL) {
      offset = 0;
    } else {
      offset = pc - (unsigned long)info.dli_saddr;        
    }

    snprintf(function, fs, "%s+%d", info.dli_sname, (int)offset);
    strncpy(loc, info.dli_fname, ls);
  }
}
#endif

void machine_show_backtrace(unsigned long *frames, int count) {
  unsigned long sym;
  int i;
  char function[1025];
  char loc[1025];

  for(i = 0; i < count; i++) {
    memset(function, 0, 1025);
    memset(loc, 0, 1025);
    sym = frames[i];
    machine_resolve_address(sym, function, 1024, loc, 1024);
    fprintf(stdout, "%10p <%s> at %s\n", (void*)sym, function, loc);
  }
}

void machine_print_registers(machine m) {
  cpu_flush_sp(m->c);
  cpu_flush_ip(m->c);
  printf("IP: %04lu     SP: %04lu\n", m->c->ip, m->c->sp);
  printf("AC: %04lu     AR: %04lu\n", m->c->argcount, m->c->args);
  printf("Exception: %s\n", rbs_inspect(m->s, m->c->exception));
}

void _machine_error_reporter(int sig, siginfo_t *info, void *ctx) {
  char *signame;
  rni_context *rni_ctx;
  
  /* See if the error happened during the running of a C function.
     If so, we raise an exception about the error. */
  rni_ctx = subtend_retrieve_context();
  if(rni_ctx->nmc && rni_ctx->nmc->system_set) {
    /* TODO: generate the C backtrace as a string array and pass it
       via the nmc or global_context so that the exception can include
       it. */
    rni_ctx->fault_address = info->si_addr;
    rni_ctx->nmc->jump_val = SEGFAULT_DETECTED;
    setcontext(&rni_ctx->nmc->system);
  }
  
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

  /* This code is kind of dangerous, and general problematic (ie, it cases
     more problems that in solves). So it's disabled unless you really want it. */
  if(getenv("ENABLE_BT")) {
  
    printf("C backtrace:\n");
#ifdef BETTER_BT
    do {
      unsigned long frames[128];
      int count = 128;
#   if defined(__ppc__)
        machine_gather_ppc_frames(ctx, frames, &count);
#   elif defined(X8632)
        machine_gather_x86_frames(ctx, frames, &count);
#   endif
      machine_show_backtrace(frames, count);
    } while(0);
#endif
    printf("\nRuby backtrace:\n");
    machine_print_callstack(current_machine);
  
  }
  
  printf("\nRuby stack:\n");
  machine_print_stack(current_machine);
  printf("\nVM Registers:\n");
  machine_print_registers(current_machine);
  
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
  
  g_use_firesuit = 0;
  g_access_violation = 0;
  
  m = calloc(1, sizeof(struct rubinius_machine));
  m->s = rubinius_state_new();
  m->c = cpu_new(m->s);
  machine_setup_signals(m);
  cpu_initialize(m->s, m->c);
  cpu_bootstrap(m->s);
  subtend_setup(m->s);
  cpu_setup_top_scope(m->s, m->c);
  cpu_initialize_context(m->s, m->c);
  
  machine_set_const(m, "MAIN", m->c->main);
  current_machine = m;
  
  return m;
}

void machine_handle_fire(int kind) {
  g_access_violation = kind;
  setcontext(&g_firesuit);
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
  return cpu_unmarshal_file(m->s, path, 0);
}

void machine_show_exception(machine m, OBJECT exc) {
  printf("\nError: An unhandled exception has terminated this VM.\n");
  printf(" => %s (%s)\n", string_as_string(m->s, NTH_FIELD(exc, 0)), rbs_inspect(m->s, HEADER(exc)->klass));
}

int machine_run(machine m) {
  cpu_run(m->s, m->c);
  if(RTEST(m->c->exception)) {
    machine_show_exception(m, m->c->exception);
    return FALSE;
  }
  return TRUE;
}

int machine_run_file(machine m, char *path) {
  OBJECT meth;
  
  meth = machine_load_file(m, path);
  if(!RTEST(meth)) {
    printf("Unable to load '%s'.\n", path);
    return FALSE;
  }
  
  cpu_run_script(m->s, m->c, meth);
  return machine_run(m);
}

void machine_set_const_under(machine m, char *str, OBJECT val, OBJECT under) {
  OBJECT con, sym;
  con = module_get_constants(under);
  sym = string_to_sym(m->s, string_new(m->s, str));
  hash_set(m->s, con, sym, val);
}

void machine_set_const(machine m, char *str, OBJECT val) {
  machine_set_const_under(m, str, val, m->s->global->object);
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
  m->interpreter = strdup(buf);
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

static void _machine_add_config(gpointer key, gpointer value, gpointer user_data) {
  machine m = (machine)user_data;
  OBJECT ok, ov;
  
  ok = string_new(m->s, (char*)key);
  ov = string_new(m->s, (char*)value);
  hash_set(m->s, m->s->global->config, ok, ov);
}

void machine_parse_config_var(machine m, char *var) {
  char *eq, *or;;
  char buf[1024];
  char *name;
  name = buf;
  int sz;
  eq = strstr(var, "=");
  if(eq) {
    or = var;
    sz = eq - var;
    strncpy(name, var, sz);
    name[sz] = 0;
    var += (sz + 1);
    while(*name == ' ') name++;
    if(m->show_config) {
      printf("[config] '%s' => '%s'\n", name, var);
    }
    g_hash_table_insert(m->s->config, (gpointer)strdup(name), (gpointer)strdup(var));
  } else {
    if(m->show_config) {
      printf("[config] '%s' => '1'\n", var);
    }
    while(*var == ' ') var++;    
    g_hash_table_insert(m->s->config, (gpointer)strdup(var), (gpointer)strdup("1"));
  }
}

void machine_parse_configs(machine m, char *config) {
  char *semi;
  char tmp[1024];
  int sz;
  semi = strstr(config, ";");
  while(semi) {
    sz = semi - config;
    strncpy(tmp, config, sz);
    tmp[sz] = 0;
    machine_parse_config_var(m, tmp);
    config += (sz + 1);
    semi = strstr(config, ";");
  }
  
  machine_parse_config_var(m, config);
  
  m->s->global->config = hash_new(m->s);
  g_hash_table_foreach(m->s->config, _machine_add_config, (gpointer)m);
  machine_set_const(m, "RUBY_CONFIG", m->s->global->config);
}

void machine_setup_from_config(machine m) {
  if(g_hash_table_lookup(m->s->config, "rbx.debug.trace")) {
    m->s->excessive_tracing = 1;
  }
}

void machine_setup_config(machine m) {
  OBJECT mod;
  mod = rbs_module_new(m->s, "Rubinius", m->s->global->object);
  machine_set_const(m, "RUBY_PLATFORM", string_new(m->s, CONFIG_HOST));
  machine_set_const(m, "RUBY_RELEASE_DATE", string_new(m->s, CONFIG_RELDATE));
  machine_set_const_under(m, "VERSION", string_new(m->s, CONFIG_VERSION), mod);
  machine_set_const_under(m, "RUBY_ENGINE", string_new(m->s, CONFIG_ENGINE), mod);
  machine_set_const_under(m, "BUILDREV", string_new(m->s, CONFIG_BUILDREV), mod);
  machine_set_const_under(m, "CODE_PATH", string_new(m->s, CONFIG_CODEPATH), mod);
  machine_set_const_under(m, "EXT_PATH", string_new(m->s, CONFIG_EXTPATH), mod);
  machine_set_const_under(m, "RBA_PATH", string_new(m->s, CONFIG_RBAPATH), mod);
  
  if(isatty(0)) {
    machine_set_const_under(m, "Terminal", string_new(m->s, ttyname(0)), mod);
  } else {
    machine_set_const_under(m, "Terminal", Qfalse, mod);    
  }
}

void machine_config_env(machine m) {
  char *config;
  if(getenv("RDEBUG")) {
    debug_enable();
  }
  
  if(getenv("RBX_CONFIG")) {
    m->show_config = 1;
  }
  
  config = getenv("RBX");
  if(config) {
    machine_parse_configs(m, config); 
  }
}

OBJECT machine_load_archive(machine m, char *path) {
  OBJECT order, cm;
  char *files, *nxt, *top;
  order = archive_get_file(m->s, path, ".load_order.txt");
  if(!RTEST(order)) {
    printf("Unable to find .load_order.txt\n");
    return Qfalse;
  }
  top = files = string_as_string(m->s, order);
  nxt = strchr(files, '\n');
  
  while(nxt) {
    *nxt++ = 0;
    cm = archive_get_object(m->s, path, files, 0);
    if(!RTEST(cm)) {
      printf("Unable to find '%s'\n", files); 
      return Qfalse;
    }
    cpu_run_script(m->s, m->c, cm);
    if(!machine_run(m)) {
      printf("Unable to run '%s'\n", files);
      return Qfalse;
    }
    files = nxt;
    nxt = strchr(nxt, '\n');
  }
  
  free(top);
  
  return Qtrue;
}

char *_inspect(OBJECT obj) {
  if(SYMBOL_P(obj)) {
    return rbs_symbol_to_cstring(current_machine->s, obj);
  }
  return rbs_inspect_verbose(current_machine->s, obj);
}

/*
void _class(OBJECT obj) {
  rbs_show_classes(current_machine->s, obj);
}
*/

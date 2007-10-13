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
#include "symbol.h"
#include "config_hash.h"
#include <sys/stat.h>
#include "config.h"
#include "methctx.h"

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

#include "instruction_names.h"
#include "tuple.h"

machine current_machine;

ucontext_t g_firesuit;
int g_use_firesuit;
int g_access_violation;

static int _recursive_reporting = 0;

static OBJECT string_newfrombstr(STATE, bstring output)
{
	return string_new2(state, (const char*)output->data, output->slen);
}

#define SYM2STR(st, sym) string_byte_address(st, rbs_symbol_to_string(st, sym))

void machine_print_callstack_limited(machine m, int maxlev);

void machine_print_callstack(machine m) {
    machine_print_callstack_limited(m, -1);
}

static int _ip2line(STATE, OBJECT meth, int ip) {
  OBJECT lines, tup;
  int l, total, start, nd, op;

  lines = cmethod_get_lines(meth);
  total = NUM_FIELDS(lines);
  for(l = 0; l < total; l++) {
    tup = tuple_at(state, lines, l);
    start = FIXNUM_TO_INT(tuple_at(state, tup, 0));
    nd = FIXNUM_TO_INT(tuple_at(state, tup, 1));
    op = FIXNUM_TO_INT(tuple_at(state, tup, 2));

    // printf("  %d-%d => %d (%d ?)\n", start, nd, op, ip);

    if(ip >= start && ip <= nd) {
      return op;
    }
  }

  return 0;
}

void machine_print_callstack_limited(machine m, int maxlev) {
  OBJECT context;
  const char *modname, *methname, *filename;
  struct fast_context *fc;

  if(!m) m = current_machine;

  context = m->c->active_context;

  cpu_flush_ip(m->c);
  cpu_flush_sp(m->c);
  
  while(RTEST(context) && maxlev--) {
    methctx_reference(m->s, context);
    fc = FASTCTX(context);

    if(RTEST(fc->method_module)) {
      modname = SYM2STR(m->s, module_get_name(fc->method_module));
    } else {
      modname = "<none>";
    }

    if(fc->type == FASTCTX_BLOCK) {
      methname = "<block>";
    } else if(RTEST(fc->name)) {
      methname = SYM2STR(m->s, fc->name);
    } else {
      methname = "<none>";
    }
    
    if(RTEST(fc->method)) {
      filename = SYM2STR(m->s, cmethod_get_file(fc->method));
    } else {
      filename = "<unknown>";
    }
    
    printf("%10p %s#%s+%d in %s:%d\n",
      (void*)context, modname, methname,
      fc->ip,
      filename,
      _ip2line(m->s, fc->method, fc->ip)
    );
    context = fc->sender;
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
  char cmd[1024];

  snprintf(cmd, sizeof(cmd), "addr2line -f -e %s %p", current_machine->interpreter, (void*)pc);
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
  char cmd[1024];
  int line;
  char buf[1024];

  snprintf(cmd, sizeof(cmd), "atos -o %s %p", current_machine->interpreter, (void*)pc);
  io = popen(cmd, "r");
  fgets(cmd, sizeof(cmd), io);
  if(cmd[0] == '0') {
    strlcpy(function, "unknown", fs);
    strlcpy(loc, "??:0", ls);
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
    strlcpy(function, "<unknown>", fs);
    strlcpy(loc, "??:0", ls);
  } else {
    if (info.dli_sname == NULL)
      info.dli_sname = "??";
    if (info.dli_saddr == NULL) {
      offset = 0;
    } else {
      offset = pc - (unsigned long)info.dli_saddr;        
    }

    snprintf(function, fs, "%s+%d", info.dli_sname, (int)offset);
    strlcpy(loc, info.dli_fname, ls);
  }
}
#endif

void machine_show_backtrace(unsigned long *frames, int count) {
  unsigned long sym;
  int i;
  char function[1024];
  char loc[1024];

  for(i = 0; i < count; i++) {
    sym = frames[i];
    machine_resolve_address(sym, function, sizeof(function), loc, sizeof(loc));
    fprintf(stdout, "%10p <%s> at %s\n", (void*)sym, function, loc);
  }
}

void machine_print_registers(machine m) {
  cpu_flush_sp(m->c);
  cpu_flush_ip(m->c);
  printf("IP: %04d      SP: %04d\n", m->c->ip, m->c->sp);
  printf("AC: %04d      AR: %04lu\n", cpu_current_argcount(m->c), m->c->args);
  printf("Exception: %s\n", rbs_inspect(m->s, m->c->exception));
}

void _machine_error_reporter(int sig, siginfo_t *info, void *ctx) {
  const char *signame;
  rni_context *rni_ctx;
  OBJECT addr;
  
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
  
  /* This is really nice. We don't have to do this check at every
     fetch, instead, let it segfault and handle it here. 
     The check for - 4 is because the bounds checks grabs the number
     of fields from a ref right away, which is where it will segfault
     if it's not a ref. The fields are 4 bytes into the header.
     The check for - 8 is because it's common this happens when
     trying to grab the class of a non-reference. The class is
     8 bytes into the header. */
  addr = (OBJECT)(info->si_addr);
  if(!REFERENCE_P(addr) || !REFERENCE_P(addr - 4) || !REFERENCE_P(addr - 8)) {
    printf("Attempted to access field of non reference.\n");
    if(g_use_firesuit) {
      machine_handle_fire(FIRE_NULL);
    }
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

  if(getenv("CRASH_WAIT")) {
    printf("Pausing so I can be debugged.\n");
    pause();
    printf("Continuing after debugger.\n");
  }

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
    // printf("\nRuby backtrace:\n");
    // machine_print_callstack(current_machine);
  
  }
  
  // printf("\nRuby stack:\n");
  // machine_print_stack(current_machine);
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
  /* Initialize the instruction addresses. */
  cpu_run(m->s, m->c, TRUE);
  machine_setup_signals(m);
  cpu_initialize(m->s, m->c);
  cpu_bootstrap(m->s);
  subtend_setup(m->s);
  cpu_setup_top_scope(m->s, m->c);
  cpu_initialize_context(m->s, m->c);
  
  machine_set_const(m, "MAIN", m->c->main);
  current_machine = m;
  
  cpu_task_configure_premption(m->s);
  
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

OBJECT machine_load_file(machine m, const char *path) {
  return cpu_unmarshal_file(m->s, path, 0);
}

void machine_show_exception(machine m, OBJECT exc) {
  OBJECT msg;
  char *buf;
  printf("\nError: An unhandled exception has terminated this VM.\n");
  msg = exception_get_message(exc);
  if(REFERENCE_P(msg)) {
    buf = string_byte_address(m->s, msg);
  } else {
    buf = "<no message>";
  }
  printf(" => %s (%s)\n", buf, rbs_inspect(m->s, HEADER(exc)->klass));
}

int machine_run(machine m) {
  cpu_run(m->s, m->c, 0);
  if(RTEST(m->c->exception)) {
    printf("Toplevel exception detected.\n");
    machine_show_exception(m, m->c->exception);
    return FALSE;
  }
  return TRUE;
}

int machine_run_file(machine m, const char *path) {
  OBJECT meth;
  int out;
  
  if(m->s->excessive_tracing) {
    printf("[ Loading file %s]\n", path);
  }
  
  meth = machine_load_file(m, path);
  if(!RTEST(meth)) {
    printf("Unable to load '%s'.\n", path);
    return FALSE;
  }
  
  cpu_stack_push(m->s, m->c, meth, FALSE);
  cpu_run_script(m->s, m->c, meth);
  out = machine_run(m);
  if(m->s->excessive_tracing) {
    printf("[ Finished loading file %s]\n", path);
  }
  return out;
}

void machine_set_const_under(machine m, const char *str, OBJECT val, OBJECT under) {
  OBJECT con, sym;
  con = module_get_constants(under);
  sym = string_to_sym(m->s, string_new(m->s, str));
  hash_set(m->s, con, sym, val);
}

void machine_set_const(machine m, const char *str, OBJECT val) {
  machine_set_const_under(m, str, val, m->s->global->object);
}

void machine_save_args(machine m, int argc, char **argv) {
  char **na;
  na = calloc(argc, sizeof(char*));
  memcpy(na, argv, argc);
  m->argc = argc;
  m->argv = na;
}

void machine_setup_standard_io(machine m) {
  machine_set_const(m, "STDIN", io_new(m->s, 0, "r"));
  machine_set_const(m, "STDOUT", io_new(m->s, 1, "w"));
  machine_set_const(m, "STDERR", io_new(m->s, 2, "w"));
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
  m->interpreter = strdup(name);
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

static void machine_parse_config_var(machine m, char *var) {
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
    ht_config_insert(m->s->config, cstr2bstr(name), cstr2bstr(var));
  } else {
    if(m->show_config) {
      printf("[config] '%s' => '1'\n", var);
    }
    while(*var == ' ') var++;    
    ht_config_insert(m->s->config, cstr2bstr(var), cstr2bstr("1"));
  }
}

static void machine_parse_configs(machine m, char *config) {
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
  
  struct hashtable_itr *iter = hashtable_iterator(m->s->config);
  do
    {
      bstring k = (bstring) hashtable_iterator_key(iter);
      bstring v = (bstring) hashtable_iterator_value(iter);
      OBJECT ok = string_newfrombstr(m->s, k);
      OBJECT ov = string_newfrombstr(m->s, v);

      hash_set(m->s, m->s->global->config, ok, ov);
    }
  while (hashtable_iterator_advance(iter));
  free(iter);
  machine_set_const(m, "RUBY_CONFIG", m->s->global->config);
}

void machine_setup_from_config(machine m) {
  if(ht_config_search(m->s->config, cstr2bstr("rbx.debug.trace"))) {
    m->s->excessive_tracing = 1;
  }
  
  if(ht_config_search(m->s->config, cstr2bstr("rbx.debug.gc"))) {
    m->s->gc_stats = 1;
  }
}

void machine_setup_config(machine m) {
  OBJECT mod;
  STATE;
  
  state = m->s;
  
  mod = rbs_module_new(m->s, "Rubinius", m->s->global->object);
  machine_set_const(m, "RUBY_PLATFORM", string_new(m->s, CONFIG_HOST));
  machine_set_const(m, "RUBY_RELEASE_DATE", string_new(m->s, CONFIG_RELDATE));
  machine_set_const_under(m, "RBX_VERSION", string_new(m->s, CONFIG_VERSION), mod);
  machine_set_const_under(m, "RUBY_VERSION", string_new(m->s, CONFIG_RUBY_VERSION), mod);
  machine_set_const_under(m, "VERSION", string_new(m->s, CONFIG_RUBY_VERSION), mod);
  machine_set_const_under(m, "RUBY_ENGINE", string_new(m->s, CONFIG_ENGINE), mod);
  machine_set_const_under(m, "BUILDREV", string_new(m->s, CONFIG_BUILDREV), mod);
  machine_set_const_under(m, "CODE_PATH", string_new(m->s, CONFIG_CODEPATH), mod);
  machine_set_const_under(m, "EXT_PATH", string_new(m->s, CONFIG_EXTPATH), mod);
  machine_set_const_under(m, "RBA_PATH", string_new(m->s, CONFIG_RBAPATH), mod);
  
#if defined(__ppc__) || defined(__POWERPC__) || defined(_POWER)
  machine_set_const_under(m, "PLATFORM", SYM("ppc"), mod);
#elif defined(__amd64__)
  machine_set_const_under(m, "PLATFORM", SYM("amd64"), mod);
#elif defined(i386) || defined(__i386__)
  machine_set_const_under(m, "PLATFORM", SYM("x86"), mod);
#elif defined(__alpha) || defined(__alpha__)
  machine_set_const_under(m, "PLATFORM", SYM("alpha"), mod);
#elif defined(VAX) || defined(__VAX)
  machine_set_const_under(m, "PLATFORM", SYM("vax"), mod);
#elif defined(__hppa__)
  machine_set_const_under(m, "PLATFORM", SYM("hppa"), mod);
#elif defined(__sparc__)
  machine_set_const_under(m, "PLATFORM", SYM("sparc"), mod);
#elif defined(__s390__)
  machine_set_const_under(m, "PLATFORM", SYM("s390"), mod);
#elif (defined(TARGET_CPU_68K) || defined(__CFM68K__) || defined(m68k) || defined(_M_M68K))
  machine_set_const_under(m, "PLATFORM", SYM("m68k"), mod);
#else
  machine_set_const_under(m, "PLATFORM", SYM("unknown"), mod);
#endif
  
#if defined(__APPLE__) || defined(__MACH__)
  machine_set_const_under(m, "OS", SYM("darwin"), mod);
#elif defined(__linux__) || defined(linux) || defined(__linux)
  machine_set_const_under(m, "OS", SYM("linux"), mod);
#elif defined(__FreeBSD__)
  machine_set_const_under(m, "OS", SYM("freebsd"), mod);
#elif defined(__CYGWIN__)
  machine_set_const_under(m, "OS", SYM("cygwin"), mod);
#elif defined(__OS2__)
  machine_set_const_under(m, "OS", SYM("os2"), mod);
#elif defined(__NT__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  machine_set_const_under(m, "OS", SYM("win32"), mod);
#elif defined(__WINDOWS__)
  machine_set_const_under(m, "OS", SYM("windows_3x"), mod);
#elif defined(__NETWARE_386__)
  machine_set_const_under(m, "OS", SYM("netware"), mod);
#elif defined(__MSDOS__)
  machine_set_const_under(m, "OS", SYM("dos"), mod);
#elif defined(VMS) || defined(__VMS__)
  machine_set_const_under(m, "OS", SYM("vms"), mod);
#elif defined(__hpux__)
  machine_set_const_under(m, "OS", SYM("hpux"), mod);
#elif defined(__sun__) || defined(__sun)
  machine_set_const_under(m, "OS", SYM("solarce"), mod);
#elif defined(__svr4__)
  machine_set_const_under(m, "OS", SYM("unixware"), mod);
#elif defined(_AIX)
  machine_set_const_under(m, "OS", SYM("aix"), mod);
#elif (defined(_SCO_DS) && defined(_SCO_ELF) && defined(_SCO_XPG_VERS) && defined(_SCO_C_DIALECT))
  machine_set_const_under(m, "OS", SYM("openserver"), mod);
#elif defined(__unix__)
  machine_set_const_under(m, "OS", SYM("decunix"), mod);
#else
  machine_set_const_under(m, "OS", SYM("unknown"), mod);
#endif

#if defined(__VERSION__)
  machine_set_const_under(m, "COMPILER_VERSION", string_new(m->s, __VERSION__), mod);
#else
  machine_set_const_under(m, "COMPILER_VERSION", Qnil, mod);
#endif

#if defined(_MSC_VER)
  machine_set_const_under(m, "COMPILER", SYM("microsoft"), mod);  
#elif defined(__DECC) || defined(VAXC)
  machine_set_const_under(m, "COMPILER", SYM("digital"), mod);  
#elif defined(__BORLANDC__)
  machine_set_const_under(m, "COMPILER", SYM("borland"), mod);
#elif defined(__WATCOMC__)
  machine_set_const_under(m, "COMPILER", SYM("watcom"), mod);
#elif defined(__GNUC__)
  machine_set_const_under(m, "COMPILER", SYM("gcc"), mod);
#elif defined(__MWERKS__)
  machine_set_const_under(m, "COMPILER", SYM("metrowerks"), mod);
#elif defined(__IBMC__) || defined(__IBMCPP__)
  machine_set_const_under(m, "COMPILER", SYM("ibm"), mod);
#elif defined(__SUNPRO_C)
  machine_set_const_under(m, "COMPILER", SYM("sunpro"), mod);
#else
  machine_set_const_under(m, "COMPILER", SYM("unknown"), mod);
#endif

#if defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
  machine_set_const_under(m, "ENDIAN", SYM("big"), mod);
#else
  machine_set_const_under(m, "ENDIAN", SYM("little"), mod);
#endif

#if defined(_WIN32) || defined(__NT__) || defined(WIN32) || defined(__WIN32__)
#define LIBSUFFIX "dll"
#elif defined(__APPLE__)
#define LIBSUFFIX "bundle"
#else
#define LIBSUFFIX "so"
#endif

  machine_set_const_under(m, "LIBSUFFIX", string_new(m->s, LIBSUFFIX), mod);
  machine_set_const_under(m, "COMPILER_PATH", string_new(m->s, CONFIG_CC), mod);
  
  if(isatty(0)) {
    machine_set_const_under(m, "Terminal", string_new(m->s, ttyname(0)), mod);
  } else {
    machine_set_const_under(m, "Terminal", Qfalse, mod);    
  }
  
  machine_set_const_under(m, "DEBUG_INST", I2N(CPU_INSTRUCTION_YIELD_DEBUGGER), mod);
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
  } else {
    machine_parse_configs(m, "");
  }
}

int machine_load_directory(machine m, const char *prefix) {
  char *path;
  char *file;
  FILE *fp;
  int len;
  path = malloc(1024);
  sprintf(path, "%s/.load_order.txt", prefix);
  
  fp = fopen(path, "r");
  if(!fp) {
    printf("Unable to open directory '%s'\n", prefix);
    free(path);
    return FALSE;
  }
  
  file = malloc(1024);
  while(!feof(fp)) {
    fgets(file, 1024, fp);
    /* Get rid of the \n on the end. */
    len = strlen(file);
    if(file[len-1] == '\n') file[len-1] = 0;
    snprintf(path, 1024, "%s/%s", prefix, file);
    if(!machine_run_file(m, path)) {
      free(file);
      free(path);
      return FALSE;
    }
  }
  
  free(file);
  free(path);
  
  return TRUE;
}

OBJECT machine_load_archive(machine m, const char *path) {
  OBJECT order, cm, ret = Qfalse;
  archive_handle_t *archive;
  char *files, *nxt, *top;
  
  if(m->s->excessive_tracing) {
    printf("[ Loading archive %s]\n", path);
  }

  archive = archive_open(m->s, path);
  if(!archive) {
    fprintf(stderr, "cannot load archive '%s'\n", path);
    return ret;
  }

  order = archive_get_file2(m->s, archive, ".load_order.txt");
  if(!RTEST(order)) {
    printf("Unable to find .load_order.txt\n");
    goto out;
  }
  top = files = strdup(string_byte_address(m->s, order));
  nxt = strchr(files, '\n');
  
  while(nxt) {
    *nxt++ = 0;
    cm = archive_get_object2(m->s, archive, files, 0);
    if(!RTEST(cm)) {
      printf("Unable to find '%s'\n", files); 
      goto out;
    }
    if(m->s->excessive_tracing) {
      printf("[ Loading file %s]\n", files);
    }
    /* We push this on the stack so it's properly seen by the GCs */
    cpu_stack_push(m->s, m->c, cm, FALSE);
    cpu_run_script(m->s, m->c, cm);
    if(!machine_run(m)) {
      printf("Unable to run '%s'\n", files);
      ret = Qfalse;
      goto out;
    }
    /* Pop the scripts return value. */
    (void)cpu_stack_pop(m->s, m->c);
    /* Pop the script object. */
    (void)cpu_stack_pop(m->s, m->c);
    files = nxt;
    nxt = strchr(nxt, '\n');
  }
  
  free(top);
  ret = Qtrue;

out:
  archive_close(m->s, archive);
  if(m->s->excessive_tracing) {
    printf("[ Finished loading archive %s]\n", path);
  }
  
  return ret;
}

int machine_load_bundle(machine m, const char *path) {
  struct stat sb;
  OBJECT ret;
  if(stat(path, &sb) != 0) return FALSE;
  
  if(S_ISDIR(sb.st_mode)) {
    return machine_load_directory(m, path);
  }
  
  ret = machine_load_archive(m, path);
  if(!TRUE_P(ret)) return FALSE;
  return TRUE;
}


/* Debugging functions */
void _print_stack(int cnt, int start) {
  OBJECT *stk;
  int i;
  
  stk = current_machine->c->sp_ptr;
  printf("Current Stack: %p\n", stk);
  
  if(start < 0) {
    stk -= start;    
    for(i = start; i < 0; i++) {
      printf("  %3d:\t%s\t\t\t%p\n", i, _inspect(*stk), stk);
      stk--;
    }
  }
  
  stk = current_machine->c->sp_ptr;
  
  printf(">   0:\t%s\n", _inspect(*stk--));  
  
  for(i = 1; i <= cnt; i++) {
    printf("  %3d:\t%s\n", i, _inspect(*stk));
    stk--;
  }
}

const char *_inspect(OBJECT obj) {
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

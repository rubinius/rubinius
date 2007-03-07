#ifndef __CPU_H_
#define __CPU_H_

#include <glib.h>

/* Configuration macros. */

/* Enables use of FastMethodContext objects. */
#define CTX_USE_FAST 1

/* Enables the context cache. */
#define CTX_CACHE_ENABLED 0

#define CPU_REGISTERS OBJECT sender; \
  unsigned long int ip; \
  unsigned long int sp; \
  OBJECT block; \
  unsigned long int raiseable; \
  OBJECT method; \
  unsigned char *data; \
  unsigned long int data_size; \
  OBJECT literals; \
  OBJECT self; \
  OBJECT locals; \
  unsigned long int argcount; \
  OBJECT name; \
  OBJECT method_module; \
  unsigned long int num_locals; \
  long type; \
  void *opaque_data;

struct fast_context {
  CPU_REGISTERS
};

#define FASTCTX_FIELDS 21
#define FASTCTX_NORMAL 0
#define FASTCTX_NMC    1

struct rubinius_cpu {
  CPU_REGISTERS
  
  char buffer[20];
  
  /* Below here, the cpu registers are not saved and restored
     per call. */
  
  long int args;
  long int cache_index;
  
  OBJECT *stack_top;
  unsigned long int stack_size;
  OBJECT exception;
  OBJECT enclosing_class;
  OBJECT new_class_of;
  OBJECT exceptions;
  OBJECT top_context;

  OBJECT active_context, home_context, main;
  
  GPtrArray *paths;
  unsigned int depth;
  
  OBJECT context_cache;
  
  unsigned char *ip_ptr;
  OBJECT *sp_ptr;
};

typedef struct rubinius_cpu *cpu;

#define cpu_stack_empty_p(state, cpu) (cpu->sp_ptr <= cpu->stack_top)
#define cpu_local_get(state, cpu, idx) (NTH_FIELD(cpu->locals, idx))
#define cpu_local_set(state, cpu, idx, obj) (SET_FIELD(cpu->locals, idx, obj))

#define stack_push(obj) cpu_stack_push(state, c, obj, FALSE)
#define stack_pop() cpu_stack_pop(state, c)
#define stack_top() cpu_stack_top(state, c)

#define cpu_flush_ip(cpu) (cpu->ip = (cpu->ip_ptr - cpu->data))
#define cpu_flush_sp(cpu) (cpu->sp = (cpu->sp_ptr - cpu->stack_top))

#define cpu_cache_ip(cpu) (cpu->ip_ptr = (cpu->data + cpu->ip))
#define cpu_cache_sp(cpu) (cpu->sp_ptr = (cpu->stack_top + cpu->sp))

cpu cpu_new(STATE);
void cpu_initialize(STATE, cpu c);
void cpu_setup_top_scope(STATE, cpu c);
void cpu_initialize_context(STATE, cpu c);
void cpu_update_roots(STATE, cpu c, GPtrArray *roots, int start);
inline void cpu_activate_context(STATE, cpu c, OBJECT ctx, OBJECT home);
inline void cpu_return_to_sender(STATE, cpu c, int consider_block);
OBJECT cpu_const_get(STATE, cpu c, OBJECT sym, OBJECT under);
OBJECT cpu_const_set(STATE, cpu c, OBJECT sym, OBJECT val, OBJECT under);
void cpu_run(STATE, cpu c);
int cpu_dispatch(STATE, cpu c);
void cpu_set_encloser_path(STATE, cpu c, OBJECT cls);
void cpu_push_encloser(STATE, cpu c);
void cpu_add_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method);
void cpu_attach_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method);
void cpu_raise_exception(STATE, cpu c, OBJECT exc);
void cpu_raise_arg_error(STATE, cpu c, int args, int req);
OBJECT cpu_new_exception(STATE, cpu c, OBJECT klass, char *msg);
inline void cpu_perform_hook(STATE, cpu c, OBJECT recv, OBJECT meth, OBJECT arg);

inline void cpu_goto_method(STATE, cpu c, OBJECT recv, OBJECT meth,
                                     int count, OBJECT name);

void cpu_send_method(STATE, cpu c, OBJECT recv, OBJECT sym, int args);


void cpu_run_script(STATE, cpu c, OBJECT meth);
inline void cpu_save_registers(STATE, cpu c);

OBJECT exported_cpu_find_method(STATE, cpu c, OBJECT klass, OBJECT name, OBJECT *mod);

OBJECT cpu_unmarshal(STATE, char *str);
OBJECT cpu_marshal(STATE, OBJECT obj);
OBJECT cpu_unmarshal_file(STATE, char *path);
GString *cpu_marshal_to_gstring(STATE, OBJECT obj);
OBJECT cpu_marshal_to_file(STATE, OBJECT obj, char *path);

void cpu_bootstrap(STATE);
void cpu_add_roots(STATE, cpu c, GPtrArray *roots);
void cpu_update_roots(STATE, cpu c, GPtrArray *roots, int start);

/* Method cache functions */
void cpu_clear_cache(STATE, cpu c);
void cpu_clear_cache_for_method(STATE, cpu c, OBJECT meth);
void cpu_clear_cache_for_class(STATE, cpu c, OBJECT klass);

#if 1

#define cpu_stack_push(state, c, oop, check) ({ OBJECT _tmp = (oop); (c)->sp_ptr++; *((c)->sp_ptr) = _tmp; })
#define cpu_stack_pop(state, c) (*(c)->sp_ptr--)
#define cpu_stack_top(state, c) (*(c)->sp_ptr)

#else 
static inline int cpu_stack_push(STATE, cpu c, OBJECT oop, int check) {
  c->sp += 1;
#if 0
  if(check) {
    if(NUM_FIELDS(c->stack) <= c->sp) {
      return FALSE;
    }
  }
#endif
  SET_FIELD(c->stack, c->sp, oop);
  return TRUE;
}

static inline OBJECT cpu_stack_pop(STATE, cpu c) {
  OBJECT obj;
  obj = NTH_FIELD(c->stack, c->sp);
  c->sp -= 1;
  return obj;
}

static inline OBJECT cpu_stack_top(STATE, cpu c) {
  return NTH_FIELD(c->stack, c->sp);
}

#endif

#define MAX_SYSTEM_PRIM 2048

int cpu_perform_system_primitive(STATE, cpu c, int prim, OBJECT mo, int num_args, OBJECT name, OBJECT mod);

static inline int cpu_perform_primitive(STATE, cpu c, int prim, OBJECT mo, int args, OBJECT name, OBJECT mod) {
  if(prim < MAX_SYSTEM_PRIM) {
    return cpu_perform_system_primitive(state, c, prim, mo, args, name, mod);
  } else {
    printf("Error: Primitive index out of range for this VM\n");
    abort();
  }
}



#endif /* __CPU_H_ */

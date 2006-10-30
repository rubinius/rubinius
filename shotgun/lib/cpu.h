#include <glib.h>

struct rubinius_cpu {
  OBJECT stack;
  OBJECT self;
  OBJECT exception;
  OBJECT enclosing_class;
  OBJECT new_class_of;
  OBJECT locals, literals;
  OBJECT block, method;
  OBJECT active_context, home_context, main;
  OBJECT exceptions;
  OBJECT top_context;
  int sp;
  int ip;
  int argcount, args, depth;
  char *data;
  int data_size;
  
  GPtrArray *paths;
};

typedef struct rubinius_cpu *cpu;

#define cpu_stack_empty_p(state, cpu) (cpu->sp == -1)
#define cpu_local_get(state, cpu, idx) (NTH_FIELD(cpu->locals, idx))
#define cpu_local_set(state, cpu, idx, obj) (SET_FIELD(cpu->locals, idx, obj))

#define stack_push(obj) cpu_stack_push(state, c, obj)
#define stack_pop() cpu_stack_pop(state, c)
#define stack_top() cpu_stack_top(state, c)

cpu cpu_new(STATE);
void cpu_initialize(STATE, cpu c);
void cpu_setup_top_scope(STATE, cpu c);
void cpu_initialize_context(STATE, cpu c);
void cpu_update_roots(STATE, cpu c, GPtrArray *roots, int start);
void cpu_save_registers(STATE, cpu c);
void cpu_restore_context(STATE, cpu c, OBJECT x);
void cpu_restore_context_with_home(STATE, cpu c, OBJECT ctx, OBJECT home, int ret);
void cpu_activate_context(STATE, cpu c, OBJECT ctx, OBJECT home);
void cpu_return_to_sender(STATE, cpu c, int consider_block);
OBJECT cpu_const_get(STATE, cpu c, OBJECT sym, OBJECT under);
OBJECT cpu_const_set(STATE, cpu c, OBJECT sym, OBJECT val, OBJECT under);
void cpu_run(STATE, cpu c);
void cpu_stack_push(STATE, cpu c, OBJECT oop);
OBJECT cpu_stack_pop(STATE, cpu c);
OBJECT cpu_stack_top(STATE, cpu c);
int cpu_dispatch(STATE, cpu c);
void cpu_set_encloser_path(STATE, cpu c, OBJECT cls);
void cpu_push_encloser(STATE, cpu c);
void cpu_add_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method);
void cpu_attach_method(STATE, cpu c, OBJECT target, OBJECT sym, OBJECT method);
int cpu_perform_primitive(STATE, cpu c, int prim, OBJECT mo);
void cpu_raise_exception(STATE, cpu c, OBJECT exc);
OBJECT cpu_new_exception(STATE, cpu c, OBJECT klass, char *msg);
inline void cpu_perform_hook(STATE, cpu c, OBJECT recv, OBJECT meth, OBJECT arg);

inline void cpu_goto_method(STATE, cpu c, OBJECT recv, OBJECT meth,
                                     int count, OBJECT name);

void cpu_run_script(STATE, cpu c, OBJECT meth);

OBJECT cpu_unmarshal(STATE, char *str);
GString *cpu_marshal(STATE, OBJECT obj);
OBJECT cpu_unmarshal_file(STATE, char *path);

void cpu_bootstrap(STATE);
void cpu_add_roots(STATE, cpu c, GPtrArray *roots);
void cpu_update_roots(STATE, cpu c, GPtrArray *roots, int start);

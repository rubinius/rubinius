#include "shotgun.h"
#include "cpu.h"
#include "machine.h"
#include <stdlib.h>
#include "tuple.h"
#include "methctx.h"
#include "object.h"
#include "bytearray.h"
#include "string.h"
#include "class.h"
#include "hash.h"
#include "symbol.h"
#include <glib.h>

#include "rubinius.h"

void cpu_task_cleanup(STATE, OBJECT self) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  
  free(task->stack_top);
}

void cpu_task_init(STATE) {
  state_add_cleanup(state, state->global->task, cpu_task_cleanup);
}

OBJECT cpu_task_dup(STATE, cpu c, OBJECT cur) {
  struct cpu_task *cur_task, *task;
  
  OBJECT obj;
  
  if(NIL_P(cur)) {
    cur_task = (struct cpu_task*)CPU_TASKS_LOCATION(c);
  } else {
    cur_task = (struct cpu_task*)BYTES_OF(cur);
  }
  
  NEW_STRUCT(obj, task, state->global->task, struct cpu_task);
  memcpy(task, cur_task, sizeof(struct cpu_task));
  
  task->stack_top = (OBJECT*)calloc(InitialStackSize, sizeof(OBJECT));
  task->sp_ptr = task->stack_top;
  task->stack_size = InitialStackSize;
  
  return obj;
}

void cpu_task_swap(STATE, cpu c, OBJECT cur, OBJECT nw) {
  struct cpu_task *cur_task, *new_task, *ct;
  OBJECT home;
  cpu_save_registers(state, c);
  
  ct = (struct cpu_task*)CPU_TASKS_LOCATION(c);
  cur_task = (struct cpu_task*)BYTES_OF(cur);
  new_task = (struct cpu_task*)BYTES_OF(nw);
  
  memcpy(cur_task, ct, sizeof(struct cpu_task));
  memcpy(ct, new_task, sizeof(struct cpu_task));
  
  home = NIL_P(c->home_context) ? c->active_context : c->home_context;
  
  cpu_restore_context_with_home(state, c, c->active_context, home, FALSE, FALSE);
  
  c->current_task = nw;
}

OBJECT cpu_task_associate(STATE, OBJECT self, OBJECT be) {
  OBJECT bc;
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  
  bc = blokenv_create_context(state, be, Qnil, 0);
  
  task->main = bc;
  task->active_context = bc;
  task->home_context = blokenv_get_home(be);
  
  return bc;
}

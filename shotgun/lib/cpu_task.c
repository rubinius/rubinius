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
#include "list.h"
#include <glib.h>

#include "rubinius.h"

void cpu_task_cleanup(STATE, OBJECT self) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  
  if(!task->stack_slave) {
    free(task->stack_top);
  }
}

void Init_cpu_task(STATE) {
  OBJECT tup;
  state_add_cleanup(state, state->global->task, cpu_task_cleanup);
  
  tup = tuple_new2(state, 7, list_new(state), list_new(state), list_new(state),
    list_new(state), list_new(state), list_new(state), list_new(state));
  
  state->global->scheduled_threads = tup;
  rbs_const_set(state, BASIC_CLASS(task), "ScheduledThreads", tup);
  
  BASIC_CLASS(channel) = rbs_class_new(state, "Channel", 2, BASIC_CLASS(object));
  BASIC_CLASS(thread) =  rbs_class_new(state, "Thread", 4, BASIC_CLASS(object));
  class_set_instance_flags(BASIC_CLASS(thread), I2N(0x02)); /* ivars allowed. */
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
  task->stack_slave = 1;
  
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
  task->stack_top = (OBJECT*)calloc(InitialStackSize, sizeof(OBJECT));
  task->sp_ptr = task->stack_top;
  task->stack_size = InitialStackSize;
  task->stack_slave = 0;
  
  task->main = bc;
  task->active_context = bc;
  task->home_context = blokenv_get_home(be);
  
  return bc;
}

void cpu_task_push(STATE, OBJECT self, OBJECT val) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  task->sp_ptr++;
  *(task->sp_ptr) = val;
}

void cpu_task_set_top(STATE, OBJECT self, OBJECT val) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  *(task->sp_ptr) = val;
}

OBJECT cpu_task_pop(STATE, OBJECT self) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  return *task->sp_ptr--;
}

OBJECT cpu_task_top(STATE, OBJECT self) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  return *task->sp_ptr;
}


#define thread_set_priority(obj, val) SET_FIELD(obj, 1, val)
#define thread_set_task(obj, val) SET_FIELD(obj, 2, val)
#define thread_set_joins(obj, val) SET_FIELD(obj, 3, val)

#define thread_get_priority(obj) NTH_FIELD(obj, 1)
#define thread_get_task(obj) NTH_FIELD(obj, 2)
#define thread_get_joins(obj) NTH_FIELD(obj, 3)

OBJECT cpu_thread_new(STATE, cpu c) {
  OBJECT thr;
  
  thr = rbs_class_new_instance(state, BASIC_CLASS(thread));
  if(c->current_thread) {
    thread_set_priority(thr, thread_get_priority(c->current_thread));
  } else {
    thread_set_priority(thr, I2N(2));
  }
  thread_set_task(thr, cpu_task_dup(state, c, Qnil));
  thread_set_joins(thr, list_new(state));
  return thr;
}

OBJECT cpu_thread_get_task(STATE, OBJECT self) {
  return thread_get_task(self);
}

void cpu_thread_schedule(STATE, OBJECT self) {
  long int prio, rprio;
  OBJECT lst;
  
  prio = FIXNUM_TO_INT(thread_get_priority(self));
  
  if(prio < 1) { 
    rprio = 1;
  } else if(prio > 7) {
    rprio = 7;
  } else {
    rprio = prio;
  }
  
  rprio--;
  lst = tuple_at(state, state->global->scheduled_threads, rprio);
  list_append(state, lst, self);
}

OBJECT cpu_thread_find_highest(STATE) {
  int i, t;
  OBJECT lst, tup;
  
  tup = state->global->scheduled_threads;
  t = NUM_FIELDS(tup);
  for(i = t - 1; i >= 0; i--) {
    lst = tuple_at(state, tup, i);
    if(FIXNUM_TO_INT(list_get_count(lst)) != 0) {
      return list_shift(state, lst);
    }
  }
  printf("DEADLOCK!\n");
  abort();
}

void cpu_thread_switch(STATE, cpu c, OBJECT thr) {
  OBJECT task;
  
  /* Save the current task back into the current thread, in case
     Task's were used inside the thread itself (not just for the thread). */
  thread_set_task(c->current_thread, c->current_task);
  task = thread_get_task(thr);
  cpu_task_swap(state, c, c->current_task, task);
  c->current_thread = thr;
}

void cpu_thread_run_best(STATE, cpu c) {
  OBJECT thr;
  
  thr = cpu_thread_find_highest(state);
  cpu_thread_switch(state, c, thr);
}

#define channel_set_waiting(obj, val) SET_FIELD(obj, 0, val)
#define channel_get_waiting(obj) NTH_FIELD(obj, 0)
#define channel_set_value(obj, val) SET_FIELD(obj, 1, val)
#define channel_get_value(obj) NTH_FIELD(obj, 1)

OBJECT cpu_channel_new(STATE) {
  OBJECT chan;
  chan = rbs_class_new_instance(state, BASIC_CLASS(channel));
  channel_set_waiting(chan, list_new(state));
  return chan;
}

OBJECT cpu_channel_send(STATE, cpu c, OBJECT self, OBJECT obj) {
  OBJECT lst, thr, task;
  long int cur_prio, new_prio;
  
  /* Sort of like setjmp, don't allow nil to be sent, because nil means there
     is no value. I could use undef here, but I still don't like exposing
     undef. */
  
  if(NIL_P(obj)) obj = Qtrue;
  lst = channel_get_waiting(self);
  
  if(list_empty_p(lst)) {
    channel_set_value(self, obj);
  } else {
    thr = list_shift(state, lst);
    task = thread_get_task(thr);
    cpu_task_set_top(state, task, obj);
    /* If we're resuming a thread thats of higher priority than we are, 
       we run it now, otherwise, we just schedule it to be run. */
    cur_prio = FIXNUM_TO_INT(thread_get_priority(c->current_thread));
    new_prio = FIXNUM_TO_INT(thread_get_priority(thr));
    if(new_prio > cur_prio) {
      cpu_thread_schedule(state, c->current_thread);
      cpu_thread_switch(state, c, thr);
    } else {
      cpu_thread_schedule(state, thr);
    }
  }
  
  return obj;
}

void cpu_channel_receive(STATE, cpu c, OBJECT self, OBJECT cur_thr) {
  OBJECT val, lst;
  
  val = channel_get_value(self);
  if(!NIL_P(val)) {
    stack_push(val);
    return;
  }
  
  /* We push nil on the stack to reserve a place to put the result. */
  stack_push(Qnil);
  
  lst = channel_get_waiting(self);
  list_append(state, lst, cur_thr);
  cpu_thread_run_best(state, c);
}

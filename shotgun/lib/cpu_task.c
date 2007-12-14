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
#include "array.h"
#include <sys/time.h>

void cpu_task_cleanup(STATE, OBJECT self) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  
  if(!task->stack_slave) {
    free(task->stack_top);
  }
}

extern machine current_machine;
void cpu_task_configure_premption(STATE);

void Init_cpu_task(STATE) {
  OBJECT tup;
  state_add_cleanup(state, state->global->task, cpu_task_cleanup);
  
  tup = tuple_new2(state, 7, list_new(state), list_new(state), list_new(state),
    list_new(state), list_new(state), list_new(state), list_new(state));
  
  state->global->scheduled_threads = tup;
  rbs_const_set(state, BASIC_CLASS(task), "ScheduledThreads", tup);
  
  BASIC_CLASS(channel) = rbs_class_new(state, "Channel", 3, BASIC_CLASS(object));
  BASIC_CLASS(thread) =  rbs_class_new(state, "Thread", 4, BASIC_CLASS(object));
  class_set_has_ivars(BASIC_CLASS(channel), Qtrue);
  class_set_has_ivars(BASIC_CLASS(thread),  Qtrue);
  
  cpu_event_init(state);  
}

static void _cpu_task_preempt(int sig) {
  current_machine->s->check_events = 1;
}

void cpu_task_configure_premption(STATE) {
  struct itimerval new, old;
  new.it_interval.tv_usec = 10000; /* Same as MRI */
  new.it_interval.tv_sec = 0;
  new.it_value.tv_usec = new.it_interval.tv_usec;
  new.it_value.tv_sec = 0;
    
  signal(SIGVTALRM, _cpu_task_preempt);
  setitimer(ITIMER_VIRTUAL, &new, &old);
}

void cpu_task_disable_preemption(STATE) {
  struct itimerval new;
  new.it_interval.tv_usec = 0;
  new.it_interval.tv_sec = 0;
  new.it_value.tv_usec = new.it_interval.tv_usec;
  new.it_value.tv_sec = 0;
    
  signal(SIGVTALRM, SIG_DFL);
  setitimer(ITIMER_VIRTUAL, &new, NULL);
}

OBJECT cpu_task_dup(STATE, cpu c, OBJECT cur) {
  struct cpu_task *cur_task, *task;
  
  OBJECT obj;
  OBJECT *ns;
  
  cpu_save_registers(state, c, 0);
  cpu_flush_sp(c);
  cpu_flush_ip(c);
  
  if(NIL_P(cur) || cur == c->current_task) {
    cur_task = (struct cpu_task*)CPU_TASKS_LOCATION(c);
    
    /*
    if(!NIL_P(c->active_context)) {
      printf("(current task dup) ip:%d, sp:%d, fp:%d%s\n", c->ip, c->sp, c->fp, _inspect(cpu_current_method(state, c)));
    }
    */
  } else {
    cur_task = (struct cpu_task*)BYTES_OF(cur);
    
    /*
    printf("(      a task dup) ip:%d, sp:%d, fp:%d %s\n", FASTCTX(cur_task->active_context)->ip, FASTCTX(cur_task->active_context)->sp, FASTCTX(cur_task->active_context)->fp,  _inspect(cpu_current_method(state, cur_task)));
    */
  }
  
  NEW_STRUCT(obj, task, state->global->task, struct cpu_task);
  memcpy(task, cur_task, sizeof(struct cpu_task));
  
  /* Duplicate the operand stack. */
  ns = (OBJECT*)calloc(InitialStackSize, sizeof(OBJECT));
  memcpy(ns, task->stack_top, InitialStackSize * sizeof(OBJECT));
  task->stack_top = ns;
  task->stack_size = InitialStackSize;
  task->stack_slave = 0;
  
  /* Duplicate the context chain */
  if(!NIL_P(task->active_context)) {
    task->active_context = 
      methctx_dup_chain(state, task->active_context, &task->home_context);
  }
    
  return obj;
}

int cpu_task_select(STATE, cpu c, OBJECT nw) {
  struct cpu_task *cur_task, *new_task, *ct;
  OBJECT home, cur;
  cpu_save_registers(state, c, 0);

  if(state->excessive_tracing) {
    printf("[CPU] Switching to task %s (%p, %d)\n", _inspect(nw), c->sp_ptr, c->sp);
  }
  cur = c->current_task;
  
  /* Invalidates the stack, so they don't get confused being used across boundries */
  if(REFERENCE_P(c->active_context)) methctx_reference(state, c->active_context);
  if(REFERENCE_P(c->home_context))   methctx_reference(state, c->home_context);
  
  ct = (struct cpu_task*)CPU_TASKS_LOCATION(c);
  cur_task = (struct cpu_task*)BYTES_OF(cur);
  new_task = (struct cpu_task*)BYTES_OF(nw);
  
  if(NIL_P(new_task->active_context) || NIL_P(new_task->home_context)) {
    cpu_raise_arg_error_generic(state, c, "Task has already exited");
    return FALSE;
  }
  
  memcpy(cur_task, ct, sizeof(struct cpu_task));
  // printf(" Saving to task %p\t(%lu / %lu / %p / %p / %p)\n", (void*)cur, c->sp, c->ip, cpu_current_method(state, c), c->active_context, c->home_context);
  memcpy(ct, new_task, sizeof(struct cpu_task));
  
  home = NIL_P(c->home_context) ? c->active_context : c->home_context;
    
  cpu_restore_context_with_home(state, c, c->active_context, home, FALSE, FALSE);
  // printf("Swaping to task %p\t(%lu / %lu / %p / %p / %p)\n", (void*)nw, c->sp, c->ip, cpu_current_method(state, c), c->active_context, c->home_context);
  
  /*
  printf("(switched to task) ip:%d, sp:%d, fp:%d %s\n", c->ip, c->sp, c->fp, _inspect(cpu_current_method(state, c)));
  */
  c->current_task = nw;
  return TRUE;
}

void cpu_task_push(STATE, OBJECT self, OBJECT val);

OBJECT cpu_task_associate(STATE, cpu c, OBJECT self, OBJECT be) {
  OBJECT bc;
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  
  bc = cpu_create_block_context(state, c, be, 1);
  FASTCTX(bc)->sender = Qnil;
  
  if(task->stack_slave) {
    task->stack_top = (OBJECT*)calloc(InitialStackSize, sizeof(OBJECT));
    task->stack_size = InitialStackSize;
    task->stack_slave = 0;
  }
  
  task->sp_ptr = task->stack_top;
    
  /* The args to the block (none). */
  cpu_task_push(state, self, tuple_new(state, 0));
  
  methctx_reference(state, bc);
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

static int cpu_task_no_stack_p(STATE, OBJECT self) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  return TASK_FLAG_P(task, TASK_NO_STACK);
}

static void cpu_task_clear_flag(STATE, OBJECT self, int flag) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  TASK_CLEAR_FLAG(task, flag);  
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

void cpu_task_set_outstanding(STATE, OBJECT self, OBJECT ary) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  task->outstanding = ary;
}

OBJECT cpu_task_get_outstanding(STATE, OBJECT self) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  return task->outstanding;
}

void cpu_task_set_debugging(STATE, OBJECT self, OBJECT dc, OBJECT cc) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  task->debug_channel = dc;
  task->control_channel = cc;
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
  
  state->pending_threads++;
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
  
  cpu_event_update(state);

  while(1) {
    tup = state->global->scheduled_threads;
    t = NUM_FIELDS(tup);
    for(i = t - 1; i >= 0; i--) {
      lst = tuple_at(state, tup, i);
      if(FIXNUM_TO_INT(list_get_count(lst)) != 0) {
        state->pending_threads--;
        return list_shift(state, lst);
      }
    }
    // printf("Nothing to do, waiting for events.\n");
    if(!cpu_event_outstanding_p(state)) {
      printf("DEADLOCK!\n");
      abort();
    }
    cpu_event_run(state);
  }
}

void cpu_thread_dequeue(STATE, OBJECT thr) {
  int i, t;
  OBJECT lst, tup;
  
  tup = state->global->scheduled_threads;
  t = NUM_FIELDS(tup);
  for(i = 0; i < t; i++) {
    lst = tuple_at(state, tup, i);
    
    /* We could exit here, since a thread should only be in one
       priority list. But run them all for now to be sure. */
    state->pending_threads -= list_delete(state, lst, thr);
  }
}

void cpu_thread_force_run(STATE, cpu c, OBJECT thr) {
  cpu_thread_dequeue(state, thr);
  cpu_thread_switch(state, c, thr);
}

void cpu_thread_switch(STATE, cpu c, OBJECT thr) {
  OBJECT task;

  /* Edge case. We could realize that we need to restore
     the already running thread (via the current thread waiting
     for an event), and we thus don't need to restore it. */
  if(thr == c->current_thread) return;
    
  /* Save the current task back into the current thread, in case
     Task's were used inside the thread itself (not just for the thread). */
  thread_set_task(c->current_thread, c->current_task);
  task = thread_get_task(thr);
  cpu_task_select(state, c, task);
  c->current_thread = thr;
}

void cpu_thread_run_best(STATE, cpu c) {
  OBJECT thr;
  
  thr = cpu_thread_find_highest(state);
  cpu_thread_switch(state, c, thr);
}

void cpu_thread_switch_best(STATE, cpu c) {
  OBJECT thr, tup, lst;
  int i, t;
  
  if(state->pending_threads == 0) return;
  
  tup = state->global->scheduled_threads;
  t = NUM_FIELDS(tup);
  
  thr = Qnil;
  
  for(i = t - 1; i >= 0; i--) {
    lst = tuple_at(state, tup, i);
    if(FIXNUM_TO_INT(list_get_count(lst)) != 0) {
      thr = list_shift(state, lst);
      break;
    }
  }
  
  if(thr == Qnil) return;
  
  state->pending_threads--;
  cpu_thread_schedule(state, c->current_thread);
  cpu_thread_switch(state, c, thr);
}

OBJECT cpu_channel_new(STATE) {
  OBJECT chan;
  chan = rbs_class_new_instance(state, BASIC_CLASS(channel));
  channel_set_waiting(chan, list_new(state));
  return chan;
}

void _cpu_channel_clear_outstanding(STATE, cpu c, OBJECT thr, OBJECT ary) {
  OBJECT t1, t2, t3;
  int k, j;
  
  t1 = array_get_tuple(ary);
  k = FIXNUM_TO_INT(array_get_total(ary));
  
  for(j = 0; j < k; j++) {
    t2 = tuple_at(state, t1, j);
    
    t3 = channel_get_waiting(t2);
    list_delete(state, t3, thr);
  }
  
}

OBJECT cpu_channel_send(STATE, cpu c, OBJECT self, OBJECT obj) {
  OBJECT readers, written, reader, reader_task, pair, out;
  long int cur_prio, new_prio;
    
  readers = channel_get_waiting(self);
  
  if(list_empty_p(readers)) {
    written = channel_get_value(self);
    if(NIL_P(written)) {
      written = list_new(state);
      channel_set_value(self, written);
    }
    list_append(state, written, obj);
  } else {
    pair = tuple_new2(state, 2, self, obj);
    reader = list_shift(state, readers);
    reader_task = thread_get_task(reader);
    /* Edge case. After going all around, we've decided that the current
       task needs to be restored. Since it's not yet saved, we push it
       to the current stack, since thats the current task's stack. */
    if(reader_task == c->current_task) {
      stack_pop();
      if(!TASK_FLAG_P(c, TASK_NO_STACK)) {
        stack_push(pair);
      } else {
        TASK_CLEAR_FLAG(c, TASK_NO_STACK);
      }
      out = c->outstanding;
    } else {
      if(cpu_task_no_stack_p(state, reader_task)) {
        cpu_task_pop(state, reader_task);
        cpu_task_clear_flag(state, reader_task, TASK_NO_STACK);
      } else {
        cpu_task_set_top(state, reader_task, pair);
      }
      out = cpu_task_get_outstanding(state, reader_task);
    }
    if(!NIL_P(out)) {
      _cpu_channel_clear_outstanding(state, c, reader, out);
    }
    /* If we're resuming a thread thats of higher priority than we are, 
       we run it now, otherwise, we just schedule it to be run. */
    cur_prio = FIXNUM_TO_INT(thread_get_priority(c->current_thread));
    new_prio = FIXNUM_TO_INT(thread_get_priority(reader));
    if(new_prio > cur_prio) {
      cpu_thread_schedule(state, c->current_thread);
      cpu_thread_switch(state, c, reader);
    } else {
      cpu_thread_schedule(state, reader);
    }
  }
  
  return obj;
}

void cpu_channel_register(STATE, cpu c, OBJECT self, OBJECT cur_thr) {
  OBJECT lst;
    
  lst = channel_get_waiting(self);
  list_append(state, lst, cur_thr);
}

void cpu_channel_receive(STATE, cpu c, OBJECT self, OBJECT cur_thr) {
  OBJECT written, obj, readers;
  
  written = channel_get_value(self);
  if(!NIL_P(written) && !list_empty_p(written)) {
    obj = list_shift(state, written);
    stack_push(tuple_new2(state, 2, self, obj));
    return;
  }
  
  /* We push nil on the stack to reserve a place to put the result. */
  stack_push(I2N(343434));
  
  readers = channel_get_waiting(self);
  list_append(state, readers, cur_thr);
  cpu_thread_run_best(state, c);
}


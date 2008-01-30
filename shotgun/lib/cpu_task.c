#include <stdlib.h>
#include <sys/time.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/machine.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/methctx.h"
#include "shotgun/lib/object.h"
#include "shotgun/lib/bytearray.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/class.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/list.h"
#include "shotgun/lib/array.h"
#include "shotgun/lib/object.h"

#define thread_set_priority(obj, val) SET_FIELD(obj, 1, val)
#define thread_set_task(obj, val) SET_FIELD(obj, 2, val)
#define thread_set_joins(obj, val) SET_FIELD(obj, 3, val)
#define thread_set_channel(obj, val) SET_FIELD(obj, 4, val)

#define thread_get_priority(obj) NTH_FIELD(obj, 1)
#define thread_get_task(obj) NTH_FIELD(obj, 2)
#define thread_get_joins(obj) NTH_FIELD(obj, 3)
#define thread_get_channel(obj) NTH_FIELD(obj, 4)


void cpu_task_cleanup(STATE, OBJECT self) {
  struct cpu_task *task;

  task = (struct cpu_task*)BYTES_OF(self);

  if(!task->stack_slave) {
    if(task->stack_top) {
      XFREE(task->stack_top);
    }
    task->stack_top = NULL;
  }
}

void cpu_task_configure_preemption(STATE);

void Init_cpu_task(STATE) {
  OBJECT tup;
  state_add_cleanup(state, state->global->task, cpu_task_cleanup);
  
  tup = tuple_new2(state, 7, list_new(state), list_new(state), list_new(state),
    list_new(state), list_new(state), list_new(state), list_new(state));
  
  state->global->scheduled_threads = tup;
  rbs_const_set(state, BASIC_CLASS(task), "ScheduledThreads", tup);
  
  BASIC_CLASS(channel) = rbs_class_new(state, "Channel", 3, BASIC_CLASS(object));
  BASIC_CLASS(thread) =  rbs_class_new(state, "Thread", 5, BASIC_CLASS(object));
  class_set_has_ivars(BASIC_CLASS(channel), Qtrue);
  class_set_has_ivars(BASIC_CLASS(thread),  Qtrue);
  
  cpu_event_init(state);

  state->global->on_gc_channel = cpu_channel_new(state);
  rbs_const_set(state, 
      rbs_const_get(state, BASIC_CLASS(object), "Rubinius"), "ON_GC",
      state->global->on_gc_channel);
}

static void _cpu_task_preempt(int sig) {
  current_machine->s->check_events = 1;
}

void cpu_task_configure_preemption(STATE) {
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

static void cpu_task_run_wb(STATE, OBJECT obj) {
  struct cpu_task *task;

  task = (struct cpu_task*)BYTES_OF(obj);

  RUN_WB(obj, task->exception);
  RUN_WB(obj, task->enclosing_class);
  RUN_WB(obj, task->main);
  RUN_WB(obj, task->current_scope);
  RUN_WB(obj, task->debug_channel);
  RUN_WB(obj, task->control_channel);
  RUN_WB(obj, task->active_context);
  RUN_WB(obj, task->home_context);
}

OBJECT cpu_task_dup(STATE, cpu c, OBJECT cur) {
  struct cpu_task *cur_task, *task;
  
  OBJECT obj, home;
  OBJECT *ns;
  
  if(c->active_context != Qnil) {
    cpu_save_registers(state, c, 0);
  }
  cpu_flush_sp(c);
  cpu_flush_ip(c);
  
  if(NIL_P(cur) || cur == c->current_task) {
    cur_task = (struct cpu_task*)CPU_TASKS_LOCATION(c);
  } else {
    cur_task = (struct cpu_task*)BYTES_OF(cur);
  }
  
  NEW_STRUCT(obj, task, state->global->task, struct cpu_task);
  memcpy(task, cur_task, sizeof(struct cpu_task_shared));

  task->active = FALSE;
  
  /* Duplicate the operand stack. */
  ns = ALLOC_N(OBJECT, InitialStackSize);
  memcpy(ns, task->stack_top, InitialStackSize * SIZE_OF_OBJECT);
  task->stack_top = ns;
  task->stack_size = InitialStackSize;
  task->stack_slave = 0;
  
  if(NIL_P(task->active_context)) {
    task->sp_ptr = ns;
  } else {
    task->sp_ptr = ns + FASTCTX(task->active_context)->sp;
  }
  
  home = task->home_context;
  
  /* Duplicate the context chain */
  if(!NIL_P(task->active_context)) {
    task->active_context = 
      methctx_dup_chain(state, task->active_context, &task->home_context);
  }
  
  if(!NIL_P(home) && home == task->home_context) {
    task->home_context = methctx_dup_chain(state, task->home_context, NULL);
  }
    
  if(REFERENCE_P(task->active_context)) {
    methctx_reference(state, task->active_context);
    
    assert(task->active_context->obj_type == MContextType ||
           task->active_context->obj_type == BContextType);

  }
  
  if(REFERENCE_P(task->home_context)) {    
    assert(home != task->home_context);
    
    methctx_reference(state, task->home_context);
    
    assert(task->home_context->obj_type == MContextType ||
           task->home_context->obj_type == BContextType);

  }

  cpu_task_run_wb(state, obj);
  
  return obj;
}

int cpu_task_alive_p(STATE, OBJECT self) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  
  if(!REFERENCE_P(task->active_context)) return FALSE;
  if(!REFERENCE_P(task->home_context)) return FALSE;
  
  return TRUE;
}

void cpu_task_flush(STATE, cpu c) {
  struct cpu_task *task, *ct;
  
  if(c->active_context != Qnil) {
    cpu_save_registers(state, c, 0);
  }

  methctx_reference(state, c->active_context);
  methctx_reference(state, c->home_context);
    
  ct = (struct cpu_task*)CPU_TASKS_LOCATION(c);
  task = (struct cpu_task*)BYTES_OF(c->current_task);
  
  memcpy(task, ct, sizeof(struct cpu_task_shared));
  
  cpu_task_run_wb(state, c->current_task);
}

int cpu_task_select(STATE, cpu c, OBJECT nw) {
  struct cpu_task *cur_task, *new_task, *ct;
  OBJECT home, cur;

  assert(cpu_task_alive_p(state, nw));

  if(c->active_context != Qnil) {
    cpu_save_registers(state, c, 0);
  }

  cur = c->current_task;
  
  /* Invalidates the stack, so they don't get confused being used across boundries */
  if(REFERENCE_P(c->active_context)) methctx_reference(state, c->active_context);
  if(REFERENCE_P(c->home_context))   methctx_reference(state, c->home_context);
  
  new_task = (struct cpu_task*)BYTES_OF(nw);
  if(NIL_P(new_task->active_context) || NIL_P(new_task->home_context)) {
    cpu_raise_arg_error_generic(state, c, "Task has already exited");
    return FALSE;
  }
  
  if(state->excessive_tracing) {
    printf("[CPU] Switching to task %s (%p, %p)\n", _inspect(nw), nw, new_task->sp_ptr);
  }
  
  ct = (struct cpu_task*)CPU_TASKS_LOCATION(c);

  if(!NIL_P(cur)) {
    cur_task = (struct cpu_task*)BYTES_OF(cur);
  
    memcpy(cur_task, ct, sizeof(struct cpu_task_shared));
    cpu_task_run_wb(state, cur);
  
    assert(cur_task->sp_ptr >= cur_task->stack_top);
    if(REFERENCE_P(cur_task->active_context)) {
      assert(cur_task->active_context->obj_type == MContextType ||
        cur_task->active_context->obj_type == BContextType);

    }

    if(REFERENCE_P(cur_task->home_context)) {
      assert(cur_task->home_context->obj_type == MContextType ||
          cur_task->home_context->obj_type == BContextType);
    }
  
    cur_task->active = FALSE;
  }

  memcpy(ct, new_task, sizeof(struct cpu_task_shared));
  new_task->active = TRUE;
  
  home = NIL_P(c->home_context) ? c->active_context : c->home_context;
  cpu_restore_context_with_home(state, c, c->active_context, home, FALSE, FALSE);
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
    task->stack_top = ALLOC_N(OBJECT, InitialStackSize);
    task->stack_size = InitialStackSize;
    task->stack_slave = 0;
  }
  task->depth = 1;
  
  task->sp_ptr = task->stack_top;
    
  /* The args to the block (none). */
  cpu_task_push(state, self, tuple_new(state, 0));
  
  methctx_reference(state, bc);
  methctx_reference(state, task->home_context);
  
  SET_STRUCT_FIELD(self, task->main, bc);
  SET_STRUCT_FIELD(self, task->active_context, bc);
  SET_STRUCT_FIELD(self, task->home_context, blokenv_get_home(be));
  
  cpu_task_run_wb(state, self);
  
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

void cpu_task_set_debugging(STATE, OBJECT self, OBJECT dc, OBJECT cc) {
  struct cpu_task *task;
  
  task = (struct cpu_task*)BYTES_OF(self);
  task->debug_channel = dc;
  task->control_channel = cc;

  cpu_task_run_wb(state, self);
}

OBJECT cpu_thread_new(STATE, cpu c) {
  OBJECT thr;
  
  thr = rbs_class_new_instance(state, BASIC_CLASS(thread));
  if(c->current_thread && !NIL_P(c->current_thread)) {
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

void cpu_thread_exited(STATE, cpu c) {
  OBJECT chan;
  thread_set_task(c->current_thread, Qnil);
  cpu_thread_dequeue(state, c->current_thread);

  /* Clear any events this thread was waiting on. */
  chan = thread_get_channel(c->current_thread);
  if(!NIL_P(chan)) {
    cpu_event_clear_channel(state, chan);
  }

  c->depth = 0;
  c->active_context = Qnil;
  c->home_context = Qnil;
  c->current_thread = Qnil;
  c->current_task = Qnil;
  THDEBUG("%d: thread exitted, switching.\n", getpid());
  cpu_thread_run_best(state, c);
}

int cpu_thread_alive_p(STATE, OBJECT self) {
  OBJECT task;
  
  task = thread_get_task(self);
  if(NIL_P(task)) {
    return FALSE;
  } else {
    return cpu_task_alive_p(state, task);
  }
}

void cpu_thread_schedule(STATE, OBJECT self) {
  long int prio, rprio;
  OBJECT lst;
  
  object_set_ivar(state, self, SYM("@sleep"), Qfalse);
  
  state->pending_threads++;
  prio = N2I(thread_get_priority(self));
  
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
  OBJECT lst, tup, thr;

  tup = state->global->scheduled_threads;
  t = NUM_FIELDS(tup);
  for(i = t - 1; i >= 0; i--) {
    lst = tuple_at(state, tup, i);
    if(N2I(list_get_count(lst)) != 0) {
      state->pending_threads--;
      thr = list_shift(state, lst);
      /* It's a bug that a dead thread shows up as queued.
       * But for now, just check again here, it's safer anyway. */
      if(cpu_thread_alive_p(state, thr)) return thr;
    }
  }

  return Qnil;
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
 
  assert(cpu_thread_alive_p(state, thr));

  object_set_ivar(state, thr, SYM("@sleep"), Qfalse);
    
  /* Save the current task back into the current thread, in case
     Task's were used inside the thread itself (not just for the thread). */
  if(!NIL_P(c->current_thread)) {
    thread_set_task(c->current_thread, c->current_task);
  }

  task = thread_get_task(thr);
  cpu_task_select(state, c, task);
  c->current_thread = thr;
}

/* Called because the current thread is waiting on something. */
void cpu_thread_run_best(STATE, cpu c) {
  OBJECT thr;
 
  THDEBUG("%d: poll for events.\n", getpid());
  cpu_event_update(state);

  thr = cpu_thread_find_highest(state);

  /* Ok, we found a suspended thread to run. */
  if(!NIL_P(thr)) {
    THDEBUG("%d: poll found thread %p\n", getpid(), thr);
    cpu_thread_switch(state, c, thr);

  } else {
    /* No threads to run. Let events run and swap in a new thread. */
    if(!cpu_event_outstanding_p(state)) {
      printf("DEADLOCK!\n");
      abort();
    }

      
    THDEBUG("%d: waiting for events. (%d)\n", getpid(), state->pending_threads);
    while(1) {
      /* Running the events will swap in a new thread. */
      cpu_event_run(state);
      
      thr = cpu_thread_find_highest(state);
      if(!NIL_P(thr)) {
        THDEBUG("%d: waiting found thread %p\n", getpid(), thr);
        cpu_thread_switch(state, c, thr);
        break;
      }
    }
  }
}

void cpu_thread_preempt(STATE, cpu c) {
  OBJECT thr;
  
  cpu_event_update(state);
  
  thr = cpu_thread_find_highest(state);
  
  /* No one else to run.. */
  if(thr == Qnil) return;

  if(thr != c->current_thread) {
    cpu_thread_schedule(state, c->current_thread);
    cpu_thread_switch(state, c, thr);
  }
}

OBJECT cpu_channel_new(STATE) {
  OBJECT chan;
  chan = rbs_class_new_instance(state, BASIC_CLASS(channel));
  channel_set_waiting(chan, list_new(state));
  return chan;
}

int cpu_channel_has_readers_p(STATE, OBJECT self) {
  OBJECT readers;

  readers = channel_get_waiting(self);

  if(list_empty_p(readers)) {
    return FALSE;
  }

  return TRUE;
}

OBJECT cpu_channel_send(STATE, cpu c, OBJECT self, OBJECT obj) {
  OBJECT readers, written, reader, reader_task;
    
  readers = channel_get_waiting(self);
  
  if(list_empty_p(readers)) {
    save_value:
    written = channel_get_value(self);
    if(NIL_P(written)) {
      written = list_new(state);
      channel_set_value(self, written);
    }
    list_append(state, written, obj);
  } else {
    reader = list_shift(state, readers);
    /* Edge case. After going all around, we've decided that the current
       task needs to be restored. Since it's not yet saved, we push it
       to the current stack, since thats the current task's stack. */
    if(reader == c->current_thread) {
      if(!TASK_FLAG_P(c, TASK_NO_STACK)) {
        stack_set_top(obj);
      } else {
        TASK_CLEAR_FLAG(c, TASK_NO_STACK);
      }
    } else {
      reader_task = thread_get_task(reader);

      /* eek. the reader is dead. oh well. pretend we didn't see it. */
      while(NIL_P(reader_task)) {
        reader = list_shift(state, readers);
        if(NIL_P(reader)) goto save_value;
        reader_task = thread_get_task(reader);
      }
      
      if(cpu_task_no_stack_p(state, reader_task)) {
        cpu_task_clear_flag(state, reader_task, TASK_NO_STACK);
      } else {
        cpu_task_set_top(state, reader_task, obj);
      }
    }

    thread_set_channel(reader, Qnil);

    cpu_thread_schedule(state, reader);
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
    stack_push(obj);
    return;
  }
  
  /* We push nil on the stack to reserve a place to put the result. */
  if(!TASK_FLAG_P(c, TASK_NO_STACK)) {
    stack_push(Qfalse);
  }

  THDEBUG("%d: thread %p, receive on %p\n", getpid(), cur_thr, self);

  thread_set_channel(cur_thr, self);
  
  object_set_ivar(state, cur_thr, SYM("@sleep"), Qtrue);
  readers = channel_get_waiting(self);
  list_append(state, readers, cur_thr);
  cpu_thread_run_best(state, c);
}


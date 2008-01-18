#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <event.h>

#include "shotgun/config.h"
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

typedef enum { OTHER, WAITER, SIGNAL } thread_info_type;
struct thread_info {
  STATE;
  cpu c;
  struct event ev;
  OBJECT channel;
  OBJECT buffer;
  int count;
  pid_t pid;
  int fd;
  union {
    int options;
    int sig;
  };
  thread_info_type type;
  struct thread_info *prev, *next;
};

void cpu_event_init(STATE) {

/* kqueue is broken on darwin, so we have to disable it's use. */
#if CONFIG_DISABLE_KQUEUE
  setenv("EVENT_NOKQUEUE", "1", 1);
  setenv("EVENT_NOPOLL", "1", 1);
#endif
  state->event_base = event_init();
  state->thread_infos = NULL;
}

void cpu_event_run(STATE) {
  event_base_loop(state->event_base, EVLOOP_ONCE);
}

void cpu_event_runonce(STATE) {
  event_base_loop(state->event_base, EVLOOP_ONCE | EVLOOP_NONBLOCK);  
}

void cpu_event_each_channel(STATE, OBJECT (*cb)(STATE, void*, OBJECT), void *cb_data) {
  struct thread_info *ti = (struct thread_info*)state->thread_infos;
  while(ti) {
    if(ti->channel && REFERENCE_P(ti->channel)) {
      ti->channel = cb(state, cb_data, ti->channel);
    }
    if(ti->buffer && REFERENCE_P(ti->buffer)) {
      ti->buffer = cb(state, cb_data, ti->buffer);
    }
    ti = ti->next;
  }
}

void _cpu_event_register_info(STATE, struct thread_info *ti) {
  state->pending_events++;
  ti->prev = NULL;
  ti->next = state->thread_infos;
  if(state->thread_infos) {
    ((struct thread_info*)state->thread_infos)->prev = ti;
  }
  state->thread_infos = ti;
}

void _cpu_event_unregister_info(STATE, struct thread_info *ti) {
  struct thread_info *next, *prev;

  next = ti->next;
  prev = ti->prev;

  if(next) {
    next->prev = prev;
  }

  if(prev) {
    prev->next = next;
  }

  if(next) {
    if(next->next == prev) {
      next->next = NULL;
    }
  }

  if(prev) {
    if(prev->prev == next) {
      prev->prev = NULL;
    }
  }

  if(state->thread_infos == ti) {
    if(next) {
      state->thread_infos = next;
    } else if(prev) {
      state->thread_infos = prev;
    } else {
      state->thread_infos = NULL;
    }
  }

  event_del(&ti->ev);
  state->pending_events--;
}

void cpu_event_clear(STATE, int fd) {
  struct thread_info *ti = (struct thread_info*)state->thread_infos;
  struct thread_info *tnext;

  if(fd <= 0) return;

  while(ti) {
    tnext = ti->next;
    if(ti->fd == fd) {
      cpu_channel_send(state, ti->c, ti->channel, Qnil);
      _cpu_event_unregister_info(state, ti);
      XFREE(ti);
    }
    ti = tnext;
  }

}

void _cpu_wake_channel(int fd, short event, void *arg) {
  STATE;
  struct thread_info *ti = (struct thread_info*)arg;
  
  state = ti->state;
  
  cpu_channel_send(state, ti->c, ti->channel, I2N((int)event));
  _cpu_event_unregister_info(state, ti);
  XFREE(ti);
}

void _cpu_wake_channel_and_read(int fd, short event, void *arg) {
  STATE;
  size_t sz, total, offset;
  ssize_t i;
  char *buf;
  OBJECT ret, ba, enc;
  struct thread_info *ti = (struct thread_info*)arg;
  
  state = ti->state;
  
  if(NIL_P(ti->buffer)) {
    ret = I2N(fd);
  } else {
    ba = string_get_data(ti->buffer);
    enc = string_get_encoding(ti->buffer);
    sz = (size_t)ti->count;
    
    if(enc == SYM("buffer")) {
      offset = FIXNUM_TO_INT(string_get_bytes(ti->buffer));
    } else {
      offset = 0;
    }

    /* Clamp the read size so we don't overrun */
    total = SIZE_OF_BODY(ba) - offset - 1;
    if(total < sz) {
      sz = total;
    }
    
    buf = bytearray_byte_address(state, ba);
    buf += offset;
    
    while(1) {
      i = read(fd, buf, sz);
      if(i == 0) {
        ret = Qnil; 
      } else if(i == -1) {
        /* If we read and got nothing, go again. We must get something. 
           It might be better to re-schedule this in libevent and try again,
           but libevent just said SOMETHING was there... */
        if(errno == EINTR) continue;
        ret = hash_find(state, state->global->errno_mapping, I2N(errno));
      } else {        
        buf[i] = 0;
        string_set_bytes(ti->buffer, I2N(i + offset));
        
        ret = I2N(i);
      }
      break;
    }
  }
  
  cpu_channel_send(state, ti->c, ti->channel, ret);
  
  _cpu_event_unregister_info(state, ti);
  XFREE(ti);
}

/* Doesn't clear its own data, since it's going to be called a lot. */
void _cpu_wake_channel_alot(int fd, short event, void *arg) {
  STATE;
  struct thread_info *ti = (struct thread_info*)arg;
    
  state = ti->state;
  cpu_channel_send(state, ti->c, ti->channel, I2N((int)event));
}

static void cpu_wake_channel_for_signal(struct thread_info *ti, int remove) {
  if (remove) {
    cpu_channel_send(ti->state, ti->c, ti->channel, Qnil);
  } else {
    cpu_channel_send(ti->state, ti->c, ti->channel, ti->c->current_thread);
  }
}

/* Doesn't clear its own data, since it's going to be called a lot. */
static void _cpu_wake_channel_for_signal_cb(int fd, short event, void *arg) {
  struct thread_info *ti = (struct thread_info*)arg;

  cpu_wake_channel_for_signal(ti, 0);
}


void cpu_event_wake_channel(STATE, cpu c, OBJECT channel, struct timeval *tv) {
  struct thread_info *ti;

  ti = ALLOC_N(struct thread_info, 1);
  ti->fd = 0;
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  _cpu_event_register_info(state, ti);

  evtimer_set(&ti->ev, _cpu_wake_channel, (void*)ti);
  event_base_set((struct event_base*)state->event_base, &ti->ev);
  evtimer_add(&ti->ev, tv);
}

/* TODO: fd needs to be a typedef, set per platform. */
void cpu_event_wait_readable(STATE, cpu c, OBJECT channel, int fd, OBJECT buffer, int count) {
  struct thread_info *ti;

  ti = ALLOC_N(struct thread_info, 1);
  ti->fd = fd;
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  ti->buffer = buffer;
  ti->count = count;
  
  event_set(&ti->ev, fd, EV_READ, _cpu_wake_channel_and_read, (void*)ti);
  event_base_set((struct event_base*)state->event_base, &ti->ev);
  
  /* Check that we were able to add it correctly... */
  if(event_add(&ti->ev, NULL) != 0) {
    struct stat sb;
    fstat(fd, &sb);
    if(S_ISREG(sb.st_mode) || S_ISDIR(sb.st_mode) || S_ISLNK(sb.st_mode)) {
      /* If we couldn't.. hm... for now, lets just spin the channel directly. */
      _cpu_wake_channel_and_read(fd, EV_READ, (void*)ti);
    } else {
      /* So, we couldn't find out if it's readable, and it's probably
       * not something we can read from. Send nil. */
      cpu_channel_send(state, ti->c, ti->channel, Qnil);
      XFREE(ti);
    }
  } else {
    _cpu_event_register_info(state, ti);
  }
}

/* TODO: fd needs to be a typedef, set per platform. */
void cpu_event_wait_writable(STATE, cpu c, OBJECT channel, int fd) {
  struct thread_info *ti;

  ti = ALLOC_N(struct thread_info, 1);
  ti->fd = fd;
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  _cpu_event_register_info(state, ti);
  
  event_set(&ti->ev, fd, EV_WRITE, _cpu_wake_channel, (void*)ti);
  event_base_set((struct event_base*)state->event_base, &ti->ev);
  
  if(event_add(&ti->ev, NULL) != 0) {
    struct stat sb;
    fstat(fd, &sb);
    if(S_ISREG(sb.st_mode) || S_ISDIR(sb.st_mode) || S_ISLNK(sb.st_mode)) {
      /* If we couldn't.. hm... for now, lets just spin the channel directly. */
      _cpu_wake_channel(fd, EV_WRITE, (void*)ti);
    } else {
      /* So, we couldn't find out if it's readable, and it's probably
       * not something we can read from. Send nil. */
      cpu_channel_send(state, ti->c, ti->channel, Qnil);
      XFREE(ti);
    }
  } else {
    _cpu_event_register_info(state, ti);
  }
}

void cpu_event_wait_signal(STATE, cpu c, OBJECT channel, int sig) {
  struct thread_info *ti;

  ti = (struct thread_info*)(state->thread_infos);
  while (ti) {
    if (ti->type == SIGNAL && ti->sig == sig) {
      _cpu_event_unregister_info(state, ti);
      cpu_wake_channel_for_signal(ti, TRUE);
      XFREE(ti);
      break;
    }
    ti = ti->next;
  }

  /* Here is a short period during which a incoming signal would not
     be delivered to either an old handler or the new handler. */

  ti = ALLOC_N(struct thread_info, 1);
  ti->fd = 0;
  ti->type = SIGNAL;
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  ti->sig = sig;
  _cpu_event_register_info(state, ti);

  signal_set(&ti->ev, sig, _cpu_wake_channel_for_signal_cb, (void*)ti);
  event_base_set((struct event_base*)state->event_base, &ti->ev);
  
  signal_add(&ti->ev, NULL);
}

static void cpu_find_waiters(STATE) {
  pid_t pid;
  int status, skip;
  OBJECT ret;
  struct thread_info *ti, *tnext;

  ti = (struct thread_info*)(state->thread_infos);
  while (ti) {
    if (ti->type != WAITER) {
      ti = ti->next;
      continue;
    }
    while((pid = waitpid(ti->pid, &status, WNOHANG)) == -1 && errno == EINTR)
      ;

    skip = 0;
    if (pid > 0) {
      if (WIFEXITED(status)) {
        ret = I2N(WEXITSTATUS(status));
      } else {
        /* Could support WIFSIGNALED also. */
        ret = Qtrue;
      }
      cpu_channel_send(ti->state, ti->c, ti->channel, 
                       tuple_new2(state, 2, I2N(pid), ret));
    } else if (pid == -1 && errno == ECHILD) {
      cpu_channel_send(ti->state, ti->c, ti->channel, Qfalse);
    } else if (pid == 0 && (ti->options & WNOHANG)) {
      cpu_channel_send(ti->state, ti->c, ti->channel, Qnil);
    } else {
      skip = 1;
    }

    tnext = ti->next;
    if (!skip) {
      _cpu_event_unregister_info(ti->state, ti);
      XFREE(ti);
    }
    ti = tnext;
  }
}

void _cpu_find_waiters_cb(int fd, short event, void *arg) {
  struct thread_info *sti = (struct thread_info*)arg;

  cpu_find_waiters(sti->state);
}

void cpu_event_wait_child(STATE, cpu c, OBJECT channel, int pid, int flags) {
  struct thread_info *ti;

  ti = ALLOC_N(struct thread_info, 1);
  ti->fd = 0;
  ti->state = state;
  ti->c = c;
  ti->pid = pid;
  ti->options = flags;
  ti->type = WAITER;
  ti->channel = channel;
  _cpu_event_register_info(state, ti);
  cpu_find_waiters(state); // run once right away, in case no children match
}

void cpu_event_setup_children(STATE, cpu c) {
  struct thread_info *ti;

  ti = ALLOC_N(struct thread_info, 1);
  ti->fd = 0;
  ti->state = state;
  ti->c = c;
  
  signal_set(&ti->ev, SIGCHLD, _cpu_find_waiters_cb, (void*)ti);
  event_base_set((struct event_base*)state->event_base, &ti->ev);
  
  signal_add(&ti->ev, NULL);  
}


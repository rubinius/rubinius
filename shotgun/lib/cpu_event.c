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
#include <ev.h>

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
typedef void (*stopper_cb)(EV_P_ void *);

struct thread_info {
  STATE;
  cpu c;
  OBJECT channel;
  union {
    struct ev_io io;
    struct ev_signal signal;
    struct ev_timer timer;
  } ev;
  OBJECT buffer;
  int count;
  pid_t pid;
  int fd;
  union {
    int options;
    int sig;
  };
  stopper_cb stopper;
  thread_info_type type;
  struct thread_info *prev, *next;
};

void cpu_event_init(STATE) {

  /* libev will not "autodetect" kqueue because it is broken on darwin */
  state->event_base = ev_default_loop(EVFLAG_FORKCHECK);
  state->thread_infos = NULL;
}

void cpu_event_run(STATE) {
  ev_loop(state->event_base, EVLOOP_ONESHOT);
}

void cpu_event_runonce(STATE) {
  ev_loop(state->event_base, EVLOOP_ONESHOT | EVLOOP_NONBLOCK);
}

void cpu_event_each_channel(STATE, OBJECT (*cb)(STATE, void*, OBJECT),
                            void *cb_data) {
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

static void _cpu_event_register_info(STATE, struct thread_info *ti) {
  state->pending_events++;
  ti->prev = NULL;
  ti->next = state->thread_infos;
  if(state->thread_infos) {
    ((struct thread_info*)state->thread_infos)->prev = ti;
  }
  state->thread_infos = ti;
}

static void _cpu_event_unregister_info(STATE, struct thread_info *ti) {
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

  if (ti->stopper) {
    ti->stopper(state->event_base, &ti->ev);
  }
  state->pending_events--;
  XFREE(ti);
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
    }
    ti = tnext;
  }

}

static void
_cpu_wake_channel_for_timer(EV_P_ struct ev_timer *ev, int revents) {
  struct thread_info *ti = (struct thread_info*)ev->data;
  cpu_channel_send(ti->state, ti->c, ti->channel, Qnil);
  _cpu_event_unregister_info(ti->state, ti);
}

static void
_cpu_wake_channel_for_writable(EV_P_ struct ev_io *ev, int revents) {
  struct thread_info *ti = (struct thread_info*)ev->data;
  cpu_channel_send(ti->state, ti->c, ti->channel, Qnil);
  _cpu_event_unregister_info(ti->state, ti);
}

static void
_cpu_wake_channel_and_read(EV_P_ struct ev_io *ev, int revents) {
  STATE;
  size_t sz, total, offset;
  ssize_t i;
  char *buf;
  OBJECT ret, ba, enc;
  struct thread_info *ti = (struct thread_info*)ev->data;
  
  state = ti->state;
  
  if(NIL_P(ti->buffer)) {
    ret = I2N(ti->fd);
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
      i = read(ti->fd, buf, sz);
      if(i == 0) {
        ret = Qnil;
      } else if(i == -1) {
        /* If we read and got nothing, go again. We must get something.
           It might be better to re-schedule this in libev and try again,
           but libev just said SOMETHING was there... */
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
}

/* Doesn't clear its own data, since it's going to be called a lot. */
//DELETEME
static void _cpu_wake_channel_alot(int fd, short event, void *arg) {
  STATE;
  struct thread_info *ti = (struct thread_info*)arg;

  state = ti->state;
  cpu_channel_send(state, ti->c, ti->channel, I2N((int)event));
}

/* Doesn't clear its own data, since it's going to be called a lot. */
static void
_cpu_wake_channel_for_signal(EV_P_ struct ev_signal *ev,
                             int revents) {
  struct thread_info *ti = (struct thread_info*)ev->data;
  cpu_channel_send(ti->state, ti->c, ti->channel, ti->c->current_thread);
}


void cpu_event_wake_channel(STATE, cpu c, OBJECT channel, double seconds) {
  struct thread_info *ti;

  ti = ALLOC_N(struct thread_info, 1);
  ti->fd = 0;
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  ti->stopper = (stopper_cb)ev_timer_stop;
  _cpu_event_register_info(state, ti);

  ev_timer_init(&ti->ev.timer, _cpu_wake_channel_for_timer,
                (ev_tstamp)seconds, 0);
  ti->ev.timer.data = ti;
  ev_timer_start(state->event_base, &ti->ev.timer);
}

void cpu_event_wait_readable(STATE, cpu c, OBJECT channel, int fd,
                             OBJECT buffer, int count) {
  struct thread_info *ti;

  ti = ALLOC_N(struct thread_info, 1);
  ti->fd = fd;
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  ti->buffer = buffer;
  ti->count = count;
  ti->stopper = (stopper_cb)ev_io_stop;
  //event_base_set((struct event_base*)state->event_base, &ti->ev);

  _cpu_event_register_info(state, ti);
  ev_io_init(&ti->ev.io, _cpu_wake_channel_and_read, fd, EV_READ);
  ti->ev.io.data = ti;
  ev_io_start(state->event_base, &ti->ev.io);
}

void cpu_event_wait_writable(STATE, cpu c, OBJECT channel, int fd) {
  struct thread_info *ti;

  ti = ALLOC_N(struct thread_info, 1);
  ti->fd = fd;
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  ti->stopper = (stopper_cb)ev_io_stop;
  _cpu_event_register_info(state, ti);

  //event_base_set((struct event_base*)state->event_base, &ti->ev);
  ev_io_init(&ti->ev.io, _cpu_wake_channel_for_writable, fd, EV_WRITE);
  ti->ev.io.data = ti;
  ev_io_start(state->event_base, &ti->ev.io);
}

void cpu_event_wait_signal(STATE, cpu c, OBJECT channel, int sig) {
  struct thread_info *ti;

  ti = (struct thread_info*)(state->thread_infos);
  while (ti) {
    if (ti->type == SIGNAL && ti->sig == sig) {
      cpu tc = ti->c;
      STATE = ti->state;
      OBJECT tchan = ti->channel;
      _cpu_event_unregister_info(state, ti);
      cpu_channel_send(state, tc, tchan, Qnil);
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
  ti->stopper = (stopper_cb)ev_signal_stop;
  _cpu_event_register_info(state, ti);

  ev_signal_init(&ti->ev.signal, _cpu_wake_channel_for_signal, sig);
  //event_base_set((struct event_base*)state->event_base, &ti->ev);
  ti->ev.signal.data = ti;
  ev_signal_start(state->event_base, &ti->ev.signal);
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
    while((pid = waitpid(ti->pid, &status, WNOHANG || ti->options)) <= -1
          && errno == EINTR)
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
    }
    ti = tnext;
  }
}


static void
_cpu_find_waiters_cb(EV_P_ struct ev_signal *w, int revents) {
  struct thread_info *ti = (struct thread_info*)w->data;

  cpu_find_waiters(ti->state);
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
  ti->stopper = NULL;
  _cpu_event_register_info(state, ti);

  cpu_find_waiters(state); // run once right away, in case SIGCHLD has already been caught
}

void cpu_event_setup_children(STATE, cpu c) {
  struct thread_info *ti;

  ti = ALLOC_N(struct thread_info, 1);
  ti->fd = 0;
  ti->state = state;
  ti->c = c;

  //event_base_set((struct event_base*)state->event_base, &ti->ev);

  ev_signal_init(&ti->ev.signal, _cpu_find_waiters_cb, SIGCHLD);
  ti->ev.signal.data = ti;
  ev_signal_start(state->event_base, &ti->ev.signal);
}

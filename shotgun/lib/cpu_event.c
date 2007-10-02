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
#include "config.h"

struct thread_info {
  STATE;
  cpu c;
  struct event ev;
  OBJECT channel;
  OBJECT buffer;
  int count;
  pid_t pid;
  struct thread_info *prev, *next;
};

void cpu_event_init(STATE) {

/* kqueue is broken on darwin, so we have to disable it's use. */
#if CONFIG_DISABLE_KQUEUE
  setenv("EVENT_NOKQUEUE", "1", 1);
  setenv("EVENT_NOPOLL", "1", 1);
#endif
  state->event_base = event_init();
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
  struct thread_info *top = (struct thread_info*)state->thread_infos;
  
  state->pending_events++;
  if(!state->thread_infos) {
    state->thread_infos = (void*)ti;
    ti->prev = ti->next = NULL;
  } else {
    top->next = ti;
    ti->prev = top;
    ti->next = NULL;
  }  
}

void _cpu_event_unregister_info(STATE, struct thread_info *ti) {
  if(!ti->prev) {
    if(ti->next) {
      state->thread_infos = ti->next;
      ti->next->prev = NULL;
    } else {
      state->thread_infos = NULL;
    }
  } else {
    ti->prev->next = ti->next;
    if(ti->next) {
      ti->next->prev = ti->prev;
    }
  }
  
  state->pending_events--;
}

void _cpu_wake_channel(int fd, short event, void *arg) {
  STATE;
  struct thread_info *ti = (struct thread_info*)arg;
  
  state = ti->state;
  
  cpu_channel_send(state, ti->c, ti->channel, I2N((int)event));
  _cpu_event_unregister_info(state, ti);
  free(ti);
}

void _cpu_wake_channel_and_read(int fd, short event, void *arg) {
  STATE;
  size_t sz, total;
  ssize_t i;
  char *buf;
  OBJECT ret, ba;
  struct thread_info *ti = (struct thread_info*)arg;
  
  state = ti->state;
  
  if(NIL_P(ti->buffer)) {
    ret = Qtrue;
  } else {
    ba = string_get_data(ti->buffer);
    sz = (size_t)ti->count;

    /* Clamp the read size so we don't overrun */
    total = SIZE_OF_BODY(ba) - 1;
    if(total < sz) {
      sz = total;
    }
    
    buf = bytearray_byte_address(state, ba);
    while(1) {
      i = read(fd, buf, sz);
      if(i == 0) {
        ret = Qnil; 
      } else if(i == -1) {
        /* If we read and got nothing, go again. We must get something. 
           It might be better to re-schedule this in libevent and try again,
           but libevent just said SOMETHING was there... */
        if(errno == EINTR) continue;
        ret = Qfalse;
      } else {        
        buf[i] = 0;
        string_set_bytes(ti->buffer, I2N(i));
        
        ret = I2N(i);
      }
      break;
    }
  }
  
  cpu_channel_send(state, ti->c, ti->channel, ret);
  
  _cpu_event_unregister_info(state, ti);
  free(ti);
}

/* Doesn't clear it's own data, since it's going to be called a lot. */
void _cpu_wake_channel_alot(int fd, short event, void *arg) {
  STATE;
  struct thread_info *ti = (struct thread_info*)arg;
    
  state = ti->state;
  cpu_channel_send(state, ti->c, ti->channel, I2N((int)event));
}


void cpu_event_wake_channel(STATE, cpu c, OBJECT channel, struct timeval *tv) {
  struct thread_info *ti;

  ti = calloc(1, sizeof(struct thread_info));
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  _cpu_event_register_info(state, ti);
  
  evtimer_set(&ti->ev, _cpu_wake_channel, (void*)ti);
  evtimer_add(&ti->ev, tv);
}

/* TODO: fd needs to be a typedef, set per platform. */
void cpu_event_wait_readable(STATE, cpu c, OBJECT channel, int fd, OBJECT buffer, int count) {
  struct thread_info *ti;

  ti = calloc(1, sizeof(struct thread_info));
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  ti->buffer = buffer;
  ti->count = count;
  
  event_set(&ti->ev, fd, EV_READ, _cpu_wake_channel_and_read, (void*)ti);
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
      free(ti);
    }
  } else {
    _cpu_event_register_info(state, ti);
  }
}

/* TODO: fd needs to be a typedef, set per platform. */
void cpu_event_wait_writable(STATE, cpu c, OBJECT channel, int fd) {
  struct thread_info *ti;

  ti = calloc(1, sizeof(struct thread_info));
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  _cpu_event_register_info(state, ti);
  
  event_set(&ti->ev, fd, EV_WRITE, _cpu_wake_channel, (void*)ti);
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
      free(ti);
    }
  } else {
    _cpu_event_register_info(state, ti);
  }
}

void cpu_event_wait_signal(STATE, cpu c, OBJECT channel, int sig) {
  struct thread_info *ti;

  ti = calloc(1, sizeof(struct thread_info));
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  _cpu_event_register_info(state, ti);
    
  signal_set(&ti->ev, sig, _cpu_wake_channel_alot, (void*)ti);
  signal_add(&ti->ev, NULL);  
}

void _cpu_find_waiters(int fd, short event, void *arg) {
  STATE;
  pid_t pid;
  int status, found;
  OBJECT ret;
  struct thread_info *ti;
  
  struct thread_info *sti = (struct thread_info*)arg;
  
  state = sti->state;
    
  while((pid = waitpid(-1, &status, WNOHANG)) != 0) {
    if(pid == -1) {
      if(errno == EINTR) continue;
      break;
    }
    
    ti = (struct thread_info*)(state->thread_infos);
    found = 0;
    
    while(ti) {
      if(ti->pid == pid) {
        state = ti->state;
        found = 1;
        if(WIFEXITED(status)) {
          ret = I2N(WEXITSTATUS(status));
        } else {
          /* Could support WIFSIGNALED also. */
          ret = Qtrue;
        }
        cpu_channel_send(state, ti->c, ti->channel, ret);
        _cpu_event_unregister_info(state, ti);
        free(ti);
        break;
      }
      ti = ti->next;
    }
    
    if(!found) {
      hash_set(state, state->global->recent_children, I2N(pid), I2N(status));
    }
  }  
}

void cpu_event_wait_child(STATE, cpu c, OBJECT channel, int pid) {
  struct thread_info *ti;
  int status;
  OBJECT ret, tmp;
  pid_t out;
    
  /* Check if the child has already exited. */
  out = waitpid((pid_t)pid, &status, WNOHANG);
  if(out > 0) {
    if(WIFEXITED(status)) {
      ret = I2N(WEXITSTATUS(status));
    } else {
      /* Could support WIFSIGNALED also. */
      ret = Qtrue;
    }
    cpu_channel_send(state, c, channel, ret);
    return;
  }
  
  /* Child exited recently and we already caught the SIGCHLD */
  tmp = hash_find(state, state->global->recent_children, I2N(pid));
  if(!NIL_P(tmp)) {
    status = FIXNUM_TO_INT(tmp);
    
    if(WIFEXITED(status)) {
      ret = I2N(WEXITSTATUS(status));
    } else {
      /* Could support WIFSIGNALED also. */
      ret = Qtrue;
    }
    
    cpu_channel_send(state, c, channel, ret);
    return;
  }
  
  /* No children out there. */
  if(out == -1 && errno == ECHILD) {
    cpu_channel_send(state, c, channel, Qfalse);
  } else {
    /* Still somewhere out there, register we want it. */
    ti = calloc(1, sizeof(struct thread_info));
    ti->state = state;
    ti->c = c;
    ti->pid = pid;
    ti->channel = channel;
    _cpu_event_register_info(state, ti);
  }
}

void cpu_event_setup_children(STATE, cpu c) {
  struct thread_info *ti;

  state->global->recent_children = hash_new(state);
  ti = calloc(1, sizeof(struct thread_info));
  ti->state = state;
  ti->c = c;
  signal_set(&ti->ev, SIGCHLD, _cpu_find_waiters, (void*)ti);
  signal_add(&ti->ev, NULL);  
}


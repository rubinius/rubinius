#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
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
#include <glib.h>

#include "rubinius.h"

struct thread_info {
  STATE;
  cpu c;
  struct event ev;
  OBJECT channel;
  struct thread_info *prev, *next;
};

void cpu_event_init(STATE) {
  state->event_base = event_init();
}

void cpu_event_run(STATE) {
  event_base_loop(state->event_base, EVLOOP_ONCE);
}

void cpu_event_each_channel(STATE, OBJECT (*cb)(STATE, void*, OBJECT), void *cb_data) {
  struct thread_info *ti = (struct thread_info*)state->thread_infos;
  while(ti) {
    ti->channel = cb(state, cb_data, ti->channel);
    ti = ti->next;
  }
}

void _cpu_event_register_info(STATE, struct thread_info *ti) {
  struct thread_info *top = (struct thread_info*)state->thread_infos;
  
  if(!state->thread_infos) {
    state->thread_infos = (void*)ti;
    ti->prev = ti->next = NULL;
  } else {
    top->next = ti;
    ti->prev = top;
    ti->next = NULL;
    state->thread_infos = (void*)ti;
  }
}

void _cpu_event_unregister_info(STATE, struct thread_info *ti) {
  struct thread_info *tmp;
  
  if(!ti->prev) {
    state->thread_infos = NULL;
  } else {
    ti->prev->next = ti->next;
    if(ti->next) {
      ti->next->prev = ti->prev;
    }
  }
}

void _cpu_wake_channel(int fd, short event, void *arg) {
  STATE;
  struct thread_info *ti = (struct thread_info*)arg;
  
  state = ti->state;
  
  cpu_channel_send(state, ti->c, ti->channel, I2N((int)event));
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
void cpu_event_wait_readable(STATE, cpu c, OBJECT channel, int fd) {
  struct thread_info *ti;

  ti = calloc(1, sizeof(struct thread_info));
  ti->state = state;
  ti->c = c;
  ti->channel = channel;
  _cpu_event_register_info(state, ti);
  
  event_set(&ti->ev, fd, EV_READ, _cpu_wake_channel, (void*)ti);
  event_add(&ti->ev, NULL);  
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
  event_add(&ti->ev, NULL);  
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

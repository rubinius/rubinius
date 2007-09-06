#include "shotgun.h"
#include "cpu.h"
#include "methctx.h"
#include <sys/time.h>
#include <time.h>
#include "machine.h"
#include "tuple.h"
#include "string.h"
#include "methctx.h"

extern machine current_machine;

#define SAMPLE_INCS 1024

void cpu_sampler_init(STATE, cpu c) {
  state->max_samples = SAMPLE_INCS;
  state->cur_sample = 0;
  state->samples = NULL;
}

void cpu_sampler_collect(STATE, OBJECT (*cb)(STATE, void*, OBJECT), void *cb_data) {
  int i;
  OBJECT obj;
  if(!state->samples) return;
  
  for(i = 0; i < state->cur_sample; i++) {
    if(REFERENCE_P(state->samples[i])) {
      obj = cb(state, cb_data, state->samples[i]);
      state->samples[i] = obj;
    }
  }
}

/* 2 other recorders, not using them currently.

void _cpu_sampler_record_method_object(int sig) {
  STATE;
  cpu c;
  state = current_machine->s;
  c = current_machine->c;
  state->samples[state->cur_sample++] = cpu_current_method(state, c);
  
  if(state->cur_sample == state->max_samples) {
    state->max_samples += SAMPLE_INCS;
    state->samples = (OBJECT*)realloc(state->samples, state->max_samples * sizeof(OBJECT));
  }
}

void _cpu_sampler_record_method_name(int sig) {
  STATE;
  cpu c;
  OBJECT tup;
  
  state = current_machine->s;
  c = current_machine->c;
  
  if(!state->samples) return;
    
  tup = tuple_new2(state, 3, c->self, cpu_current_module(state, c), cpu_current_name(state, c));
  state->samples[state->cur_sample++] = tup;
  
  if(state->cur_sample == state->max_samples) {
    state->max_samples += SAMPLE_INCS;
    state->samples = (OBJECT*)realloc(state->samples, state->max_samples * sizeof(OBJECT));
  }
}

*/

void _cpu_sampler_record_context(int sig) {
  STATE;
  cpu c;
  
  state = current_machine->s;
  c = current_machine->c;
  
  if(!state->samples) return;
  
  /* We don't recycle context's even still, but when we do, rather than using
     this, we should tell the cpu to just disable all recycling while
     we sample. */
  methctx_reference(state, c->active_context);
  state->samples[state->cur_sample++] = c->active_context;
  
  /* Add more space for samples. */
  if(state->cur_sample == state->max_samples) {
    state->max_samples += SAMPLE_INCS;
    state->samples = (OBJECT*)realloc(state->samples, state->max_samples * sizeof(OBJECT));
  }
}


void cpu_sampler_suspend(STATE) {
  if(state->samples) {
    signal(SIGPROF, SIG_IGN);
  }
}

void cpu_sampler_resume(STATE) {
  if(state->samples) {
    signal(SIGPROF, _cpu_sampler_record_context);
  }
}

/* move into state. */
static int interval;

void cpu_sampler_activate(STATE, int hz) {
  struct itimerval new, old;
  new.it_interval.tv_usec = 1000000 / hz;
  new.it_interval.tv_sec = 0;
  new.it_value.tv_usec = new.it_interval.tv_usec;
  new.it_value.tv_sec = 0;
  
  interval = new.it_interval.tv_usec;
  
  state->max_samples = SAMPLE_INCS;
  state->cur_sample = 0;
  state->samples = (OBJECT*)calloc(state->max_samples, sizeof(OBJECT));
    
  cpu_sampler_resume(state);
  setitimer(ITIMER_PROF, &new, &old);
}

OBJECT cpu_sampler_disable(STATE) {
  OBJECT tup;
  int i;
  struct itimerval new, old;
  clock_t fin;
  
  fin = clock();
  
  new.it_interval.tv_usec = 0;
  new.it_interval.tv_sec = 0;
  new.it_value.tv_usec = 0;
  new.it_value.tv_sec = 0;
  
  cpu_sampler_suspend(state);
  setitimer(ITIMER_PROF, &new, &old);
    
  if(!state->samples) return Qnil;
  tup = tuple_new(state, state->cur_sample);
  for(i = 0; i < state->cur_sample; i++) {
    assert(ISA(state->samples[i], state->global->fastctx) ||
      ISA(state->samples[i], state->global->blokctx));
    tuple_put(state, tup, i, state->samples[i]);
  }
  
  free(state->samples);
  state->samples = NULL;
  
  return tuple_new2(state, 3, tup, I2N((int)fin), I2N(interval));
}

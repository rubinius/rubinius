#include <time.h>
#include <sys/time.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/methctx.h"
#include "shotgun/lib/machine.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/methctx.h"

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

  /* If we weren't doing anything, nothing to do. */
  if(NIL_P(c->active_context)) return;
  
  /* We don't recycle context's even still, but when we do, rather than using
     this, we should tell the cpu to just disable all recycling while
     we sample. */
  methctx_reference(state, c->active_context);
  if(c->in_primitive) {
    state->samples[state->cur_sample++] = APPLY_TAG(c->in_primitive, TAG_FIXNUM);
  } else {
    state->samples[state->cur_sample++] = c->active_context;
  }
  
  /* Add more space for samples. */
  if(state->cur_sample == state->max_samples) {
    OBJECT* chunk;
    state->max_samples += SAMPLE_INCS;
    chunk = (OBJECT*)realloc(state->samples, state->max_samples * SIZE_OF_OBJECT);
    if(chunk != state->samples) {
      // free(state->samples);
    }
    state->samples = chunk;    
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

struct sampler_args {
  int hz;
  pthread_t other;
  unsigned int* active;
  int* in_gc;
  uint32_t gc_cycles;
};

void *_cpu_thread_tick(void* blah) {
  struct timespec spec;
  struct timespec actual;

  struct sampler_args* args = (struct sampler_args*)blah;

  spec.tv_sec = 0;
  /* The 3 is here because it seems like the constant overhead of
   * everything is about 3x. Thus if we say 100 hz, we only get 30
   * samples, 1000hz, 300some samples. By reducing it by 3, we get
   * nearly the right number each time. */
  spec.tv_nsec = 1000000000 / args->hz / 3;

  unsigned int* active = args->active;
  int* in_gc = args->in_gc;
  uint32_t gc_cycles = 0;

  while(*active) {
    nanosleep(&spec, &actual);
    if(*in_gc) {
      gc_cycles++;
    } else {
      pthread_kill(args->other, SIGPROF);
    }
  }

  args->gc_cycles = gc_cycles;

  pthread_exit(blah);
  return blah;
}

void cpu_sampler_activate(STATE, int hz, machine m) {
  state->max_samples = SAMPLE_INCS;
  state->cur_sample = 0;
  state->samples = ALLOC_N(OBJECT, state->max_samples);
    
  cpu_sampler_resume(state);
  
  m->sampler_active = 1;
  struct sampler_args *args = ALLOC_N(struct sampler_args, 1);
  
  args->hz = hz;
  args->other = pthread_self();
  args->active = &m->sampler_active;
  args->in_gc = &state->in_gc;
  
  pthread_create(&m->sampler_thread, NULL, _cpu_thread_tick, (void*)args);
}

OBJECT cpu_sampler_disable(STATE, machine m) {
  OBJECT tup;
  int i;
  clock_t fin;
  
  fin = clock();
 
  /* tell the other thread to stop, then join it. */
  m->sampler_active = 0;
  struct sampler_args* args;
  pthread_join(m->sampler_thread, (void*)&args);
  
  cpu_sampler_suspend(state);
    
  if(!state->samples) return Qnil;
  tup = tuple_new(state, state->cur_sample);
  for(i = 0; i < state->cur_sample; i++) {
    tuple_put(state, tup, i, state->samples[i]);
  }
  
  XFREE(state->samples);
  state->samples = NULL;
  
  tup = tuple_new2(state, 3, tup, I2N((int)fin), I2N(args->gc_cycles));
  free(args);
  return tup;
}

#include "shotgun.h"
#include "nmc.h"

#include <string.h>

#include "string.h"
#include "cpu.h"
#include "methctx.h"
#undef SYMBOL_P
#include "ruby.h"

/* TODO: replace this static with a pthread local */
static rni_context* global_context = NULL;

void subtend_setup_global() {
  if(!global_context) {
    global_context = calloc(1, sizeof(rni_context));
  }
}

void subtend_set_context(STATE, cpu c, rni_nmc *nmc) {
  global_context->state = state;
  global_context->cpu = c;
  global_context->nmc = nmc;
}

rni_context* subtend_retrieve_context() {
  return global_context;
}

OBJECT nmc_new(STATE, OBJECT nmethod, OBJECT sender, OBJECT recv, OBJECT name, int args) {
  rni_nmc *n;
  OBJECT ctx, sys;
  struct fast_context *fc;
  
  ctx = object_memory_new_context(state->om);
  if(ctx >= state->om->context_last) {
    state->om->collect_now |= OMCollectYoung;
  }
  
  if(state->excessive_tracing) {
    printf("CTX:           block running %d\n", (int)ctx);
  }
  
  CLEAR_FLAGS(ctx);
  ctx->field_count = FASTCTX_FIELDS;
    
  fc = FASTCTX(ctx);
  fc->sender = sender;
  fc->ip = 0;
  fc->sp = 0;
  fc->name = name;
  fc->self = recv;
  fc->method = nmethod;  
  fc->block = (OBJECT)Qnil;
  fc->literals = (OBJECT)Qnil;
  fc->method_module = (OBJECT)Qnil;
  fc->locals = (OBJECT)Qnil;
  fc->argcount = args;
  fc->type = FASTCTX_NMC;
  
  n = nmc_new_standalone();
  sys = nmethod_get_data(nmethod);
  n->method = DATA_STRUCT(sys, native_method*);
  
  fc->opaque_data = (void*)n;
  
  ctx->ForeverYoung = TRUE;
    
  return ctx;
}

rni_nmc *nmc_new_standalone() {
  rni_nmc *n;
  
  n = calloc(1, sizeof(rni_nmc));
  
  n->num_handles = 16;
  n->used = 0;
  n->handles = calloc(n->num_handles, sizeof(rni_handle*));
  n->system_set = 0;
  n->cont_set = 0;
    
  return n;
}

void nmc_enlarge(rni_nmc *n) {  
  n->num_handles += 8;
  n->handles = realloc(n->handles, sizeof(rni_handle*) * n->num_handles);
}

rni_handle *nmc_handle_new(rni_nmc *n, rni_handle_table *tbl, OBJECT obj) {
  rni_handle *h;
  
  if(!REFERENCE_P(obj)) {
    return (rni_handle*)obj;
  }
  
  h = handle_new(tbl, obj);
  n->handles[n->used++] = h;
  if(n->used == n->num_handles) {
    nmc_enlarge(n);
  }
  
  return h;
}

void nmc_cleanup(rni_nmc *nmc, rni_handle_table *tbl) {
  int i;
  rni_handle *h;
  for(i = 0; i < nmc->used; i++) {
    h = nmc->handles[i];
    if(!handle_is_global(h)) {
      handle_remove(tbl, h);
      handle_delete(h);
    }
  }
  
  if(nmc->stack) {
    free(nmc->stack);
    nmc->stack = NULL;
  }
  
  if(nmc->local_data) {
    free(nmc->local_data);
  }
  
  nmc->used = 0;
}

void nmc_delete(rni_nmc *nmc) {
  free(nmc->handles);
  free(nmc);
}

void _nmc_start() {
  rni_nmc *n;
  struct fast_context *fc;
  cpu c;
  rni_handle *retval, *recv;  
  int i;
  void **args = NULL;
  VALUE *va = NULL;
  rni_handle **handles_used = NULL;
  STATE;
  
  long *data;
    
  n =  global_context->nmc;
  fc = global_context->fc;
  c =  global_context->cpu;
  state = global_context->state;

  recv = nmc_handle_new(n, global_context->state->handle_tbl, fc->self);
  
  va = NULL;
  args = NULL;
  
  /*
  #ifndef __i386__
  if(n->method->stub) {
    retval = n->method->stub(recv);
    goto done;
  }
  #endif
  */
  
  rni_handle* (*func)() = (rni_handle* (*)())(n->method->entry);
  int hs, ch;
  if(n->method->args == -2) {
    hs = 2;
  } else if(n->method->args == -1) {
    hs = 3;
  } else {
    hs = n->method->args + 2;
  }
  
  data = (void*)calloc(hs, sizeof(rni_handle*));
  n->local_data = data;
  handles_used = (rni_handle**)data;
  
  ch = 0;
  handles_used[ch++] = recv;
  
#define nha() ({ rni_handle *_h = nmc_handle_new(n, global_context->state->handle_tbl, cpu_stack_pop(global_context->state, c)); handles_used[ch++] = _h; _h; })

  retval = (rni_handle*)Qnil;
  
  if(n->method->args == -2) {
    OBJECT rargs = array_new(state, fc->argcount);
    rni_handle *ah;
    
    for(i = 0; i < fc->argcount; i++) {
      array_set(state, rargs, i, cpu_stack_pop(state, c));
    }
    
    ah = nmc_handle_new(n, global_context->state->handle_tbl, rargs);
    handles_used[ch++] = ah;
    
    retval = (*func)(recv, ah);
  } else if(n->method->args == -1) {
    rni_handle **args;
    
    args = &handles_used[ch];
    
    for(i = 0; i < fc->argcount; i++) {
      nha();
    }
    
    retval = (*func)(recv, fc->argcount, args);
  } else {
    int i, start;
    
    start = ch;
#define rh(which) handles_used[which + start]
    
    for(i = 0; i < n->method->args; i++) { 
      handles_used[ch++] = nmc_handle_new(n, global_context->state->handle_tbl,
          cpu_stack_pop(global_context->state, c));
    }
            
    switch(n->method->args) {
    case 0:
      retval = (*func)(recv);
      break;
    case 1:
      retval = (*func)(recv, rh(0));
      break;
    case 2:
      retval = (*func)(recv, rh(0), rh(1));
      break;
    case 3:
      retval = (*func)(recv, rh(0), rh(1), rh(2));
      break;
    case 4:
      retval = (*func)(recv, rh(0), rh(1), rh(2), rh(3));
      break;
    case 5:
      retval = (*func)(recv, rh(0), rh(1), rh(2), rh(3), rh(4));
      break;
    case 6:
      retval = (*func)(recv, rh(0), rh(1), rh(2), rh(3), rh(4), rh(5));
      break;
    case 7:
      abort();
    }
  }
  
  /*
  #ifndef __i386__
    done:
  #endif
  */
  
  /*
  if(args) free(args);
  if(va) free(va);
  if(handles_used) free(handles_used);
  */
  
  n->cont_set--;
  n->system_set--;
  
  n->value = retval;
  
  /* Switch back to the main stack, into nmc_activate to finish up. */
  n->jump_val = ALL_DONE;
  setcontext(&n->system);
}

void nmc_activate(STATE, cpu c, OBJECT nmc, OBJECT val, int reraise) {
  int travel;
  rni_nmc *n;
  struct fast_context *fc;
  
  fc = FASTCTX(nmc);
  
  n = (rni_nmc*)fc->opaque_data;
  
  global_context->state = state;
  global_context->cpu = c;
  global_context->nmc = n;
  global_context->fc = fc;
      
  /* Reraise indicates that this request to activate is actually
     because an exception is trying to propagate up. Currently,
     we just restore the system, cleanup, and raise the 
     exception again. */
  if(reraise && n->system_set) {
    n->jump_val = CLEANUP;
    setcontext(&n->system);
  }
  
  n->system_set++;
  getcontext(&n->system);
  travel = n->jump_val;
  n->jump_val = 0;
  
  /* If we haven't traveled yet, call the method. */
  if(!travel) {
    c->self = fc->self;
    c->type = fc->type;
    
    c->home_context = nmc;
    c->active_context = nmc;
    
    /* Oh! It's already running, lets reactivate it. */
    if(n->cont_set) {
      /* Grab the return value and make a handle for it. */
      n->value = nmc_handle_new(n, state->handle_tbl, val);
      
      /* Go go gadget stack! */
      n->jump_val = 1;
      setcontext(&n->cont);
      /* You'll never get here because the stack has been restored
         to where it was before. */
    }
 
    n->cont_set++;
    n->stack_size = 65536;
    n->stack = (void*)calloc(1, n->stack_size + 16);

    getcontext(&n->cont);
    n->cont.uc_stack.ss_sp = n->stack;
    n->cont.uc_stack.ss_size = n->stack_size;
#ifndef PORTABLEUCONTEXT_DEFINED
    n->cont.uc_stack.ss_flags = 0;
    n->cont.uc_link = NULL;
#endif

    makecontext(&n->cont, _nmc_start, 0);
    setcontext(&n->cont);
  /* Oh, we have traveled back here! Lets figure out why! */
  } else {
    OBJECT tmp;
    
    // printf("Welcome back! %d\n", travel);
    switch(travel) {
    case CLEANUP:
      cpu_raise_exception(state, c, c->exception);
      break;
      
    case RAISED_EXCEPTION:
      tmp = handle_to_object(state, state->handle_tbl, n->value);
      cpu_raise_exception(state, c, tmp);
      break;

    case CALL_METHOD:
      // c->active_context = nmc;
      // n->setup_context++;
      /* With call method, the rb_funcall shim pushs the arguments
         on the stack already, so we just have to perform the send. */
      tmp = handle_to_object(state, state->handle_tbl, n->value);
      cpu_unified_send(state, c, tmp, n->symbol, n->args, (OBJECT)Qnil);
      break;
    case SEGFAULT_DETECTED:
      {
        char msg[1024];
        OBJECT cur;
        sprintf(msg, "Segfault detected in function %p (accessing %p)", 
            n->method->entry, global_context->fault_address);
            
        /* We swap around the active_conetext so the exception thats created
           references the NativeMethodContext, but raise_exception doesn't
           see it so we go directly up. */
        cur = c->active_context;
        c->active_context = nmc;
        tmp = cpu_new_exception(state, c, BASIC_CLASS(exc_segfault), msg);
        nmc_cleanup(n, state->handle_tbl);
        n->stack = NULL;
        nmc_delete(n);
        fc->opaque_data = NULL;
        c->active_context = cur;
        cpu_raise_exception(state, c, tmp);
        return;
      }
      
    case ALL_DONE:
      /* Push the return value onto the stack. */
      tmp = handle_to_object(global_context->state, global_context->state->handle_tbl, n->value);
      nmc_cleanup(n, state->handle_tbl);
      n->stack = NULL;
      cpu_return_to_sender(state, c, tmp, 0, FALSE);
      nmc_delete(n);
      fc->opaque_data = NULL;
      return;
      
      
    default:
      printf("Unknown travel plans %d!\n", travel);
      abort();
    }
  }
  
  global_context->nmc = NULL;
  global_context->fc =  NULL;
}

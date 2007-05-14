#include "shotgun.h"
#include "nmethod.h"
#include "symbol.h"
#include "object.h"
#include "nmc.h"

#define JIT_NEED_PUSH_POP 1

#include <lightning.h>

#ifdef RBS_DISASS
#error 'disass'
void disassemble(FILE*, char*, char*);
#endif

static rni_handle* _ffi_pop() {
  rni_handle *h;
  rni_context *ctx = subtend_retrieve_context();
  h = nmc_handle_new(ctx->nmc, ctx->state->handle_tbl, cpu_stack_pop(ctx->state, ctx->cpu));
  return h;
}

#define ffi_call(func) jit_finish(func)
#define ffi_pop(reg) jit_prepare(0); jit_calli(_ffi_pop); jit_retval_p(reg);

char *ffi_generate_c_stub(STATE, int args, void *func) {
  char *start, *end;
  char *codebuf;
  int in, vars, i, aligned;
  int *ids;
    
  /* The stub is called with the receiver, so if there are no args, we
     can just use func as the stub. */
  if(args == 0) return func;
  
  /* Until lightning supports more than 6 args, we can only generate a stub
     for 5 args (0 is the receiver). */
  if(args > 5) return NULL;
  
  codebuf = (char*)malloc(512);
  start = codebuf;
  
  (void)jit_set_ip((jit_insn*)codebuf);
      
  jit_prolog(1);
  
  in = jit_arg_p();
  jit_getarg_p(JIT_V0, in);
  
  if(args == 0) {
    jit_prepare_i(1);
    jit_pusharg_p(JIT_V0);
    
  }
 /* else if(args == 1) {
    ffi_pop(JIT_V1);
    jit_prepare_i(2);
    jit_pusharg_p(JIT_V1);
    jit_pusharg_p(JIT_V0);
    
  } else if(args == 2) {
    ffi_pop(JIT_V1);
    ffi_pop(JIT_V2);
    jit_prepare_i(3);
    jit_pusharg_p(JIT_V2);
    jit_pusharg_p(JIT_V1);
    jit_pusharg_p(JIT_V0);
    
  } */ 
  else {
    
    ids = calloc(args, sizeof(int));
    
    for(i = 0; i < args; i++) {    
      ffi_pop(JIT_V1);
      ids[i] = jit_allocai(sizeof(void*));
      jit_stxi_p(ids[i], JIT_FP, JIT_V1);
    }
        
    jit_prepare_i(args + 1);
    
    for(i = args - 1; i >= 0; i--) {
      jit_ldxi_p(JIT_R2, JIT_FP, ids[i]);
      jit_pusharg_p(JIT_R2);
    }
            
    jit_pusharg_p(JIT_V0);
    free(ids);
  }    
  
  ffi_call(func);
  jit_retval_p(JIT_RET);  
  jit_ret();
  
  if(getenv("RBX_SHOW_STUBS")) {
    printf("Assembly stub for: %p (%d)\n", func, args);

    #ifdef RBS_DISASS
    disassemble(stderr, start, jit_get_ip().ptr);
    #endif
  }
  
  jit_flush_code(start, jit_get_ip().ptr);
  
  return start;
}

/******************************** -*- C -*- ****************************
 *
 *	Test `jit_allocai'
 *
 ***********************************************************************/


/* Contributed by Ludovic Courtès.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include "lightning.h"

typedef int (* int_return_int_t) (int);

static int
identity (int arg)
{
  printf ("received %i\n", arg);
  return arg;
}

static int_return_int_t
generate_function_proxy (int_return_int_t func)
{
  static const char failure_message[] = "numbers don't add up to zero\n";
  static char buffer[1024];

  int_return_int_t result;
  int arg, arg_offset, argneg_offset;
  jit_insn *branch;

  result = (int_return_int_t)(jit_set_ip (buffer).ptr);
  jit_prolog (1);
  arg = jit_arg_i ();
  jit_getarg_i (JIT_R1, arg);

  /* Store the argument on the stack.  */
  arg_offset = jit_allocai (sizeof (int));
  jit_stxi_i (arg_offset, JIT_FP, JIT_R1);

  /* Store the negative of the argument on the stack.  */
  argneg_offset = jit_allocai (sizeof (int));
  jit_negr_i (JIT_R2, JIT_R1);
  jit_stxi_i (argneg_offset, JIT_FP, JIT_R2);

  /* Invoke FUNC.  */
  jit_prepare (1);
  jit_pusharg_i (JIT_R1);
  (void)jit_finish (func);

  /* Ignore the result.  */

  /* Restore the negative and the argument from the stack.  */
  jit_ldxi_i (JIT_R2, JIT_FP, argneg_offset);
  jit_ldxi_i (JIT_V1, JIT_FP, arg_offset);

  /* Make sure they still add to zero.  */
  jit_addr_i (JIT_R0, JIT_V1, JIT_R2);
  branch = jit_bnei_i (jit_forward (), JIT_R0, 0);

  /* Return it.  */
  jit_movr_i (JIT_RET, JIT_V1);
  jit_ret ();

  /* Display a failure message.  */
  jit_patch (branch);
  jit_movi_p (JIT_R2, failure_message);
  jit_prepare (1);
  jit_pusharg_p (JIT_R2);
  jit_finish (printf);

  /* Leave.  */
  jit_movr_i (JIT_RET, JIT_V1);
  jit_ret ();

  jit_flush_code (buffer, jit_get_ip ().ptr);

  return result;
}


int
main (int argc, char *argv[])
{
  int_return_int_t identity_func;

  identity_func = generate_function_proxy (identity);
  if (identity_func (7777) != 7777)
    {
      printf ("failed: got %i instead of %i\n",
	      identity_func (7777), 7777);
      return 1;
    }
  else
    printf ("succeeded\n");

  return 0;
}

/*
   Local Variables:
   coding: latin-1
   End:
 */

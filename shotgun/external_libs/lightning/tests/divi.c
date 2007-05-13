/******************************** -*- C -*- ****************************
 *
 *	Test jit_divi_i
 *
 ***********************************************************************/


/* Contributed by Ludovic Courtès.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include "lightning.h"

typedef int (* divider_t) (int);

static divider_t
generate_divider (int operand, unsigned int *size)
{
  static char buffer[1024];
  divider_t result;
  int arg;

  result = (divider_t)(jit_set_ip (buffer).iptr);
  jit_leaf (1);
  arg = jit_arg_i ();
  jit_getarg_i (JIT_R1, arg);

  jit_divi_i (JIT_R2, JIT_R1, operand);
  jit_movr_i (JIT_RET, JIT_R2);

  jit_ret ();
  jit_flush_code (buffer, jit_get_ip ().ptr);

  *size = (char *)jit_get_ip ().ptr - buffer;

  return result;
}

static int
test_divider (int divisor, const int *operands, unsigned operand_count)
{
  int failed = 0;
  unsigned int op, size;
  divider_t div = generate_divider (divisor, &size);

  for (op = 0; op < operand_count; op++)
    {
      int result, expected;

      result = div (operands[op]);
      expected = operands[op] / divisor;
      printf ("%i/%i = %i (expected %i)\n", operands[op], divisor,
	      result, expected);

      if (result != expected)
	failed = 1;
    }

  return (!failed);
}



int
main (int argc, char *argv[])
{
  static const int small_ops[] = { 40, 64, 80 };
  static const int large_ops[] = { 98304, 65536, 163840 };

  /* Test a small immediate operand.  */
  if (!test_divider (8, small_ops, sizeof (small_ops) / sizeof (int)))
    return 1;

  /* Test a large immediate operand (requires more instructions on RISC
     processors).  */
  if (!test_divider (32768, large_ops, sizeof (large_ops) / sizeof (int)))
    return 1;

  return 0;
}

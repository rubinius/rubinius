/******************************** -*- C -*- ****************************
 *
 *	Test jit_ldxi_i
 *
 ***********************************************************************/


/* Contributed by Ludovic Courtes.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include "lightning.h"

typedef char (* loader_t) (int);

/* Check `ldxi' with a big operand (OPERAND is assumed to be ``big'', e.g.,
   more than one octet-long on PowerPC).  */
static loader_t
generate_ldxi_big_operand (const void *operand)
{
  static char buffer[1024];
  loader_t result;
  int arg;

  /* printf ("si?=%i ui?=%i\n", _siP (16, operand), _uiP (16, operand)); */

  result = (loader_t)(jit_set_ip (buffer).iptr);
  jit_leaf (1);
  arg = jit_arg_i ();
  jit_getarg_p (JIT_R1, arg);

  jit_ldxi_c (JIT_R0, JIT_R1, operand);
  jit_movr_i (JIT_RET, JIT_R0);

  jit_ret ();
  jit_flush_code (buffer, jit_get_ip ().ptr);

  return result;
}


int
main (int argc, char *argv[])
{
  static const char the_array[] = "GNU Lightning";
  char the_on_stack_array[] = "GNU Lightning";
  unsigned i;
  loader_t array_loader;
  const char *large_pointer;

  if (the_array > the_on_stack_array)
    large_pointer = the_array;
  else
    large_pointer = the_on_stack_array;

  array_loader = generate_ldxi_big_operand (large_pointer);

  for (i = 0; i < sizeof (the_array) - 1; i++)
    {
      printf ("array[%i] = %c, array_loader (%i) = %c\n",
	      i, large_pointer[i], i, array_loader (i));
      if (large_pointer[i] != array_loader (i))
	return 1;
    }

  return 0;
}

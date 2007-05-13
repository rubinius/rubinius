/******************************** -*- C -*- ****************************
 *
 *	Test jit_movi_p
 *
 ***********************************************************************/


/* Contributed by Ludovic Courtès.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include "lightning.h"

typedef void * (* mover_t) (void);

static mover_t
generate_movi (const void *operand)
{
  static char buffer[1024];
  mover_t result;

  /* printf ("si?=%i ui?=%i\n", _siP (16, operand), _uiP (16, operand)); */

  result = (mover_t)(jit_set_ip (buffer).iptr);
  jit_leaf (1);

  jit_movi_p (JIT_R0, operand);
  jit_movr_p (JIT_RET, JIT_R0);

  jit_ret ();
  jit_flush_code (buffer, jit_get_ip ().ptr);

  return result;
}


int
main (int argc, char *argv[])
{
  static const char the_array[] = "GNU Lightning";
  mover_t get_array;
  const void *large_pointer;

  if (the_array > (char *)&get_array)
    large_pointer = the_array;
  else
    large_pointer = &get_array;

  /* On RISC machines, moving a large immediate may require several
     instructions (e.g., `sethi' followed by `ori' on SPARC).  */
  get_array = generate_movi (large_pointer);

  if (get_array () == large_pointer)
    printf ("`jit_movi_p' succeeded\n");
  else
    printf ("`jit_movi_p' failed\n");

  return (get_array () != large_pointer);
}

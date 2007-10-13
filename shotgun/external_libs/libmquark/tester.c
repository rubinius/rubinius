#include "quark.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv)
{
   /* by default, 0 <-> NULL */
   assert(quark_from_string(NULL) == 0);
   assert(quark_to_string(0) == NULL);

   /* strings are internalized, setting the same value multiple times
    * will give the same quark */
   const char * foo = "foo";
   assert(quark_from_string(foo) == 1);
   assert(quark_from_string(foo) == 1);

   /* this is a copy of the string, unless the quark_from_static_string
    * is used */
   assert(quark_to_string(1) != foo);
   const char *bar = "bar";
   assert(quark_from_static_string(bar) == 2);
   assert(quark_to_string(2) == bar);

   /* this is the same for the same string content at a different address */
   char * foo2 = strdup(foo);
   assert(quark_from_string(foo2) == 1);
   assert(quark_to_string(1) != foo2);
   free(foo2);
 
   return 0;
}

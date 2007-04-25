#include "shotgun.h"
#include "cpu.h"
#include "module.h"
#include "methctx.h"
#include "class.h"
#include "string.h"
#include "hash.h"
#include "symbol.h"
#include "object.h"
#include "bytearray.h"
#include "tuple.h"
#include "regexp.h"
#include "archive.h"
#include "machine.h"
#include "grammar.h"
#include "subtend.h"
#include "subtend/nmc.h"
#include "fixnum.h"
#include "list.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <glib.h>
#include <zlib.h>
#include <math.h>
#include <glob.h>
#include <termios.h>

OBJECT math_sqrt(STATE, OBJECT a);

#define STATIC_SIZE 100

struct time_data {
  struct timeval tv;
  struct timezone tz;
};

#define MAX_STRFTIME_OUTPUT 1024

#define ZLIB_CHUNK_SIZE 512

#define INDEXED(obj) (RTEST(obj) && (REFERENCE_P(self) || !object_stores_bytes_p(state, obj)))

#define RISA(obj,cls) (REFERENCE_P(obj) && ISA(obj,BASIC_CLASS(cls)))

#define BIGNUM_P(obj) (RISA(obj, bignum))
#define FLOAT_P(obj) (RISA(obj, floatpoint))
#define COMPLEX_P(obj) (FALSE)

#define INTEGER_P(obj) (FIXNUM_P(obj) || BIGNUM_P(obj))
#define NUMERIC_P(obj) (FIXNUM_P(obj) || COMPLEX_P(obj) || BIGNUM_P(obj) || FLOAT_P(obj))

#define CLASS_P(obj) RISA(obj, class)
#define TUPLE_P(obj) RISA(obj, tuple)
#define IO_P(obj) RISA(obj, io)
#define STRING_P(obj) RISA(obj, string)
#define HASH_P(obj) RISA(obj, hash)
#define ARRAY_P(obj) RISA(obj, array)

// defines a required arity for a primitive
// return true because we want other handler code to ignore it
// this is because it is raised directly in the primitive as an exception
#define ARITY(required) if((required) != num_args) { _ret = TRUE; cpu_raise_arg_error(state, c, num_args, required); break; }
// for primitive protection
#define GUARD(predicate_expression) if( ! (predicate_expression) ) { _ret = FALSE; break; }
// popping with type checking -- a predicate function must be specified
// i.e. if type is STRING then STRING_P must be specified
#define POP(var, type) var = stack_pop(); GUARD( type##_P(var) )

int cpu_perform_system_primitive(STATE, cpu c, int prim, OBJECT mo, int num_args, OBJECT method_name, OBJECT mod) {
  int _ret = TRUE;
  OBJECT self, t1, t2, t3;
  int j, k, m, _orig_sp;
  OBJECT *_orig_sp_ptr;
  int fds[2];
  char *buf;
  
  _orig_sp_ptr = c->sp_ptr;
  _orig_sp = c->sp;
  #include "system_primitives.gen"
  
  if(!_ret) {
    c->sp_ptr = _orig_sp_ptr;
    c->sp = _orig_sp;
  }
  return _ret;
}


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <zlib.h>
#include <math.h>
#include <glob.h>
#include <termios.h>
#include <sys/time.h>
#include <ev.h>

#include "shotgun/lib/shotgun.h"
#include "shotgun/lib/cpu.h"
#include "shotgun/lib/module.h"
#include "shotgun/lib/methctx.h"
#include "shotgun/lib/class.h"
#include "shotgun/lib/string.h"
#include "shotgun/lib/hash.h"
#include "shotgun/lib/symbol.h"
#include "shotgun/lib/object.h"
#include "shotgun/lib/bytearray.h"
#include "shotgun/lib/tuple.h"
#include "shotgun/lib/regexp.h"
#include "shotgun/lib/archive.h"
#include "shotgun/lib/machine.h"
#include "shotgun/lib/grammar.h"
#include "shotgun/lib/subtend.h"
#include "shotgun/lib/subtend/nmc.h"
#include "shotgun/lib/fixnum.h"
#include "shotgun/lib/list.h"
#include "shotgun/lib/io.h"
#include "shotgun/lib/subtend/ffi.h"
#include "shotgun/lib/environment.h"

#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
# define HAVE_STRUCT_TM_TM_GMTOFF
# define HAVE_STRUCT_TM_TM_ZONE
#endif

extern char **environ;

OBJECT math_sqrt(STATE, OBJECT a);
int _object_stores_bytes(OBJECT self);

#define STATIC_SIZE 100

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

#define STRING_OR_NIL_P(obj) (STRING_P(obj) || NIL_P(obj))

// defines a required arity for a primitive
// return true because we want other handler code to ignore it
// this is because it is raised directly in the primitive as an exception
#define ARITY(required) if((required) != num_args) { _ret = TRUE; cpu_raise_arg_error(state, c, num_args, required); break; }
// for primitive protection
#define GUARD(predicate_expression) if( ! (predicate_expression) ) { _ret = FALSE; break; }
// popping with type checking -- a predicate function must be specified
// i.e. if type is STRING then STRING_P must be specified
#define POP(var, type) var = stack_pop(); GUARD( type##_P(var) )
// raise an exception of the specified type and msg
#define RAISE(exc_class, msg) ({ \
  OBJECT _cls = rbs_const_get(state, BASIC_CLASS(object), exc_class); \
  GUARD(CLASS_P(_cls)); \
  cpu_raise_exception(state, c, cpu_new_exception(state, c, _cls, msg)); \
  })
#define RAISE_FROM_ERRNO(msg) cpu_raise_from_errno(state, c, msg)

void ffi_call(STATE, cpu c, OBJECT ptr);

int cpu_perform_system_primitive(STATE, cpu c, int prim, OBJECT mo, int num_args, OBJECT method_name, OBJECT mod, OBJECT block) {
  int _ret = TRUE;
  OBJECT self, t1, t2, t3, t4;
  int j, k, m, _orig_sp;
  OBJECT *_orig_sp_ptr;
  int fds[2];
  char *buf;
  
  _orig_sp_ptr = c->sp_ptr;
  _orig_sp = c->sp;
  #include "shotgun/lib/system_primitives.gen"
  
  if(!_ret) {
    c->sp_ptr = _orig_sp_ptr;
    c->sp = _orig_sp;
  }
  return _ret;
}

int cpu_perform_runtime_primitive(STATE, cpu c, int prim, OBJECT mo, int num_args, OBJECT method_name, OBJECT mod, OBJECT block) {
  int _ret = TRUE;
  OBJECT self, t1, t2, t3;
  int j, _orig_sp;
  OBJECT *_orig_sp_ptr;
  
  _orig_sp_ptr = c->sp_ptr;
  _orig_sp = c->sp;
  #include "shotgun/lib/runtime_primitives.gen"
  
  if(!_ret) {
    c->sp_ptr = _orig_sp_ptr;
    c->sp = _orig_sp;
  }
  return _ret;
}



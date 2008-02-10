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

#define INDEXED(obj) (RTEST(obj) && (REFERENCE_P(obj) || !object_stores_bytes_p(state, obj)))

#define RTYPE(obj,type) (REFERENCE_P(obj) && obj->obj_type == type)
#define RISA(obj,cls) (REFERENCE_P(obj) && ISA(obj,BASIC_CLASS(cls)))

#define BIGNUM_P(obj) (RTYPE(obj, BignumType))
#define FLOAT_P(obj) (RTYPE(obj, FloatType))
#define COMPLEX_P(obj) (FALSE)

#define INTEGER_P(obj) (FIXNUM_P(obj) || BIGNUM_P(obj))
#define NUMERIC_P(obj) (FIXNUM_P(obj) || COMPLEX_P(obj) || BIGNUM_P(obj) || FLOAT_P(obj))

#define CLASS_P(obj) RTYPE(obj, ClassType)
#define TUPLE_P(obj) RTYPE(obj, TupleType)
#define IO_P(obj) RISA(obj, io)
#define STRING_P(obj) RTYPE(obj, StringType)
// #define STRING_P(obj) RISA(obj, string)
#define HASH_P(obj) (RISA(obj, hash))
#define ARRAY_P(obj) RTYPE(obj, ArrayType)

#define STRING_OR_NIL_P(obj) (STRING_P(obj) || NIL_P(obj))

#define CMETHOD_P(obj) RTYPE(obj, CMethodType)
#define REGEXP_P(obj) RTYPE(obj, RegexpType)

#define CTX_P(obj) RISA(obj, fastctx)
#define BYTEARRAY_P(obj) RTYPE(obj, ByteArrayType)
#define ISEQ_P(obj) RTYPE(obj, ISeqType)
#define TASK_P(obj) RTYPE(obj, TaskType)
#define CHANNEL_P(obj) RTYPE(obj, ChannelType)
#define BLOCKENV_P(obj) RTYPE(obj, BlockEnvType)
#define THREAD_P(obj) RTYPE(obj, ThreadType)

// defines a required arity for a primitive
// return true because we want other handler code to ignore it
// this is because it is raised directly in the primitive as an exception
#define ARITY(required) if((required) != msg->args) { cpu_raise_arg_error(state, c, msg->args, required); DONE(); }
// for primitive protection
#define GUARD(predicate_expression) if( ! (predicate_expression) ) { FAIL(); }
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

#define PRIM_OK 1
#define PRIM_FAIL 0

#define RET(val) ({ stack_push(val); return PRIM_OK; })
#define DONE() return PRIM_OK;
#define FAIL() return PRIM_FAIL;


void ffi_call(STATE, cpu c, OBJECT ptr);

#include "shotgun/lib/primitive_implementation.gen"

int cpu_perform_system_primitive(STATE, cpu c, int prim, struct message *msg) {
  int _ret = TRUE;
  int _orig_sp;
  OBJECT *_orig_sp_ptr;
  
  _orig_sp_ptr = c->sp_ptr;
  _orig_sp = c->sp;
  #include "shotgun/lib/system_primitives.gen"
  
  if(!_ret) {
    c->sp_ptr = _orig_sp_ptr;
    c->sp = _orig_sp;
  }
  return _ret;
}


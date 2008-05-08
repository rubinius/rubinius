#include "builtin.hpp"
#include "primitives.hpp"
#include "event.hpp"

#include <sys/time.h>
#include <time.h>
#include <fcntl.h>

namespace rubinius {
#define NEXT_ARG() msg.get_argument(cur_arg)

// defines a required arity for a primitive
// return true because we want other handler code to ignore it
// this is because it is raised directly in the primitive as an exception
#define ARITY(required) if((required) != msg.args) { throw new ArgumentError(msg.args, required); }
// for primitive protection
#define GUARD(predicate_expression) if( ! (predicate_expression) ) { FAIL(); }
// popping with type checking -- a predicate function must be specified
// i.e. if type is STRING then STRING_P must be specified
#define POP(var, type) var = NEXT_ARG(); GUARD( type##_P(var) )
#define ARG(var, type) type* var = try_as<type>(NEXT_ARG()); if(!var) FAIL();
// raise an exception of the specified type and msg
#define RAISE(exc_class, msg) ({ \
  OBJECT _cls = rbs_const_get(state, BASIC_CLASS(object), exc_class); \
  GUARD(CLASS_P(_cls)); \
  state->raise_exception(state->new_exception(_cls, msg)); \
  })
#define RAISE_FROM_ERRNO(msg) state->raise_from_errno(msg)

#define PRIM_OK true
#define PRIM_FAIL false

#define RET_VAL(val) state->return_value(val);

#define RET(val) ({ RET_VAL(val); return PRIM_OK; })
#define DONE() return PRIM_OK;
#define FAIL() return PRIM_FAIL;

#define INTEGER_P(obj) kind_of<Integer>(obj)
#define CLASS_P(obj) kind_of<Class>(obj)
#define IO_P(obj) kind_of<IO>(obj)
#define STRING_P(obj) kind_of<String>(obj)
#define BLOCKENV_P(obj) kind_of<BlockEnvironment>(obj)
#define ARRAY_P(obj) kind_of<Array>(obj)
#define HASH_P(obj) kind_of<Hash>(obj)
#define NUMERIC_P(obj) kind_of<Numeric>(obj)

#define SET_FIELD(obj, fel, val) state->om->store_object(obj, fel, val)
#define NTH_FIELD(obj, fel) obj->field[fel]

  extern char **environ;
#define STATIC_SIZE 100

#define MAX_STRFTIME_OUTPUT 1024

#define ZLIB_CHUNK_SIZE 512

}

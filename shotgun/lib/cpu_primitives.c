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
#include "grammer.h"

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

#define STATIC_SIZE 100

struct time_data {
  struct timeval tv;
  struct timezone tz;
};

#define MAX_SYSTEM_PRIM 2048

#define MAX_STRFTIME_OUTPUT 1024

#define ZLIB_CHUNK_SIZE 512

#define INDEXED(obj) (RTEST(obj) && (REFERENCE_P(self) || !object_stores_bytes_p(state, obj)))

#define RISA(obj,cls) (REFERENCE_P(obj) && ISA(obj,BASIC_CLASS(cls)))

#define IS_INTEGER(obj) (FIXNUM_P(obj) || RISA(obj, bignum))

int cpu_perform_system_primitive(STATE, cpu c, int prim, OBJECT mo, int num_args) {
  int _ret = TRUE;
  OBJECT self, t1, t2, t3;
  int j, k, m, _orig_sp;
  int fds[2];
  char *buf;
  
  _orig_sp = c->sp;
  #include "system_primitives.gen"
  
  if(!_ret) {
    c->sp = _orig_sp;
  }
  return _ret;
}

int cpu_perform_primitive(STATE, cpu c, int prim, OBJECT mo, int args) {
  if(prim < MAX_SYSTEM_PRIM) {
    return cpu_perform_system_primitive(state, c, prim, mo, args);
  } else {
    printf("TODO: implement custom primitives.\n");
    abort();
  }
}

#ifndef RBX_PRELUDE_H
#define RBX_PRELUDE_H

#include <stdint.h>
#include <vector>

namespace rubinius {
  typedef intptr_t native_int;

  /* Forward declaration so it can be used in prototypes */
  class VM;

  class Object;
  typedef Object* OBJECT;
  typedef std::vector<OBJECT> ObjectArray;

  class Symbol;
  typedef Symbol* SYMBOL;

  class Fixnum;
  typedef Fixnum* FIXNUM;

  class Integer;
  typedef Integer* INTEGER;

#define FALSE false
#define TRUE true
#define Qfalse ((OBJECT)6L)
#define Qnil   ((OBJECT)14L)
#define Qtrue  ((OBJECT)10L)
#define Qundef ((OBJECT)18L)
#define STATE rubinius::VM *state
#define G(whatever) state->globals.whatever.get()
#define GO(whatever) state->globals.whatever
};

#endif

#ifndef RBX_PRELUDE_H
#define RBX_PRELUDE_H

#define FALSE false
#define TRUE true

#include <stdint.h>
#include <vector>

#define STATE rubinius::VM *state
#define G(whatever) state->globals.whatever.get()
#define GO(whatever) state->globals.whatever

namespace rubinius {
  typedef intptr_t native_int;

  /* Forward declaration so it can be used in prototypes */
  class VM;

  class Object;
  typedef Object* OBJECT;
  typedef std::vector<OBJECT> ObjectArray;

  class ObjectMemory;
  class TypeInfo;
  class Class;
  class Module;
  class SymbolTable;
  class LookupTable;
  class Tuple;
  class Task;
  class Thread;

  class Symbol;
  typedef Symbol* SYMBOL;

  class Fixnum;
  typedef Fixnum* FIXNUM;

  class Integer;
  typedef Integer* INTEGER;

#define Qfalse ((OBJECT)6L)
#define Qnil   ((OBJECT)14L)
#define Qtrue  ((OBJECT)10L)
#define Qundef ((OBJECT)18L)

};

#include "virtual.hpp"

#endif

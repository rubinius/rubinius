#ifndef RBX_SUBTEND_RUBY_H
#define RBX_SUBTEND_RUBY_H

#include <cstddef>
#include <stdint.h>

#define RUBY

/**
 *  In MRI, VALUE represents an object.
 *  In rbx, this is a Handle pointer.
 */
#define VALUE size_t

/**
 *  In MRI, ID represents an interned string, i.e. a Symbol.
 *  In rbx, this is a Symbol pointer
 */
#define ID    uintptr_t

/* "Stash" the real versions. */
#define RBX_Qfalse  ((Object*)6L)
#define RBX_Qtrue   ((Object*)10L)
#define RBX_Qnil    ((Object*)14L)
#define RBX_Qundef  ((Object*)18L)

/* Reset relative to our VALUEs */
#undef Qfalse
#undef Qtrue
#undef Qnil
#undef Qundef

#define Qfalse ((VALUE)6L)
#define Qtrue  ((VALUE)10L)
#define Qnil   ((VALUE)14L)
#define Qundef ((VALUE)18L)

typedef void (*SubtendGenericFunction)(void);


extern "C" {

  /** Call method on receiver, args as varargs. */
  VALUE rb_funcall(VALUE receiver, ID method_name, int arg_count, ...);

  /** Call the method with args provided in a C array. */
  VALUE rb_funcall2(VALUE receiver, ID method_name, int arg_count, VALUE* args);

  /**
   *  Backend to actually define a method. Macros add the filename.
   *
   *  Kind:
   *    0:  Public
   *    1:  Private
   *    2:  Protected
   *    3:  Singleton
   */
  void rb_define_method_impl(const char* file,
                             VALUE module_handle,
                             const char* name,
                             SubtendGenericFunction fptr,
                             int arity,
                             int kind);

  /** Generate a NativeMethod to represent a method defined as a C function. Records file. */
  #define rb_define_method(mod, name, fptr, arity) \
          rb_define_method_impl(__FILE__, mod, name, (SubtendGenericFunction)fptr, arity, 0)
}

#endif


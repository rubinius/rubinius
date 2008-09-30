#ifndef RBX_EXCEPTION_HPP
#define RBX_EXCEPTION_HPP

#include <string>
#include <cstdlib>
#include <cstring>

#include "prelude.hpp"
#include "builtin/object.hpp"

namespace rubinius {

  void abort();
  void print_backtrace();

  /**
   * Base class for the various exception.
   *
   * Each exception class should define a +raise+ function. Code
   * should call the +raise+ function instead of throwing one of
   * the exceptions directly. This allows setting a breakpoint
   * on +raise+ to more easily debug exceptions. For example, to
   * cause a TypeError exception, use
   *
   *  TypeError::raise(type, obj, reason);
   *
   * instead of
   *
   *  throw TypeError(type, obj, reason);
   */
  class VMException {
  public:   /* Instance vars */

    char*   reason;

  public:   /* Ctors */

    VMException();
    VMException(const char* reason);
    ~VMException() { if(reason) free(reason); }

  public:   /* Interface */

    typedef std::vector<std::string> Backtrace;

    Backtrace* backtrace;
    void print_backtrace();
  };

  class Assertion : public VMException {
  public:   /* Ctors */

    /**
     *  Use Assertion::raise to throw an Assertion exception.
     */
    Assertion(const char* reason) : VMException(reason) { }

  public:   /* Interface */

    /**
     *  Throws an Assertion exception with explanation +mesg+.
     */
    static void raise(const char* mesg);
  };

  class TypeError : public VMException {
  public:   /* Instance vars */

    object_type type;
    OBJECT object;

  public:   /* Ctors */

    TypeError(object_type type, OBJECT obj, const char* reason = NULL)
      : VMException(reason), type(type), object(obj) { }
    TypeError(const char* reason)
      : VMException(reason), type((object_type)0), object(Qnil) { }

  public:   /* Interface */

    static void raise(object_type type, OBJECT obj, const char* reason = NULL);
    static void raise(const char* reason);
  };

  class ArgumentError : public VMException {
  public:   /* Instance vars */

    size_t expected;
    size_t given;
    char *reason;

  public:   /* Ctors */

    ArgumentError(size_t e, size_t g)
      : VMException(), expected(e), given(g) { }
    ArgumentError(const char* reason)
      : VMException(reason), expected(0), given(0) { }

  public:   /* Interface */

    static void raise(size_t expected, size_t given);
    static void raise(const char* reason);
  };

  class ObjectBoundsExceeded : public VMException {
  public:   /* Instance vars */

    OBJECT obj;
    size_t index;

  public:   /* Ctors */

    ObjectBoundsExceeded(OBJECT o, size_t i) : obj(obj), index(index) { }

  public:   /* Interface */

    static void raise(OBJECT obj, size_t index);
  };

  class ZeroDivisionError : public VMException {
  public:   /* Instance vars */

    INTEGER i;

  public:   /* Ctors */

    ZeroDivisionError(INTEGER i, const char* reason = NULL)
      : VMException(reason), i(i) { }

  public:   /* Interface */
    // TODO: add ::raise
  };

  class FloatDomainError : public VMException {
  public:   /* Instance vars */

    double d;

  public:   /* Ctors */

    FloatDomainError(double d, const char* reason = NULL)
      : VMException(reason), d(d) { }

  public:   /* Interface */
    // TODO: add ::raise
  };
};

#endif

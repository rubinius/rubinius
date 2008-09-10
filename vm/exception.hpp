#ifndef RBX_EXCEPTION_HPP
#define RBX_EXCEPTION_HPP

#include <string>
#include <cstdlib>
#include <cstring>

#include "prelude.hpp"
#include "builtin/object.hpp"

/* Each exception class should define a +raise+ function. Code
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
namespace rubinius {

  void abort();
  void print_backtrace();

  class VMException {
  public:
    typedef std::vector<std::string> Backtrace;

    Backtrace* backtrace;
    VMException();
    void print_backtrace();
  };

  class Assertion : public VMException
  {
    public:   /* Ctors */

    /**
     *  Constructor hidden behind Assertion::raise() usually.
     *
     *  The reason should live fine through the raise.
     */
    Assertion(const char* reason) : reason(reason) {}
    ~Assertion() {}

    public:   /* Interface */

    /**
     *  Main interface for raising Assertion errors.
     */
    static void raise(const char* mesg);


    public:   /* Instance vars */

    const char*   reason;
  };

  class TypeError : public VMException {
  public:
    object_type type;
    OBJECT object;
    const char* reason;

    static void raise(object_type type, OBJECT obj, const char* reason = NULL);

    TypeError(object_type type, OBJECT obj, const char* reason = NULL)
      : type(type), object(obj), reason(reason) { };
  };

  class ArgumentError : public VMException {
  public:
    size_t expected;
    size_t given;
    char *reason;

    static void raise(size_t expected, size_t given);
    static void raise(const char* reason);

    ArgumentError(size_t e, size_t g) : expected(e), given(g), reason(NULL) { }
    ArgumentError(const char* reason) : expected(0), given(0), reason(NULL) { this->reason = strdup(reason); }

    ~ArgumentError() {
      if(reason) free(reason);
    }
  };

  class ObjectBoundsExceeded : public VMException {
  public:
    OBJECT obj;
    size_t index;

    static void raise(OBJECT o, size_t i);

    ObjectBoundsExceeded(OBJECT o, size_t i) : obj(o), index(i) { }
  };

  class ZeroDivisionError : public VMException {
  public:
    INTEGER i;
    const char* reason;

    ZeroDivisionError(INTEGER i, const char* reason = NULL)
      : i(i), reason(reason) { };
  };

  class FloatDomainError : public VMException {
  public:
    double d;
    const char* reason;

    FloatDomainError(double d, const char* reason = NULL)
      : d(d), reason(reason) { };
  };
};

#endif

#ifndef RBX_EXCEPTION_HPP
#define RBX_EXCEPTION_HPP

#include "prelude.hpp"
#include "builtin/object.hpp"

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

  class Assertion : public VMException {
  public:
    char *reason;

    // Used instead of throwing an Assertion directly so that
    // you can set a breakpoint on raise.
    static void raise(const char* mesg);

    Assertion(const char* reason) {
      this->reason = strdup(reason);
    }

    ~Assertion() {
      free(reason);
    }
  };

  class TypeError : public VMException {
  public:
    object_type type;
    OBJECT object;
    const char* reason;

    // Used instead of throw'ning a TypeError directly,
    // so that breakpoints can be set on this function and easily be
    // debugged.
    static void raise(object_type type, OBJECT obj, const char* reason = NULL);

    TypeError(object_type type, OBJECT obj, const char* reason = NULL)
      : type(type), object(obj), reason(reason) { };
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

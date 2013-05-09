#ifndef RBX_EXCEPTION_HPP
#define RBX_EXCEPTION_HPP

#include <string>
#include <stdlib.h>
#include <string.h>

#include "object_types.hpp"
#include "prelude.hpp"
#include "builtin/object.hpp"

#include "bug.hpp"

namespace rubinius {
  class Exception;

  /**
   * Base class for the various exception.
   *
   * Each exception class should define a +raise+ function. Code
   * should call the +raise+ function instead of throwing one of
   * the exceptions directly. This allows setting a breakpoint
   * on +raise+ to more easily debug exceptions. For example, to
   * cause a Assertion exception, use
   *
   *  Assertion::raise(type, obj, reason);
   *
   * instead of
   *
   *  throw Assertion(type, obj, reason);
   */
  class VMException {
  public:   /* Instance vars */

    typedef std::vector<std::string> Backtrace;

    Backtrace* backtrace;
    char*   reason;

  public:   /* Ctors */

    VMException(bool make_backtrace = true);
    VMException(const char* reason, bool make_backtrace = true);
    ~VMException() {
      if(backtrace) delete backtrace;
      if(reason) free(reason);
    }
    VMException(const VMException&);

  public:   /* Interface */

    void print_backtrace();
  };

  /**
   * An assertion in the VM failed for the given +reason+.
   * Only use this exception if there is no possible hope
   * of handling the condition in Ruby-land. Otherwise, use:
   *
   *   Exception::assertion_error(state, reason);
   */
  class Assertion : public VMException {
  public:   /* Ctors */

    /**
     *  Use Assertion::raise to throw an Assertion exception.
     */
    Assertion(const char* reason) : VMException(reason) { }

  public:   /* Interface */

    /**
     *  Throws an Assertion exception with explanation +reason+.
     */
    static void raise(const char* reason);
  };

  class TypeError : public VMException {
  public:   /* Instance vars */

    object_type type;
    Object* object;

  public:   /* Ctors */

    TypeError(object_type type, Object* obj, const char* reason = NULL)
      : VMException(reason), type(type), object(obj) { }

  public:   /* Interface */

    static void raise(object_type type, Object* obj, const char* reason = NULL);
  };

  /**
   * Any exceptional VM condition that can be handled in Ruby-land
   * should be propagated using a RubyException. However, instead
   * of using this exception directly, use the appropriate static
   * method on the builtin class Exception.
   */
  class RubyException : public VMException {
  public:   /* Instance vars */

    Exception* exception;

  public:   /* Ctors */

    RubyException(Exception* exception, bool make_backtrace);

  public:   /* Interface */

    static void raise(Exception* exception, bool make_backtrace = false);

    /**
     * Prints out the exception message and the Ruby backtrace.
     * Also prints the VM backtrace if it was generated.
     */
    void show(STATE);
  };
};

#endif

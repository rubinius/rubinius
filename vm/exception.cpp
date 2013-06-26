#include "builtin/exception.hpp"

#include "vm.hpp"
#include "prelude.hpp"
#include "exception.hpp"
#include "detection.hpp"

#include "builtin/string.hpp"
#include "builtin/class.hpp"
#include "builtin/symbol.hpp"

#include <ctype.h>
#include <vector>
#include <iostream>
#include <cxxabi.h>
#include <assert.h>

#ifdef USE_EXECINFO
#include <execinfo.h>
#endif

namespace rubinius {

  void TypeError::raise(object_type type, Object* obj, const char* reason) {
    throw TypeError(type, obj, reason);
    // Not reached.
  }

  void Assertion::raise(const char* reason) {
    throw Assertion(reason);
    // Not reached.
  }

  RubyException::RubyException(Exception* exception, bool make_backtrace)
      : VMException(make_backtrace), exception(exception) {
  }

  void RubyException::raise(Exception* exception, bool make_backtrace) {
    throw RubyException(exception, make_backtrace);
    // Not reached.
  }

  void RubyException::show(STATE) {
    std::cout << exception->message_c_str(state) <<
        " (" <<
        exception->class_object(state)->debug_str(state) <<
        ") \n";
    print_backtrace();
  }

  static VMException::Backtrace get_trace(size_t skip, size_t max_depth=100) {
#ifdef USE_EXECINFO
    size_t stack_depth;
    char **stack_strings;

    void **stack_addrs = (void **) calloc(max_depth, sizeof(void*));

    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);

    VMException::Backtrace s;

    for (size_t i = skip; i < stack_depth; i++) {
      s.push_back(std::string(stack_strings[i]));
    }
    free(stack_strings); // malloc()ed by backtrace_symbols
    free(stack_addrs);
#else
    VMException::Backtrace s;
    s.push_back(std::string("C++ backtrace not available"));
#endif

    return s;
  }

  static void squeeze_space(std::string& str) {
    std::string ws = "    ";

    std::string::size_type pos = str.find(ws);
    if(pos == std::string::npos) return;

    std::string::size_type i = pos + 4;
    while(std::isspace(str[i])) i++;
    str.erase(pos + 2, i - pos - 2);
  }

  static void demangle(VMException::Backtrace& s) {
    for (size_t i = 0; i < s.size(); i++) {
      squeeze_space(s[i]);
      const char* str = s[i].c_str();
      const char* pos = strstr(str, " _Z");
      /* Found a mangle. */
      if(pos) {
        size_t sz = 0;
        char *cpp_name = 0;
        char* name = strdup(pos + 1);
        char* end = strstr(name, " + ");
        *end = 0;

        int status;
        cpp_name = abi::__cxa_demangle(name, cpp_name, &sz, &status);

        // It's possible for __cxa_demangle to return 0x0, which probably
        // shouldn't happen with status == 0, but it was observed in OS X
        // Mavericks Preview 2 so be paranoid.
        if(cpp_name) {
          if(!status) {
            std::string full_cpp = std::string(str, pos - str) + " " + cpp_name +
              " " + (++end);
            s[i] = full_cpp;
          }
          free(cpp_name);
        }
        free(name);
      }
    }
  }

  void abort() {
    std::cout << "Abort!" << std::endl;
    print_backtrace();
    ::abort();
  }

  void bug(const char* message) {
    std::cerr << "[BUG: " << message << "]\n";
    print_backtrace();
    ::abort();
  }

  void bug(const char* message, const char* arg) {
    std::cerr << "[BUG: " << message
              << ": " << arg
              << "]\n";
    print_backtrace();
    ::abort();
  }

  void warn(const char* message) {
    std::cerr << "[WARNING: " << message << "]\n";
    print_backtrace();
  }

  void print_backtrace(size_t max) {
    VMException::Backtrace s = get_trace(2, max);
    demangle(s);

    for(size_t i = 0; i < s.size(); i++) {
      std::cout << s[i] << std::endl;
    }
  }

  static VMException::Backtrace get_cpp_backtrace() {
    VMException::Backtrace s = get_trace(2);
    demangle(s);
    return s;
  }

  VMException::VMException(bool make_backtrace)
      : backtrace(NULL), reason(NULL) {
    if(make_backtrace) {
      backtrace = new VMException::Backtrace(get_cpp_backtrace());
    }
  }

  VMException::VMException(const char* reason, bool make_backtrace)
      : backtrace(NULL), reason(NULL) {
    if(make_backtrace) {
      backtrace = new VMException::Backtrace(get_cpp_backtrace());
    }
    if(reason) this->reason = strdup(reason);
  }

  VMException::VMException(const VMException& other)
      : backtrace(NULL), reason(NULL) {
    if(other.backtrace) {
      backtrace = new VMException::Backtrace(*other.backtrace);
    }
    if(other.reason) {
      reason = strdup(reason);
    }
  }

  void VMException::print_backtrace() {
    if(!backtrace) return;

    for(size_t i = 0; i < backtrace->size(); i++) {
      std::cout << backtrace->at(i) << std::endl;
    }
  }
}

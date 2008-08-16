#include "prelude.hpp"
#include "object.hpp"
#include <vector>
#include <iostream>
#include <execinfo.h>
#include <cxxabi.h>

namespace rubinius {

  static VMException::Backtrace get_trace(size_t skip) {

    const size_t max_depth = 100;
    size_t stack_depth;
    void *stack_addrs[max_depth];
    char **stack_strings;

    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);

    VMException::Backtrace s;

    for (size_t i = skip; i < stack_depth; i++) {
      s.push_back(std::string(stack_strings[i]));
    }
    free(stack_strings); // malloc()ed by backtrace_symbols

    return s;
  }


  static void demangle(VMException::Backtrace& s) {
    for (size_t i = 0; i < s.size(); i++) {
      const char* str = s[i].c_str();
      const char* pos = strstr(str, " _Z");
      /* Found a mangle. */
      if(pos) {
        size_t sz = 1024;
        char *cpp_name = 0;
        char* name = strdup(pos + 1);
        char* end = strstr(name, " + ");
        *end = 0;

        int status;
        cpp_name = abi::__cxa_demangle(name, cpp_name, &sz, &status);

        if(!status) {
          std::string full_cpp = std::string(str, pos - str) + " " + cpp_name +
            " " + (++end);
          s[i] = full_cpp;
        }
        if(cpp_name) free(cpp_name);
        free(name);
      }
    }
  }

  void abort() {
    std::cout << "Abort!" << std::endl;
    print_backtrace();
    assert(0);
  }

  void print_backtrace() {
    VMException::Backtrace s = get_trace(2);
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

  VMException::VMException() {
    backtrace = new VMException::Backtrace(get_cpp_backtrace());
  }

  void VMException::print_backtrace() {
    if(!backtrace) return;

    for(size_t i = 0; i < backtrace->size(); i++) {
      std::cout << backtrace->at(i) << std::endl;
    }
  }
}

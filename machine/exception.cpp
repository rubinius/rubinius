#include "thread_state.hpp"
#include "exception.hpp"
#include "detection.hpp"
#include "machine.hpp"
#include "memory.hpp"

#include "class/exception.hpp"
#include "class/string.hpp"
#include "class/class.hpp"
#include "class/symbol.hpp"
#include "class/unwind_state.hpp"

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

  static MachineException::Backtrace get_trace(size_t skip, size_t max_depth=100) {
#ifdef USE_EXECINFO
    size_t stack_depth;
    char **stack_strings;

    void **stack_addrs = (void **) calloc(max_depth, sizeof(void*));

    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);

    MachineException::Backtrace s;

    for (size_t i = skip; i < stack_depth; i++) {
      s.push_back(std::string(stack_strings[i]));
    }
    free(stack_strings); // malloc()ed by backtrace_symbols
    free(stack_addrs);
#else
    MachineException::Backtrace s;
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

  static void demangle(MachineException::Backtrace& s) {
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
        // shouldn't happen with status == 0, but it was observed in macOS
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

  void bug() {
    logger::fatal("The Rubinius process is aborting");
    print_backtrace();
    ::abort();
  }

  void bug(const char* message) {
    logger::fatal("[BUG: %s]", message);
    print_backtrace();
    ::abort();
  }

  void bug(const char* message, const char* arg) {
    logger::fatal("[BUG: %s: %s]", message, arg);
    print_backtrace();
    ::abort();
  }

  void warn(const char* message) {
    logger::warn("[WARNING: %s]", message);
    std::cerr << "[WARNING: " << message << "]\n";
  }

  void print_backtrace(size_t max) {
    MachineException::Backtrace s = get_trace(2, max);
    demangle(s);

    for(size_t i = 0; i < s.size(); i++) {
      logger::warn("%s", s[i].c_str());
      std::cout << s[i] << std::endl;
    }
  }

  static MachineException::Backtrace get_cpp_backtrace() {
    MachineException::Backtrace s = get_trace(2);
    demangle(s);
    return s;
  }

  MachineException::MachineException(bool make_backtrace)
      : backtrace(NULL), reason(NULL) {
    if(make_backtrace) {
      backtrace = new MachineException::Backtrace(get_cpp_backtrace());
    }
  }

  MachineException::MachineException(const char* reason, bool make_backtrace)
      : backtrace(NULL), reason(NULL) {
    if(make_backtrace) {
      backtrace = new MachineException::Backtrace(get_cpp_backtrace());
    }
    if(reason) this->reason = strdup(reason);
  }

  MachineException::MachineException(const MachineException& other)
      : backtrace(NULL), reason(NULL) {
    if(other.backtrace) {
      backtrace = new MachineException::Backtrace(*other.backtrace);
    }
    if(other.reason) {
      reason = strdup(other.reason);
    }
  }

  void MachineException::print_backtrace() {
    if(!backtrace) return;

    for(size_t i = 0; i < backtrace->size(); i++) {
      logger::warn("%s", backtrace->at(i).c_str());
      std::cout << backtrace->at(i) << std::endl;
    }
  }

  void MachineException::guard(STATE, bool exit, std::function<void ()> process) {
    try {
      process();
    } catch(Assertion *e) {
      std::cout << "Machine Assertion:" << std::endl;
      std::cout << "  " << e->reason << std::endl << std::endl;
      e->print_backtrace();
      delete e;
    } catch(RubyException &e) {
      std::cout << "Ruby Exception hit toplevel:\n";
      // Prints Ruby backtrace, and Machine backtrace if captured
      e.show(state);
    } catch(TypeError &e) {
      /* TypeError's here are dealt with specially so that they can deliver
       * more information, such as _why_ there was a type error issue.
       *
       * This has the same name as the RubyException TypeError (run `5 + "string"`
       * as an example), but these are NOT the same - this exception is raised
       * internally when cNil gets passed to an array method, for instance, when
       * an array was expected.
       */
      std::cout << "Type Error detected:" << std::endl;
      TypeInfo* wanted = state->memory()->find_type(e.type);

      if(!e.object->reference_p()) {
        std::cout << "  Tried to use non-reference value " << e.object;
      } else {
        TypeInfo* was = state->memory()->find_type(e.object->type_id());
        std::cout << "  Tried to use object of type " <<
          was->type_name << " (" << was->type << ")";
      }

      std::cout << " as type " << wanted->type_name << " (" <<
        wanted->type << ")" << std::endl;

      e.print_backtrace();
    } catch(MachineException &e) {
      std::cout << "Unknown Machine exception detected:" << std::endl;
      e.print_backtrace();
    } catch(std::exception& e) {
      std::cout << "C++ exception detected: " << e.what() << std::endl;
    } catch(...) {
      std::cout << "Unknown C++ exception detected at top level" << std::endl;
    }

    if(exit) {
      state->machine()->halt(state, state->unwind_state()->raise_value());
    }
  }
}

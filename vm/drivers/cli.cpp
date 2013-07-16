#include <iostream>
#include <fstream>

#include <sys/stat.h>

#ifdef ENABLE_LLVM
#include <llvm/Support/ManagedStatic.h>
#endif

#include "environment.hpp"
#include "oop.hpp"
#include "type_info.hpp"
#include "exception.hpp"

#include "config.h"
#include "paths.h"


using namespace std;
using namespace rubinius;

/**
 * Main rbx entry point.
 *
 * The main function here handles the environment settings and command-line
 * arguments passed to it. It then boots the VM, runs the appropriate file
 * (`loader`), and returns 0 if no errors occur along the way.
 *
 * If there is an Assertion raised or an Exception, it prints the backtrace
 * supplied. This function is the wrapper for the entire VM, as it deals with
 * anything that could possibly happen to the VM. It's like the person
 * playing whack-a-mole, in that if something tries to come out of the VM
 * that's evil (such as a failed assertion or exception), it catches it and
 * skins it and shows it to the user.
 */
int main(int argc, char** argv) {
  int exit_code = 0;

  // Ensure to destruct an Environment before calling llvm_shutdown(), which
  // follows this block. Because Environment's destructor uses LLVM API, it
  // must precede llvm_shutdown().
  {
    Environment env(argc, argv);
    env.setup_cpp_terminate();

    try {
      if(const char* var = getenv("RBX_OPTIONS")) {
        env.load_string(var);
      }

      if(const char* path = getenv("RBX_OPTFILE")) {
        env.load_conf(path);
      }

      env.run_from_filesystem();
    } catch(Assertion *e) {
      std::cout << "VM Assertion:" << std::endl;
      std::cout << "  " << e->reason << std::endl << std::endl;
      e->print_backtrace();

      std::cout << std::endl << "Ruby backtrace:" << std::endl;
      env.state->vm()->print_backtrace();
      delete e;
      exit_code = 1;
    } catch(RubyException &e) {
      std::cout << "Ruby Exception hit toplevel:\n";
      // Prints Ruby backtrace, and VM backtrace if captured
      e.show(env.state);
      exit_code = 1;
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
      TypeInfo* wanted = env.state->vm()->find_type(e.type);

      if(!e.object->reference_p()) {
        std::cout << "  Tried to use non-reference value " << e.object;
      } else {
        TypeInfo* was = env.state->vm()->find_type(e.object->type_id());
        std::cout << "  Tried to use object of type " <<
          was->type_name << " (" << was->type << ")";
      }

      std::cout << " as type " << wanted->type_name << " (" <<
        wanted->type << ")" << std::endl;

      e.print_backtrace();

      std::cout << "Ruby backtrace:" << std::endl;
      env.state->vm()->print_backtrace();
      exit_code = 1;
    } catch(MissingRuntime& e) {
      std::cerr << std::endl;
      std::cerr << e.what() << std::endl;
      std::cerr << "Rubinius was configured to find the directories relative to:" << std::endl;
      std::cerr << std::endl << "  " << RBX_PREFIX_PATH << std::endl << std::endl;
      std::cerr << "Set the environment variable RBX_PREFIX_PATH to the directory";
      std::cerr << std::endl;
      std::cerr << "that is the prefix of the following runtime directories:" << std::endl;
      std::cerr << std::endl;
      std::cerr << "      BIN_PATH: " << RBX_BIN_PATH << std::endl;
      std::cerr << "  RUNTIME_PATH: " << RBX_RUNTIME_PATH << std::endl;
      std::cerr << "   KERNEL_PATH: " << RBX_KERNEL_PATH << std::endl;
      std::cerr << "      LIB_PATH: " << RBX_LIB_PATH << std::endl;
      std::cerr << "     SITE_PATH: " << RBX_SITE_PATH << std::endl;
      std::cerr << "   VENDOR_PATH: " << RBX_VENDOR_PATH << std::endl;
      std::cerr << "     GEMS_PATH: " << RBX_GEMS_PATH << std::endl;
      std::cerr << std::endl;
      exit_code = 1;
    } catch(BadKernelFile& e) {
      std::cout << "ERROR: Unable to load: " << e.what() << std::endl << std::endl;
      std::cout << "Please run the following commands to rebuild:" << std::endl;
      std::cout << "  rake clean" << std::endl;
      std::cout << "  rake or rake install" << std::endl << std::endl;
      std::cout << "If the problem persists, please open an issue at:" << std::endl;
      std::cout << "  http://github.com/rubinius/rubinius\n";
      exit_code = 1;
    } catch(VMException &e) {
      std::cout << "Unknown VM exception detected." << std::endl;
      e.print_backtrace();
      exit_code = 1;
    } catch(std::runtime_error& e) {
      std::cout << "Runtime exception: " << e.what() << std::endl;
      exit_code = 1;
    }

    if(!exit_code) {
      env.halt(env.state);
      exit_code = env.exit_code(env.state);
    }
  }

#ifdef ENABLE_LLVM
  llvm::llvm_shutdown();
#endif

  return exit_code;
}

#include <iostream>
#include <fstream>

#include <sys/stat.h>

#include "vm/environment.hpp"
#include "vm/oop.hpp"
#include "vm/type_info.hpp"
#include "vm/exception.hpp"

#include "vm/config.h"


using namespace std;
using namespace rubinius;

static void check_directory(std::string root);

/* The main function here handles the CL arguments passed to it.
 * It then boots the VM, runs the appropriate file (`loader`),
 * and returns 0. If there is an Assertion raised or an Exception,
 * it prints the backtrace supplied. This function is the wrapper for
 * the entire VM, as it deals with anything that could possibly happen
 * to the VM. It's like the person playing whack-a-mole, in that if
 * something tries to come out of the VM that's evil (such as a failed
 * assertion or exception), it catches it and skins it and shows it to
 * the user.
 *
 * Note: Although Rubinius is gathering support for multiple VMs, this
 *       function does not deal with that subject.
 */
int main(int argc, char** argv) {
  Environment env(argc, argv);
  env.setup_cpp_terminate();

  try {
    if(const char* var = getenv("RBX_OPTIONS")) {
      env.load_string(var);
    }

    if(const char* path = getenv("RBX_OPTFILE")) {
      env.load_conf(path);
    }

    const char* runtime = getenv("RBX_RUNTIME");

    if(!runtime) runtime = RBX_RUNTIME;

    check_directory(runtime);
    env.run_from_filesystem(runtime);

  } catch(Assertion *e) {
    std::cout << "VM Assertion:" << std::endl;
    std::cout << "  " << e->reason << std::endl << std::endl;
    e->print_backtrace();

    std::cout << std::endl << "Ruby backtrace:" << std::endl;
    env.state->print_backtrace();
    delete e;
  } catch(RubyException &e) {
    std::cout << "Ruby Exception hit toplevel:\n";
    // Prints Ruby backtrace, and VM backtrace if captured
    e.show(env.state);
  } catch(TypeError &e) {

    /* TypeError's here are dealt with specially so that they can deliver
     * more information, such as _why_ there was a type error issue.
     *
     * This has the same name as the RubyException TypeError (run `5 + "string"`
     * as an example), but these are NOT the same - this exception is raised
     * internally when Qnil gets passed to an array method, for instance, when
     * an array was expected.
     */
    std::cout << "Type Error detected:" << std::endl;
    TypeInfo* wanted = env.state->find_type(e.type);

    if(!e.object->reference_p()) {
      std::cout << "  Tried to use non-reference value " << e.object;
    } else {
      TypeInfo* was = env.state->find_type(e.object->type_id());
      std::cout << "  Tried to use object of type " <<
        was->type_name << " (" << was->type << ")";
    }

    std::cout << " as type " << wanted->type_name << " (" <<
      wanted->type << ")" << std::endl;

    e.print_backtrace();

    std::cout << "Ruby backtrace:" << std::endl;
    env.state->print_backtrace();
  } catch(BadKernelFile& e) {
    std::cout << "ERROR: BadKernelFile: " << e.what() << "\n\n";
    std::cout << "An invalid kernel file has been detected.\n";
    std::cout << "This is because the VM is out of sync with the kernel.\n";
    std::cout << "Please recompile your kernel using:\n";
    std::cout << "  rake kernel:clean clean\n";
    std::cout << "  rake\n";
    std::cout << "\nIf the problem persists, please open an issue at:\n";
    std::cout << "  http://github.com/rubinius/rubinius\n";
    std::cout << "\nThanks,\n  Management.\n";
    return 1;
  } catch(VMException &e) {
    std::cout << "Unknown VM exception detected." << std::endl;
    e.print_backtrace();
  } catch(std::runtime_error& e) {
    std::cout << "Runtime exception: " << e.what() << std::endl;
  } catch(...) {
    std::cout << "Unknown exception detected." << std::endl;
  }

  env.halt();
  return env.exit_code();
}

static void check_directory(std::string runtime) {
  struct stat st;

  if(stat(runtime.c_str(), &st) == -1 || !S_ISDIR(st.st_mode)) {

    std::cerr << "ERROR: unable to find runtime directory" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Rubinius was configured to find the runtime directory at:" << std::endl;
    std::cerr << std::endl << "  " << runtime << std::endl;
    std::cerr << std::endl;
    std::cerr << "but that directory does not exist." << std::endl;
    std::cerr << std::endl;
    std::cerr << "Set the environment variable RBX_RUNTIME to the location" << std::endl;
    std::cerr << "of the directory with the compiled Rubinius kernel files." << std::endl;
    std::cerr << std::endl;
    std::cerr << "You may have configured Rubinius for a different install" << std::endl;
    std::cerr << "directory but you have not run \'rake install\' yet." << std::endl;
    std::cerr << std::endl;

    exit(1);
  }
}

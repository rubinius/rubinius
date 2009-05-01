#include <iostream>
#include <fstream>

#include <sys/stat.h>

#include "vm/environment.hpp"
#include "vm/oop.hpp"
#include "vm/type_info.hpp"
#include "vm/exception.hpp"


using namespace std;
using namespace rubinius;

/* Loads the runtime kernel files. They're stored in /kernel.
 * These files consist of classes needed to bootstrap the kernel
 * and just get things started in general.
 *
 * @param root [String] The file root for /kernel. This expects to find
 *                      alpha.rbc (will compile if not there).
 * @param env  [Environment&] The environment for Rubinius. It is the uber
 *                      manager for multiple VMs and process-Ruby interaction. 
 */
static void load_runtime_kernel(Environment& env, std::string root) {
  std::string dirs = root + "/index";
  std::ifstream stream(dirs.c_str());
  if(!stream) {
    std::cout << "It appears that " << root << "/index is missing.\n";
    exit(1);
  }

  // Load the ruby file to prepare for bootstrapping Ruby!
  // The bootstrapping for the VM is already done by the time we're here.
  env.run_file(root + "/alpha.rbc");

  while(!stream.eof()) {
    std::string line;

    stream >> line;
    stream.get(); // eat newline

    // skip empty lines
    if(line.size() == 0) continue;

    env.load_directory(root + "/" + line);
  }
}

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
  env.state->init_stack_size();
  env.state->set_stack_start(&env);

  try {
    const char* runtime = getenv("RBX_RUNTIME");

    if(!runtime) {
      struct stat st;

      runtime = RBA_PATH;
      if(stat(runtime, &st) == -1 || !S_ISDIR(st.st_mode)) {
        // Use throw rather than ::raise here because we're outside
        // the VM really.
        throw new Assertion("set RBX_RUNTIME to runtime (or equiv)");
      }
    }

    std::string root = std::string(runtime);

    env.load_platform_conf(root);
    env.boot_vm();
    env.load_argv(argc, argv);

    load_runtime_kernel(env, std::string(root));

    std::string loader = root + "/loader.rbc";

    env.enable_preemption();
    env.start_signals();
    env.run_file(loader);
    return env.exit_code();

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
  } catch(std::runtime_error& e) {
    std::cout << "Runtime exception: " << e.what() << std::endl;
  } catch(VMException &e) {
    std::cout << "Unknown VM exception detected." << std::endl;
    e.print_backtrace();
  } catch(std::string e) {
    std::cout << e << std::endl;
  } catch(...) {
    std::cout << "Unknown exception detected." << std::endl;
  }
}

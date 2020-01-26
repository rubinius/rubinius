#include <iostream>
#include <fstream>

#include <sys/stat.h>

#include "environment.hpp"
#include "type_info.hpp"
#include "exception.hpp"

#include "memory/header.hpp"

#include "config.h"
#include "paths.h"
#include "debug.h"


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
  {
    Environment env(argc, argv);
    env.setup_cpp_terminate();

    MachineException::guard(env.state, true, [&]{
        if(const char* var = getenv("RBX_OPTIONS")) {
          env.load_string(var);
        }

        if(const char* path = getenv("RBX_OPTFILE")) {
          env.load_conf(path);
        }

        env.boot();
      });
  }

  return 1;
}

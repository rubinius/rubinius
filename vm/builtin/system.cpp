#include <vector>
#include <cerrno>

#include <unistd.h>

#include "vm/object.hpp"
#include "vm/vm.hpp"

#include "builtin/array.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"

#include "builtin/system.hpp"


namespace rubinius {


/* Primitives */

  Fixnum* System::vm_fork(VM* state)
  {
    int result = ::fork();

    if ( -1 == result ) {
      Exception::errno_error(state, "fork() failed!");
    }

    /*  TODO: EVFLAG_FORKCHECK should remove need for any maintenance
     *        here, but is there any other re-initialisation needed?
     */

    return Fixnum::from(result);
  }

  /* TODO: Improve error messages */
  Object* System::vm_exec(VM* state, String* path, Array* args)
  {
    std::size_t argc = args->size();

    /* execvp() requires a NULL as last element */
    std::vector<char*> argv((argc + 1), NULL);

    for (std::size_t i = 0; i < argc; ++i) {
      argv[i] = as<String>(args->get(state, i))->c_str();
    }

    (void) ::execvp(path->c_str(), &argv[0]);

    /* execvp() returning means it failed. */
    Exception::errno_error(state, "execvp() failed!");

    return Qnil;
  }

}

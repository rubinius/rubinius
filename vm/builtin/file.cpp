#include "file.hpp"

#include "arguments.hpp"
#include "fixnum.hpp"
#include "string.hpp"

#include <sys/stat.h>

namespace rubinius {
  Object* File::lchmod19(STATE, Fixnum* mode, Arguments& args) {
#ifdef HAVE_LCHMOD
    for (uint32_t i = 0; i < args.total(); i++) {
      if(String* path = try_as<String>(args.get_argument(i))) {
        int errno = chmod(path->c_str(state), mode->to_int());
        if(errno != 0) {
          Exception::errno_error(state, "Unable to change link permissions",
                                 errno, path->c_str(state));
          return 0;
        }

      }
    }
    return cTrue;
#else
    return Primitives::failure();
#endif
  }

  Object* File::is_lchmod_available(STATE) {
#ifdef HAVE_LCHMOD
    return cTrue;
#else
    return cFalse;
#endif
  }

}

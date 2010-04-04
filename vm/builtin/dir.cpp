#include "builtin/dir.hpp"
#include "ffi.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/memorypointer.hpp"
#include "builtin/string.hpp"

#include "object_utils.hpp"

#include <sys/types.h>
#include <dirent.h>

namespace rubinius {
  void Dir::init(STATE) {
    GO(dir).set(state->new_class("Dir", G(object)));
    G(dir)->set_object_type(state, DirType);
  }

  Dir* Dir::create(STATE) {
    Dir* d = state->new_object<Dir>(G(dir));
    d->os_ = 0;

    state->om->needs_finalization(d, (FinalizerFunction)&Dir::finalize);

    return d;
  }

  Dir* Dir::allocate(STATE, Object* self) {
    Dir* dir = create(state);

    if(Class* cls = try_as<Class>(self)) {
      dir->klass(state, cls);
    }

    return dir;
  }

  void Dir::finalize(STATE, Dir* dir) {
    if(dir->os_) {
      closedir(dir->os_);
      dir->os_ = 0;
    }
  }

  void Dir::guard(STATE) {
    if(!os_) {
      Exception::io_error(state, "closed directory");
    }
  }

  Object* Dir::open(STATE, String* path) {
    if(os_) closedir(os_);

    os_ = opendir(path->c_str());

    if(!os_) {
      Exception::errno_error(state, "Unable to open directory", errno, path->c_str());
      return 0;
    }

    this->path(state, path);

    return Qtrue;
  }

  Object* Dir::close(STATE) {
    guard(state);

    if(os_) {
      closedir(os_);
      os_ = 0;
    }

    return Qnil;
  }

  Object* Dir::closed_p(STATE) {
    return os_ ? Qfalse : Qtrue;
  }

  Object* Dir::read(STATE) {
    guard(state);

    struct dirent *ent = readdir(os_);

    if(!ent) return Qnil;

    return String::create(state, ent->d_name);
  }

  Object* Dir::control(STATE, Fixnum* kind, Integer* pos) {
    guard(state);

    switch(kind->to_native()) {
    case 0:
      seekdir(os_, pos->to_native());
      return Qtrue;
    case 1:
      rewinddir(os_);
      return Qtrue;
    case 2:
      return Integer::from(state, telldir(os_));
    }
    return Qnil;
  }
}

#include "object_utils.hpp"
#include "object_memory.hpp"

#include "builtin/dir.hpp"
#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"

namespace rubinius {
  void Dir::bootstrap(STATE) {
    GO(dir).set(state->memory()->new_class<Class, Dir>(state, G(object), "Dir"));
  }

  Dir* Dir::create(STATE) {
    Dir* d = Dir::allocate(state, G(dir));
    d->os_ = 0;

    state->memory()->needs_finalization(d, (FinalizerFunction)&Dir::finalize,
        FinalizeObject::eUnmanaged);

    return d;
  }

  Dir* Dir::allocate(STATE, Object* self) {
    return state->memory()->new_object<Dir>(state, as<Class>(self));
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

  Object* Dir::open(STATE, String* path, Object* enc) {
    if(os_) closedir(os_);

    os_ = opendir(path->c_str_null_safe(state));

    if(!os_) {
      Exception::errno_error(state, "Unable to open directory", errno, path->c_str_null_safe(state));
      return 0;
    }

    this->path(state, path);

    Encoding* encoding = 0;
    if(enc->nil_p()) {
      encoding = Encoding::filesystem_encoding(state);
    } else {
      encoding = as<Encoding>(enc);
    }
    this->encoding(state, encoding);

    return cTrue;
  }

  Object* Dir::close(STATE) {
    guard(state);

    if(os_) {
      closedir(os_);
      os_ = 0;
    }

    return cNil;
  }

  Object* Dir::closed_p(STATE) {
    return RBOOL(!os_);
  }

  Object* Dir::read(STATE) {
    guard(state);

    struct dirent ent;
    struct dirent* entp = &ent;
    if(int erno = readdir_r(os_, entp, &entp)) {
      Exception::errno_error(state, "readdir_r(3) failed", erno);
    }

    if(!entp) return cNil;

    String* str = String::create(state, ent.d_name);
    str->encoding(state, encoding());
    return str;
  }

  Object* Dir::control(STATE, Fixnum* kind, Integer* pos) {
    guard(state);

    switch(kind->to_native()) {
    case 0:
      seekdir(os_, pos->to_native());
      return cTrue;
    case 1:
      rewinddir(os_);
      return cTrue;
    case 2:
      return Integer::from(state, telldir(os_));
    }
    return cNil;
  }
}

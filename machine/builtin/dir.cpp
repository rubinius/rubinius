#include "object_utils.hpp"
#include "memory.hpp"

#include "builtin/dir.hpp"
#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"

namespace rubinius {
  void Dir::bootstrap(STATE) {
    GO(dir).set(state->memory()->new_class<Class, Dir>(state, "Dir"));
  }

  Dir* Dir::create(STATE) {
    Dir* d = Dir::allocate(state, G(dir));
    d->os(0);

    state->memory()->needs_finalization(state, d,
        (memory::FinalizerFunction)&Dir::finalize,
        memory::FinalizeObject::eUnmanaged);

    return d;
  }

  Dir* Dir::allocate(STATE, Object* self) {
    return state->memory()->new_object<Dir>(state, as<Class>(self));
  }

  void Dir::finalize(STATE, Dir* dir) {
    if(dir->os()) {
      closedir(dir->os());
      dir->os(0);
    }
  }

  void Dir::guard(STATE) {
    if(!os()) {
      Exception::raise_io_error(state, "closed directory");
    }
  }

  Object* Dir::open(STATE, String* path, Object* enc) {
    if(os()) closedir(os());

    os(opendir(path->c_str_null_safe(state)));

    if(!os()) {
      Exception::raise_errno_error(state, "Unable to open directory", errno, path->c_str_null_safe(state));
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

  Object* Dir::fileno(STATE) {
    int fd = dirfd(os());

    return Fixnum::from(fd);
  }

  Object* Dir::close(STATE) {
    if(os()) {
      closedir(os());
      os(0);
    }

    return cNil;
  }

  Object* Dir::closed_p(STATE) {
    return RBOOL(!os());
  }

  Object* Dir::read(STATE) {
    guard(state);

    struct dirent ent;
    struct dirent* entp = &ent;
    if(int erno = readdir_r(os(), entp, &entp)) {
      Exception::raise_errno_error(state, "readdir_r(3) failed", erno);
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
      seekdir(os(), pos->to_native());
      return cTrue;
    case 1:
      rewinddir(os());
      return cTrue;
    case 2:
      return Integer::from(state, telldir(os()));
    }
    return cNil;
  }
}

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

#include <sys/types.h>
#include <dirent.h>

namespace rubinius {
  void Dir::init(STATE) {
    GO(dir).set(state->new_class("Dir", G(object), Dir::fields));
    G(dir)->set_object_type(state, DirType);
  }

  Dir* Dir::create(STATE) {
    Dir* d = (Dir*)state->om->new_object(G(dir), Dir::fields);
    d->data(state, (MemoryPointer*)Qnil);

    return d;
  }

  void Dir::guard(STATE) {
    if(data_->nil_p()) {
      Exception::io_error(state, "closed directory");
    }
  }

  Object* Dir::open(STATE, String* path) {
    DIR* d = opendir(path->c_str());

    if(!d) Exception::errno_error(state, "Unable to open directory");
    data(state, MemoryPointer::create(state, d));

    return Qnil;
  }

  Object* Dir::close(STATE) {
    guard(state);

    DIR* d = (DIR*)data_->pointer;
    if(d) {
      data(state, (MemoryPointer*)Qnil);
      closedir(d);
      return Qtrue;
    }

    return Qfalse;
  }

  Object* Dir::closed_p(STATE) {
    return data_->nil_p() ? Qtrue : Qfalse;
  }

  Object* Dir::read(STATE) {
    guard(state);

    DIR* d = (DIR*)data_->pointer;
    struct dirent *ent = readdir(d);

    if(!ent) return Qnil;

    return String::create(state, ent->d_name);
  }

  Object* Dir::control(STATE, Fixnum* kind, Integer* pos) {
    guard(state);

    DIR* d = (DIR*)data_->pointer;

    switch(kind->to_native()) {
    case 0:
      seekdir(d, pos->to_native());
      return Qtrue;
    case 1:
      rewinddir(d);
      return Qtrue;
    case 2:
      return Integer::from(state, telldir(d));
    }
    return Qnil;
  }
}

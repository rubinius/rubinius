#include "memory.hpp"
#include "primitives.hpp"
#include "thread_phase.hpp"

#include "class/class.hpp"
#include "class/object.hpp"
#include "class/stat.hpp"
#include "class/string.hpp"
#include "class/time.hpp"

namespace rubinius {
  void Stat::bootstrap(STATE) {
    GO(stat_class).set(state->memory()->new_class<Class, Stat>(
          state, G(rubinius), "Stat"));
  }

  Stat* Stat::allocate(STATE, Object* self) {
    return state->memory()->new_object<Stat>(state, as<Class>(self));
  }

  Fixnum* Stat::stat(STATE, String* p) {
    UnmanagedPhase unmanaged(state);

    path(state, p);
    return Fixnum::from(::stat(p->c_str_null_safe(state), &st_));
  }

  Fixnum* Stat::fstat(STATE, Integer* fd) {
    UnmanagedPhase unmanaged(state);

    return Fixnum::from(::fstat(fd->to_native(), &st_));
  }

  Fixnum* Stat::lstat(STATE, String* p) {
    UnmanagedPhase unmanaged(state);

    path(state, p);
    return Fixnum::from(::lstat(p->c_str_null_safe(state), &st_));
  }

  Integer* Stat::stat_dev(STATE) {
    return Integer::from(state, st_.st_dev);
  }

  Integer* Stat::stat_ino(STATE) {
    return Integer::from(state, st_.st_ino);
  }

  Integer* Stat::stat_mode(STATE) {
    return Integer::from(state, st_.st_mode);
  }

  Integer* Stat::stat_nlink(STATE) {
    return Integer::from(state, st_.st_nlink);
  }

  Integer* Stat::stat_uid(STATE) {
    return Integer::from(state, st_.st_uid);
  }

  Integer* Stat::stat_gid(STATE) {
    return Integer::from(state, st_.st_gid);
  }

  Integer* Stat::stat_rdev(STATE) {
    return Integer::from(state, st_.st_rdev);
  }

  Integer* Stat::stat_size(STATE) {
    return Integer::from(state, st_.st_size);
  }

  Integer* Stat::stat_blksize(STATE) {
    return Integer::from(state, st_.st_blksize);
  }

  Integer* Stat::stat_blocks(STATE) {
    return Integer::from(state, st_.st_blocks);
  }

  Time* Stat::stat_atime(STATE) {
    #ifdef HAVE_STRUCT_STAT_ST_ATIM
      return Time::at(state, st_.st_atim.tv_sec, st_.st_atim.tv_nsec);
    #elif HAVE_STRUCT_STAT_ST_ATIMESPEC
      return Time::at(state, st_.st_atimespec.tv_sec, st_.st_atimespec.tv_nsec);
    #elif HAVE_STRUCT_STAT_ST_ATIMENSEC
      return Time::at(state, st_.st_atime, static_cast<long>(st_.st_atimensec));
    #else
      return Time::at(state, st_.st_atime);
    #endif
  }

  Time* Stat::stat_mtime(STATE) {
    #ifdef HAVE_STRUCT_STAT_ST_MTIM
      return Time::at(state, st_.st_mtim.tv_sec, st_.st_mtim.tv_nsec);
    #elif HAVE_STRUCT_STAT_ST_MTIMESPEC
      return Time::at(state, st_.st_mtimespec.tv_sec, st_.st_mtimespec.tv_nsec);
    #elif HAVE_STRUCT_STAT_ST_MTIMENSEC
      return Time::at(state, st_.st_mtime, static_cast<long>(st_.st_mtimensec));
    #else
      return Time::at(state, st_.st_mtime);
    #endif
  }

  Time* Stat::stat_ctime(STATE) {
    #ifdef HAVE_STRUCT_STAT_ST_CTIM
      return Time::at(state, st_.st_ctim.tv_sec, st_.st_ctim.tv_nsec);
    #elif HAVE_STRUCT_STAT_ST_CTIMESPEC
      return Time::at(state, st_.st_ctimespec.tv_sec, st_.st_ctimespec.tv_nsec);
    #elif HAVE_STRUCT_STAT_ST_CTIMENSEC
      return Time::at(state, st_.st_ctime, static_cast<long>(st_.st_ctimensec));
    #else
      return Time::at(state, st_.st_ctime);
    #endif
  }

  Object* Stat::stat_birthtime(STATE) {
    #ifdef HAVE_ST_BIRTHTIME
      struct timespec ts = st_.st_birthtimespec;
      return Time::at(state, ts.tv_sec, ts.tv_nsec);
    #else
      return Primitives::failure();
    #endif
  }
}

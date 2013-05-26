#include "builtin/class.hpp"
#include "builtin/object.hpp"
#include "builtin/stat.hpp"
#include "builtin/string.hpp"
#include "builtin/time.hpp"
#include "ontology.hpp"

namespace rubinius {
  void Stat::init(STATE) {
    GO(stat_class).set(ontology::new_class(state, "Stat", G(object), G(rubinius)));
    G(stat_class)->set_object_type(state, StatType);
  }

  Fixnum* Stat::stat(STATE, String* p) {
    path(state, p);
    return Fixnum::from(::stat(p->c_str_null_safe(state), &st_));
  }

  Fixnum* Stat::fstat(STATE, Integer* fd) {
    return Fixnum::from(::fstat(fd->to_native(), &st_));
  }

  Fixnum* Stat::lstat(STATE, String* p) {
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
    return Time::at(state, st_.st_atime);
  }

  Time* Stat::stat_mtime(STATE) {
    return Time::at(state, st_.st_mtime);
  }

  Time* Stat::stat_ctime(STATE) {
    return Time::at(state, st_.st_ctime);
  }

}


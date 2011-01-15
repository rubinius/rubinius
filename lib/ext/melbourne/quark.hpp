#ifndef MEL_QUARK_HPP
#define MEL_QUARK_HPP

#include <stddef.h>

namespace melbourne {
  typedef size_t quark;
  enum {QUARK_NOT_FOUND = ~0L};

  quark quark_from_string(const char* str);
  const char* quark_to_string(const quark quark);
}
#endif


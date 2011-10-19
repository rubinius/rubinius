#ifndef MEL_GRAMMAR18_HPP
#define MEL_GRAMMAR18_HPP

#include "bstrlib.h"

namespace melbourne {
  namespace grammar18 {
    VALUE string_to_ast(VALUE ptp, const char *f, bstring s, int line);
    VALUE file_to_ast(VALUE ptp, const char *f, FILE *file, int start);
  };
};

#endif

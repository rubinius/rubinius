#ifndef MEL_GRAMMAR_HPP
#define MEL_GRAMMAR_HPP

#include "internal.hpp"

namespace melbourne {
  namespace grammar18 {
    VALUE string_to_ast(VALUE ptp, const char *f, bstring s, int line);
    VALUE file_to_ast(VALUE ptp, const char *f, FILE *file, int start);
    quark id_to_quark(QUID id);
  };

  namespace grammar19 {
    VALUE string_to_ast(VALUE ptp, VALUE name, VALUE source, VALUE line);
    VALUE file_to_ast(VALUE ptp, const char *f, FILE *file, int start);
    quark id_to_quark(QUID id);
  };
};

#endif

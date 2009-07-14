#ifndef MEL_GRAMMAR_HPP
#define MEL_GRAMMAR_HPP

#include "internal.hpp"

namespace melbourne {

  VALUE string_to_ast(VALUE ptp, const char *f, bstring s, int line);
  VALUE file_to_ast(VALUE ptp, const char *f, FILE *file, int start);
  quark id_to_quark(QUID id);

};

#endif

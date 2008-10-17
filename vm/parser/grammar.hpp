#ifndef RBX_GRAMMAR_HPP
#define RBX_GRAMMAR_HPP

#include "parser/grammar_internal.hpp"

namespace rubinius {

  namespace parser {

    OBJECT syd_compile_string(STATE, const char *f, bstring s, int line);
    OBJECT syd_compile_file(STATE, const char *f, FILE *file, int start);
    quark id_to_quark(ID id);

  };
};

#endif

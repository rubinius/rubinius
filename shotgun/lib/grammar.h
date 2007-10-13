#include "grammar_internal.h"

OBJECT syd_compile_string(STATE, const char *f, bstring s, int line, int newlines);
OBJECT syd_compile_file(STATE, const char *f, FILE *file, int start, int newlines);
quark id_to_quark(ID id);

#include "grammar_internal.h"

OBJECT syd_compile_string(STATE, const char *f, GString *s, int line, int newlines);
OBJECT syd_compile_file(STATE, const char *f, FILE *file, int start, int newlines);
GQuark id_to_quark(ID id);

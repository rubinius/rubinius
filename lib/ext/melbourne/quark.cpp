#include "quark.hpp"
#include "parser_state18.hpp"

#include <string.h>
#include <stdlib.h>

using namespace melbourne::grammar18;

quark melbourne::grammar18::quark_from_string(rb_parser_state* parser_state, const char* str) {
  if (str == NULL) {
    return QUARK_NOT_FOUND;
  }

  /* attempt to find it in our cache */
  quark_map::iterator it = quark_indexes->find(str);
  if (it != quark_indexes->end()) {
    return it->second;
  }

  /* otherwise, we need to duplicate and store the string */
  const char* new_quark = strdup(str);
  quarks->push_back(new_quark);
  size_t index = quarks->size() - 1;
  quark_indexes->insert(quark_map::value_type(new_quark,index));
  return index;
}

const char* melbourne::grammar18::quark_to_string(rb_parser_state* parser_state, quark q) {
  if (q >= quarks->size())
    return NULL;
  return quarks->at(q);
}

void melbourne::grammar18::quark_cleanup(rb_parser_state* parser_state) {
  delete quark_indexes;

  for (quark_vector::iterator it = quarks->begin(); it != quarks->end(); ++it) {
    free((char *)*it);
  }

  delete quarks;
}

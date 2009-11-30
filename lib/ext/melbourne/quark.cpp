#include "quark.hpp"
#include <tr1/unordered_map>
#include <vector>
#include <string.h>

using namespace melbourne;

struct ConstCharHash {
  size_t operator() (const char* value) const {
    size_t length = strlen(value);
    size_t hash = 0;
    for (size_t i = 0; i < length; i++) {
      hash = hash * 5 + value[i];
    }
    return hash;
  }
};

struct ConstCharEqualTo {
  bool operator() ( const char* lhs, const char* rhs) const {
    return strcmp(lhs, rhs) == 0;
  }
};

typedef std::tr1::unordered_map<const char*, int, ConstCharHash, ConstCharEqualTo> quark_map;
typedef std::vector<const char*> quark_vector;

static quark_map quark_indexes;
static quark_vector quarks;

quark melbourne::quark_from_string(const char* str) {
  if (str == NULL)
    return QUARK_NOT_FOUND;

  /* attempt to find it in our cache */
  quark_map::iterator it = quark_indexes.find(str);
  if (it != quark_indexes.end())
    return it->second;

  /* otherwise, we need to duplicate and store the string */
  const char* new_quark = strdup(str);
  quarks.push_back(new_quark);
  size_t index = quarks.size() - 1;
  quark_indexes.insert(quark_map::value_type(new_quark,index));
  return index;
}

const char* melbourne::quark_to_string(quark q) {
  if (q >= quarks.size())
    return NULL;
  return quarks[q];
}

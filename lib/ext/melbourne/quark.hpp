#ifndef MEL_QUARK_HPP
#define MEL_QUARK_HPP

#include <stddef.h>
#include <cmath>
#include <vector>
#include <tr1/unordered_map>
#include <string.h>

namespace melbourne {

  namespace grammar18 {

    typedef size_t quark;

    struct ConstCharHash {
      size_t operator() (const char* value) const {
        size_t length = strlen(value);
        quark hash = 0;
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

    struct rb_parser_state;
    enum {QUARK_NOT_FOUND = ~0L};

    quark quark_from_string(rb_parser_state* parser_state, const char* str);
    const char* quark_to_string(rb_parser_state* parser_state, const quark quark);
    void quark_cleanup(rb_parser_state* parser_state);

    typedef std::tr1::unordered_map<const char*, quark, ConstCharHash, ConstCharEqualTo> quark_map;
    typedef std::vector<const char*> quark_vector;
  };
};
#endif


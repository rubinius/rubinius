#include "alloc.hpp"

#include "builtin/trie.hpp"

namespace rubinius {
  uintptr_t Trie::fields_offset;

  void Trie::bootstrap(STATE) {
    Trie* trie = ALLOCA(Trie);
    fields_offset = (uintptr_t)&(trie->field) - (uintptr_t)trie;
  }
}

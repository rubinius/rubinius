#include "alloc.hpp"

#include "class/trie.hpp"

namespace rubinius {
  uintptr_t Trie::fields_offset;

  void Trie::bootstrap(STATE) {
    Trie* trie = ALLOCA(Trie);
    fields_offset = (uintptr_t)&(trie->field) - (uintptr_t)trie;
  }
}

#ifndef RBX_BUILTIN_TRIE_HPP
#define RBX_BUILTIN_TRIE_HPP

#include "memory.hpp"

#include "class/object.hpp"

namespace rubinius {
  class Trie : public DataHeader {
  public:
    const static object_type type = TrieType;
    static uintptr_t fields_offset;

    attr_field(full_size, intptr_t);
    attr_field(bitmap, intptr_t);

    Object* field[0];

  public:
    intptr_t num_fields() const {
      return (full_size() - fields_offset) / sizeof(Object*);
    }

    static void bootstrap(STATE);

    class Info : public TypeInfo {
    public:
      Info(object_type type)
        : TypeInfo(type)
      {
        allow_user_allocate = false;
      }

      virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {}
      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {}
      virtual void show(STATE, Object* self, int level) {}
      virtual void show_simple(STATE, Object* self, int level) {}
      virtual size_t object_size(const ObjectHeader* object) { return sizeof(Trie); }
    };
  };
}
#endif

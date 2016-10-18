#ifndef RBX_BUILTIN_TRIE_HPP
#define RBX_BUILTIN_TRIE_HPP

#include "memory.hpp"

#include "class/object.hpp"

namespace rubinius {
  class Trie : public DataHeader {
  public:
    const static object_type type = TrieType;
    static uintptr_t fields_offset;

    attr_field(full_size, native_int);
    attr_field(bitmap, native_int);

    Object* field[0];

  public:
    native_int num_fields() const {
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

      virtual void mark(Object* obj, memory::ObjectMark& mark) {}
      virtual void auto_mark(Object* obj, memory::ObjectMark& mark) {}
      virtual void show(STATE, Object* self, int level) {}
      virtual void show_simple(STATE, Object* self, int level) {}
      virtual size_t object_size(const ObjectHeader* object) { return sizeof(Trie); }
    };
  };
}
#endif

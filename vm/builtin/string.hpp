#ifndef RBX_STRING_HPP
#define RBX_STRING_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"
#include "objects.hpp" // FIX: nuke objects.hpp... it is a mess

namespace rubinius {
  class ByteArray;

  class String : public Object {
    public:
    const static size_t fields = 7;
    const static object_type type = StringType;

    OBJECT __ivars__; // slot
    INTEGER num_bytes; // slot
    INTEGER characters; // slot
    OBJECT encoding; // slot
    ByteArray* data; // slot
    INTEGER hash; // slot
    OBJECT shared; // slot

    // Ruby.primitive :string_allocate
    static String* allocate(STATE, FIXNUM size);

    static String* create(STATE, const char* str, size_t bytes = 0);
    static hashval hash_str(const unsigned char *bp, unsigned int sz);
    static bool string_equal_p(STATE, OBJECT self, OBJECT other);
	// Ruby.primitive :string_equal
	OBJECT equal(STATE, String* other);

    size_t size(STATE) {
      return num_bytes->n2i();
    }

    size_t size() {
      return num_bytes->n2i();
    }

    /* TODO: since we're technically say it's ok to change this, we might
     * want to copy it first. */
    operator char *() {
      return (char*)(data->bytes);
    }

    char* byte_address() {
      return (char*)data->bytes;
    }

    void unshare(STATE);
    hashval hash_string(STATE);
    // Ruby.primitive :symbol_lookup
    SYMBOL to_sym(STATE);
    char* byte_address(STATE);

    // Ruby.primitive :string_dup
    String* string_dup(STATE);

    // Ruby.primitive :string_append
    String* append(STATE, String* other);
    String* append(STATE, const char* other);
    String* add(STATE, String* other);
    String* add(STATE, const char* other);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, OBJECT self);
    };

  };
};

#endif

#ifndef RBX_STRING_HPP
#define RBX_STRING_HPP

namespace rubinius {
  class String : public BuiltinType {
    public:
    const static size_t fields = 6;
    const static object_type type = StringType;

    static bool is_a(OBJECT obj) {
      return obj->reference_p() && obj->obj_type == StringType;
    }

    INTEGER num_bytes;
    OBJECT characters;
    OBJECT encoding;
    OBJECT data;
    OBJECT hash;
    OBJECT shared;

    static String* create(STATE, const char* str, size_t bytes = 0);
    static hashval hash_str(const unsigned char *bp, unsigned int sz);
    static int string_equal_p(STATE, OBJECT self, OBJECT other);

    size_t size(STATE) {
      return num_bytes->n2i();
    }

    size_t size() {
      return num_bytes->n2i();
    }

    /* Allows the String object to be cast as a char* */
    operator const char *() {
      return (const char*)(data->bytes);
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
    SYMBOL to_sym(STATE);
    char* byte_address(STATE);
    String* string_dup(STATE);
    String* append(STATE, String* other);
    String* append(STATE, char* other);
    String* add(STATE, String* other);
    String* add(STATE, char* other);

  };
};

#endif

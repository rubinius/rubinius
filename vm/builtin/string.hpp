#ifndef RBX_STRING_HPP
#define RBX_STRING_HPP

#include "builtin/object.hpp"
#include "type_info.hpp"
#include <ctype.h> // For isdigit and friends
#include <cerrno> // For ERANGE

// copied from ruby 1.8.x source, ruby.h
/* need to include <ctype.h> to use these macros */
#ifndef ISPRINT
#define ISASCII(c) isascii((int)(unsigned char)(c))
#undef ISPRINT
#define ISPRINT(c) (ISASCII(c) && isprint((int)(unsigned char)(c)))
#define ISSPACE(c) (ISASCII(c) && isspace((int)(unsigned char)(c)))
#define ISUPPER(c) (ISASCII(c) && isupper((int)(unsigned char)(c)))
#define ISLOWER(c) (ISASCII(c) && islower((int)(unsigned char)(c)))
#define ISALNUM(c) (ISASCII(c) && isalnum((int)(unsigned char)(c)))
#define ISALPHA(c) (ISASCII(c) && isalpha((int)(unsigned char)(c)))
#define ISDIGIT(c) (ISASCII(c) && isdigit((int)(unsigned char)(c)))
#define ISXDIGIT(c) (ISASCII(c) && isxdigit((int)(unsigned char)(c)))
#endif


namespace rubinius {
  class ByteArray;
  class Float;

  class String : public Object {
    public:
    const static size_t fields = 6;
    const static object_type type = StringType;

    INTEGER num_bytes; // slot
    INTEGER characters; // slot
    OBJECT encoding; // slot
    ByteArray* data; // slot
    INTEGER hash_value; // slot
    OBJECT shared; // slot

    static void init(STATE);

    // Ruby.primitive :string_allocate
    static String* allocate(STATE, FIXNUM size);

    static String* create(STATE, const char* str, size_t bytes = 0);
    static hashval hash_str(const unsigned char *bp, unsigned int sz);
    static bool string_equal_p(STATE, OBJECT self, OBJECT other);
    // Ruby.primitive :string_equal
    OBJECT equal(STATE, String* other);

    // Returns the number of bytes this String contains
    size_t size();

    // Access the String as a char* directly. WARNING: doesn't necessarily
    // return a null terminated char*, so be sure to use size() with it.
    //
    // NOTE: do not free() or realloc() this buffer.
    char* byte_address();

    // Use this version if you want to use this String as a null
    // terminated char*.
    // It doesn't return a copy, it just makes sure that the String
    // object's data is null clamped properly.
    //
    // NOTE: do not free() or realloc() this buffer.
    char* c_str();

    void unshare(STATE);
    hashval hash_string(STATE);
    // Ruby.primitive :symbol_lookup
    SYMBOL to_sym(STATE);

    // Ruby.primitive :string_dup
    String* string_dup(STATE);

    // Ruby.primitive :string_append
    String* append(STATE, String* other);
    String* append(STATE, const char* other);
    String* add(STATE, String* other);
    String* add(STATE, const char* other);

    // Ruby.primitive :string_to_f
    Float* to_f(STATE);
    double to_double(STATE);

    // Ruby.primitive :string_apply_and
    String* apply_and(STATE, String* other);

    // Ruby.primitive :string_tr_expand
    FIXNUM tr_expand(STATE, OBJECT limit);
    FIXNUM tr_replace(STATE, struct tr_data* data);

    // Ruby.primitive :string_copy_from
    String* copy_from(STATE, String* other, FIXNUM start, FIXNUM size, FIXNUM dest);

    // Ruby.primitive :string_compare_substring
    FIXNUM compare_substring(STATE, String* other, FIXNUM start, FIXNUM size);

    // Ruby.primitive :string_crypt
    String* crypt(STATE, String* salt);

    // Ruby.primitive :string_pattern
    static String* pattern(STATE, OBJECT self, FIXNUM size, OBJECT pattern);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, OBJECT self, int level);
      virtual void show_simple(STATE, OBJECT self, int level);
    };
  };
};

#endif

#ifndef RBX_STRING_HPP
#define RBX_STRING_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
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

  private:
    Integer* num_bytes_;  // slot
    Integer* characters_; // slot
    Object* encoding_;    // slot
    ByteArray* data_;    // slot
    Integer* hash_value_; // slot
    Object* shared_;      // slot

  public:
    /* accessors */

    attr_accessor(num_bytes, Integer);
    attr_accessor(characters, Integer);
    attr_accessor(encoding, Object);
    attr_accessor(data, ByteArray);
    attr_accessor(hash_value, Integer);
    attr_accessor(shared, Object);

    /* interface */

    static void init(STATE);

    static String* create(STATE, Fixnum* size);

    // Ruby.primitive :string_from_bytearray
    static String* from_bytearray(STATE, ByteArray* ba, Integer* start, Integer* count);
    static String* create(STATE, const char* str, size_t bytes = 0);
    static hashval hash_str(const unsigned char *bp, unsigned int sz);
    static bool string_equal_p(STATE, Object* self, Object* other);
    // Ruby.primitive :string_equal
    Object* equal(STATE, String* other);

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
    const char* c_str();

    void unshare(STATE);
    hashval hash_string(STATE);
    // Ruby.primitive :symbol_lookup
    Symbol* to_sym(STATE);

    // Ruby.primitive :string_dup
    String* string_dup(STATE);

    String* add(STATE, String* other);
    String* add(STATE, const char* other);

    /**
     *  Append other String to self.
     *
     *  Returns self.
     */
    // Ruby.primitive :string_append
    String* append(STATE, String* other);

    /**
     *  Append C string to self.
     *
     *  strlen() is used to calculate number of bytes to append.
     *  If the C string may contain NUL bytes, use the version
     *  that takes a length argument instead. Returns self.
     *
     *  @see String::append(VM*, const char*, std::size_t)
     */
    String* append(STATE, const char* other);

    /** Append length bytes from C string. Returns self. */
    String* append(STATE, const char* other, std::size_t length);


    // Ruby.primitive :string_to_f
    Float* to_f(STATE);
    double to_double(STATE);

    Integer* to_i(STATE, Fixnum* base = Fixnum::from(0), Object* strict = Qtrue);

    // Ruby.primitive :string_to_inum
    Integer* to_inum_prim(STATE, Fixnum* base, Object* strict);

    // Ruby.primitive :string_tr_expand
    Fixnum* tr_expand(STATE, Object* limit);
    Fixnum* tr_replace(STATE, struct tr_data* data);

    // Ruby.primitive :string_copy_from
    String* copy_from(STATE, String* other, Fixnum* start, Fixnum* size, Fixnum* dest);

    // Ruby.primitive :string_compare_substring
    Fixnum* compare_substring(STATE, String* other, Fixnum* start, Fixnum* size);

    // Ruby.primitive :string_crypt
    String* crypt(STATE, String* salt);

    // Ruby.primitive :string_pattern
    static String* pattern(STATE, Object* self, Fixnum* size, Object* pattern);

    // Ruby.primitive :string_parse
    Object* parse(STATE, String* name, Fixnum* line);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
    };
  };
};

#endif

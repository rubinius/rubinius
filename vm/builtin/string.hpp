#ifndef RBX_STRING_HPP
#define RBX_STRING_HPP

#include "builtin/object.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/chararray.hpp"

#include "type_info.hpp"
#include <ctype.h> // For isdigit and friends
#include <errno.h> // For ERANGE

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
  class CharArray;
  class Float;

  class String : public Object {
  public:
    const static object_type type = StringType;

  private:
    Fixnum* num_bytes_;  // slot
    Object* encoding_;   // slot
    CharArray* data_;    // slot
    Fixnum* hash_value_; // slot
    Object* shared_;     // slot

  public:
    /* accessors */

    attr_accessor(num_bytes, Fixnum);
    attr_accessor(encoding, Object);
    attr_accessor(data, CharArray);
    attr_accessor(hash_value, Fixnum);
    attr_accessor(shared, Object);

    /* interface */

    static void init(STATE);

    static String* create(STATE, Fixnum* size);

    // Rubinius.primitive :string_from_chararray
    static String* from_chararray(STATE, CharArray* ca, Fixnum* start, Fixnum* count);
    static String* create(STATE, const char* str);
    static String* create(STATE, const char* str, native_int bytes);
    static String* create_pinned(STATE, Fixnum* size);
    static String* create_reserved(STATE, native_int bytes);

    // Hash the NUL-terminated string _bp_.
    static hashval hash_str(const char *bp);

    // Hash the byte array _bp_ which contains _sz_ bytes.
    static hashval hash_str(const unsigned char *bp, unsigned int sz);

    // Rubinius.primitive :string_equal
    Object* equal(STATE, String* other) {
      if(this->num_bytes() != other->num_bytes()) return Qfalse;
      int comp = memcmp(
          this->byte_address(),
          other->byte_address(),
          this->num_bytes()->to_native());

      return comp == 0 ? Qtrue : Qfalse;
    }


    // Rubinius.primitive :string_secure_compare
    Object* secure_compare(STATE, String* other);

    // Returns the number of bytes this String contains
    native_int size() {
      return num_bytes_->to_native();
    }

    // Access the String as a char* directly. WARNING: doesn't necessarily
    // return a null terminated char*, so be sure to use size() with it.
    //
    // NOTE: do not free() or realloc() this buffer.
    uint8_t* byte_address() {
      return data_->raw_bytes();
    }

    // Use this version if you want to use this String as a null
    // terminated char*.
    // It doesn't return a copy, it just makes sure that the String
    // object's data is null clamped properly.
    //
    // NOTE: do not free() or realloc() this buffer.
    const char* c_str(STATE);

    void unshare(STATE);
    hashval hash_string(STATE);
    // Rubinius.primitive :symbol_lookup
    Symbol* to_sym(STATE);

    // Rubinius.primitive :string_dup
    String* string_dup(STATE);

    String* add(STATE, String* other);
    String* add(STATE, const char* other);

    /**
     *  Append other String to self.
     *
     *  Returns self.
     */
    // Rubinius.primitive :string_append
    String* append(STATE, String* other);

    /**
     *  Append C string to self.
     *
     *  strlen() is used to calculate number of bytes to append.
     *  If the C string may contain NUL bytes, use the version
     *  that takes a length argument instead. Returns self.
     *
     *  @see String::append(VM*, const char*, native_int)
     */
    String* append(STATE, const char* other);

    /** Append length bytes from C string. Returns self. */
    String* append(STATE, const char* other, native_int length);

    // Rubinius.primitive :string_to_f
    Float* to_f(STATE);
    double to_double(STATE);

    Integer* to_i(STATE, Fixnum* base = Fixnum::from(0), Object* strict = Qtrue);

    // Rubinius.primitive :string_to_inum
    Integer* to_inum_prim(STATE, Fixnum* base, Object* strict);

    // Rubinius.primitive :string_apply_and
    String* apply_and(STATE, String* other);

    // Rubinius.primitive :string_tr_expand
    Fixnum* tr_expand(STATE, Object* limit, Object* invalid_as_empty);
    Fixnum* tr_replace(STATE, struct tr_data* data);

    // Rubinius.primitive :string_copy_from
    String* copy_from(STATE, String* other, Fixnum* start, Fixnum* size, Fixnum* dest);

    // Rubinius.primitive :string_compare_substring
    Fixnum* compare_substring(STATE, String* other, Fixnum* start, Fixnum* size);

    // Rubinius.primitive :string_crypt
    String* crypt(STATE, String* salt);

    // Rubinius.primitive :string_pattern
    static String* pattern(STATE, Object* self, Fixnum* size, Object* pattern);

    // Rubinius.primitive :string_substring
    String* substring(STATE, Fixnum* start, Fixnum* count);

    // Rubinius.primitive :string_index
    Fixnum* index(STATE, String* pattern, Fixnum* start);

    // Rubinius.primitive :string_rindex
    Fixnum* rindex(STATE, String* pattern, Fixnum* start);

    // Rubinius.primitive :string_transform
    String* transform(STATE, Tuple* table, Object* respect_kcode);

    // Rubinius.primitive :string_find_character
    String* find_character(STATE, Fixnum* offset);

    // Rubinius.primitive :string_awk_split
    Array* awk_split(STATE, Fixnum* limit);

    // Rubinius.primitive :string_unpack18
    Array* unpack18(STATE, String *directives);

    // Rubinius.primitive :string_unpack19
    Array* unpack19(STATE, String *directives);

    // Rubinius.primitive :string_resize_capacity
    String* resize_capacity(STATE, Fixnum* count);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
    };
  };
};

#endif

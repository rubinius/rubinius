#ifndef RBX_STRING_HPP
#define RBX_STRING_HPP

#include "builtin/object.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/fixnum.hpp"

#include "object_utils.hpp"
#include "type_info.hpp"
#include <ctype.h> // For isdigit and friends
#include <errno.h> // For ERANGE

// See comment in regexp.hpp
#ifndef ONIGURUMA_H
struct OnigEncodingType;
#endif

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
  class Encoding;
  class Float;

  class String : public Object {
  public:
    const static object_type type = StringType;

  private:
    Fixnum* num_bytes_;       // slot
    Fixnum* num_chars_;       // slot
    ByteArray* data_;         // slot
    Fixnum* hash_value_;      // slot
    Object* shared_;          // slot
    Encoding* encoding_;      // slot
    Object* ascii_only_;      // slot
    Object* valid_encoding_;  // slot

  public:
    /* accessors */

    attr_reader(num_bytes, Fixnum);
    attr_reader(data, ByteArray);

    void update_handle();

    template <class T>
      void num_bytes(T state, Fixnum* obj) {
        num_bytes_ = obj;
        num_chars_ = nil<Fixnum>();
        update_handle();
      }

    template <class T>
      void data(T state, ByteArray* obj) {
        data_ = obj;
        if(mature_object_p()) this->write_barrier(state, obj);

        update_handle();
      }

    attr_accessor(num_chars, Fixnum);
    attr_accessor(hash_value, Fixnum);
    attr_accessor(shared, Object);
    attr_accessor(encoding, Encoding);
    attr_accessor(ascii_only, Object);
    attr_accessor(valid_encoding, Object);

    /* interface */

    static void init(STATE);

    static String* create(STATE, Fixnum* size);

    // Rubinius.primitive :string_from_bytearray
    static String* from_bytearray(STATE, ByteArray* ba, Fixnum* start, Fixnum* count);
    static String* from_bytearray(STATE, ByteArray* ba, native_int size);
    static String* create(STATE, const char* str);
    static String* create(STATE, const char* str, native_int bytes);
    static String* create_pinned(STATE, Fixnum* size);
    static String* create_reserved(STATE, native_int bytes);

    // Hash the byte array _bp_ which contains _sz_ bytes.
    static hashval hash_str(const unsigned char *bp, unsigned int sz, uint32_t seed);

    static hashval hash_str(STATE, const unsigned char *bp, unsigned int sz) {
      return hash_str(bp, sz, state->hash_seed());
    }

    // Rubinius.primitive :string_equal
    Object* equal(STATE, String* other) {
      if(this->num_bytes() != other->num_bytes()) return cFalse;
      int comp = memcmp(
          this->byte_address(),
          other->byte_address(),
          this->num_bytes()->to_native());

      return comp == 0 ? cTrue : cFalse;
    }


    // Rubinius.primitive :string_secure_compare
    Object* secure_compare(STATE, String* other);

    // Returns the number of bytes this String contains
    native_int byte_size() {
      return num_bytes_->to_native();
    }

    native_int char_size(STATE);

    // Rubinius.primitive :string_size
    Fixnum* size(STATE);

    // Access the String as a char* directly. WARNING: doesn't necessarily
    // return a null terminated char*, so be sure to use byte_size() with it.
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

    // Use this if you want to have a string that does not
    // contain a \0 character. It will raise an exception
    // if it does.
    const char* c_str_null_safe(STATE);

    // Rubinius.primitive :string_check_null_safe
    String* check_null_safe(STATE);

    String* convert_escaped(STATE, Encoding*& enc, bool& fixed_encoding);

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
    Float* to_f(STATE, Object* strict = cTrue);

    Integer* to_i(STATE, Fixnum* base = Fixnum::from(0), Object* strict = cTrue);

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

    // Rubinius.primitive :string_from_codepoint
    static String* from_codepoint(STATE, Object* self, Integer* code, Encoding* enc);

    // Rubinius.primitive :string_aref
    Object* aref(STATE, Fixnum* index);

    // Rubinius.primitive :string_substring
    String* substring(STATE, Fixnum* index, Fixnum* length);

    // Rubinius.primitive :string_byte_substring
    String* byte_substring(STATE, Fixnum* index, Fixnum* length);

    String* byte_substring(STATE, native_int index, native_int length);
    String* char_substring(STATE, native_int index, native_int length);

    native_int find_character_byte_index(STATE, native_int index, native_int start = 0);

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

    // Rubinius.primitive :string_encoding
    Encoding* encoding(STATE);

    // Rubinius.primitive :string_ascii_only_p
    Object* ascii_only_p(STATE);

    // Rubinius.primitive :string_valid_encoding_p
    Object* valid_encoding_p(STATE);

    // Rubinius.primitive :string_codepoint
    Fixnum* codepoint(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
    };
  };
};

#endif

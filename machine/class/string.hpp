#ifndef RBX_STRING_HPP
#define RBX_STRING_HPP

#include "class/object.hpp"
#include "class/byte_array.hpp"
#include "class/fixnum.hpp"
#include "class/encoding.hpp"

#include "configuration.hpp"
#include "object_utils.hpp"
#include "memory.hpp"
#include "memory/header.hpp"

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

    enum HeaderFlags {
      eRString = 1,
    };

    const static object_type type = StringType;

    Fixnum* _num_bytes_;  // slot
    attr_accessor(num_chars, Fixnum);
    attr_reader(data, ByteArray);
    attr_accessor(hash_value, Fixnum);
    attr_accessor(shared, Object);
    attr_reader(encoding, Encoding);
    attr_accessor(ascii_only, Object);
    attr_accessor(valid_encoding, Object);

    void read_rstring();
    void write_rstring(STATE);

    Fixnum* num_bytes() {
      if(type_specific() == eRString) {
        read_rstring();
      }

      return _num_bytes_;
    }

    void num_bytes(Fixnum* num) {
      _num_bytes_ = num;
    }

    Object** p_num_bytes() {
      return reinterpret_cast<Object**>(&this->_num_bytes_);
    }

    void num_bytes(STATE, Fixnum* obj) {
      num_bytes(obj);
      num_chars(nil<Fixnum>());

      if(type_specific() == eRString) {
        write_rstring(state);
      }
    }

    template<typename T>
    void data(T* state, ByteArray* obj) {
      data(obj);
      write_barrier(state, obj);

      if(type_specific() == eRString) {
        write_rstring(state);
      }
    }

    template<typename T>
    void encoding(T* state, Encoding* obj) {
      if(obj->nil_p() || (!CBOOL(ascii_only()) && obj->ascii_compatible())) {
        ascii_only(cNil);
        num_chars(nil<Fixnum>());
        valid_encoding(cNil);
      }
      if(byte_size() == 0 && !obj->nil_p() && obj->ascii_compatible()) {
        ascii_only(cTrue);
        num_chars(Fixnum::from(0));
        valid_encoding(cTrue);
      }
      encoding(obj);
      write_barrier(state, obj);
    }

    /* interface */

    static void init_hash();
    static void bootstrap(STATE);
    static void initialize(STATE, String* string) {
      string->num_bytes(nil<Fixnum>());
      string->num_chars(nil<Fixnum>());
      string->data(nil<ByteArray>());
      string->hash_value(nil<Fixnum>());
      string->shared(cFalse);
      string->encoding(nil<Encoding>());
      string->ascii_only(nil<Object>());
      string->valid_encoding(nil<Object>());
    }

    // Rubinius.primitive+ :string_allocate
    static String* allocate(STATE, Object* self);

    static String* create(STATE, Fixnum* size);

    // Rubinius.primitive :string_from_bytearray
    static String* from_bytearray(STATE, ByteArray* ba, Fixnum* start, Fixnum* count);
    static String* from_bytearray(STATE, ByteArray* ba, intptr_t size);
    static String* create(STATE, const char* str);
    static String* create(STATE, const char* str, intptr_t bytes);
    static String* create_pinned(STATE, Fixnum* size);
    static String* create_pinned(STATE, const char* str);
    static String* create_pinned(STATE, const char* str, intptr_t bytes);

    static uint64_t siphash(const unsigned char *bp, unsigned int sz, uint32_t seed);

    // Hash the byte array _bp_ which contains _sz_ bytes.
    static hashval hash_str(const unsigned char *bp, unsigned int sz, uint32_t seed);

    static hashval hash_str(STATE, const unsigned char *bp, unsigned int sz) {
      return hash_str(bp, sz, state->hash_seed());
    }

    // Rubinius.primitive+ :string_equal
    Object* equal(STATE, String* other) {
      if(!encoding()->equal_p(other->encoding()) &&
         Encoding::compatible_p(state, this, other)->nil_p()) return cFalse;
      if(this->num_bytes() != other->num_bytes()) return cFalse;
      int comp = memcmp(
          this->byte_address(),
          other->byte_address(),
          this->num_bytes()->to_native());

      return RBOOL(comp == 0);
    }


    // Rubinius.primitive+ :string_secure_compare
    Object* secure_compare(STATE, String* other);

    // Returns the number of bytes this String contains
    intptr_t byte_size() {
      return num_bytes()->to_native();
    }

    intptr_t get_byte_size() {
      return _num_bytes_->to_native();
    }

    intptr_t char_size(STATE);

    // Rubinius.primitive+ :string_size
    Fixnum* size(STATE);

    // Access the String as a char* directly. WARNING: doesn't necessarily
    // return a null terminated char*, so be sure to use byte_size() with it.
    //
    // NOTE: do not free() or realloc() this buffer.
    uint8_t* byte_address() const {
      return data()->raw_bytes();
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

    String* string_dup_slow(STATE);

    // Rubinius.primitive :string_dup
    String* string_dup(STATE) {
      if(likely(klass() == G(string) && ivars()->nil_p())) {
        /* We have a plain string with no metaclass or ivars, so if we can
         * allocate a new object in the young space, we can copy it directly
         * without needing a write barrier.
         */
        String* so = state->memory()->new_object<String>(state, G(string));
        // TODO: fix for generational heap
        // if(likely(so->young_object_p())) {
        if(true) {
          so->copy_body(state, this);
          so->shared(state, cTrue);
          shared(state, cTrue);
          infect(state, so);
          return so;
        }
      }
      return string_dup_slow(state);
    }

    void encoding_from(STATE, String* other) {
      encoding(state, other->encoding());

      if(other->ascii_only()->true_p()) {
        ascii_only(cTrue);
      } else {
        ascii_only(cNil);
      }
      if(other->valid_encoding()->true_p()) {
        valid_encoding(cTrue);
      } else {
        valid_encoding(cNil);
      }
    }

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
     * Append byte strings, ignores encoding
     */
    // Rubinius.primitive :string_byte_append
    String* byte_append(STATE, String* other);

    /**
     *  Append C string to self.
     *
     *  strlen() is used to calculate number of bytes to append.
     *  If the C string may contain NUL bytes, use the version
     *  that takes a length argument instead. Returns self.
     *
     *  @see String::append(VM*, const char*, intptr_t)
     */
    String* append(STATE, const char* other);

    /** Append length bytes from C string. Returns self. */
    String* append(STATE, const char* other, intptr_t length);

    // Rubinius.primitive :string_to_f
    Float* to_f(STATE, Object* strict = cTrue);

    Integer* to_i(STATE, Fixnum* base = Fixnum::from(0), Object* strict = cTrue);

    // Rubinius.primitive :string_to_inum
    Integer* to_inum_prim(STATE, Fixnum* base, Object* strict);

    // Rubinius.primitive :string_apply_and
    String* apply_and(STATE, String* other);

    // Rubinius.primitive :string_tr_expand
    Fixnum* tr_expand(STATE, Object* limit, Object* invalid_as_empty);
    intptr_t tr_replace(STATE, intptr_t first, intptr_t last, intptr_t start, intptr_t finish);

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

    String* byte_substring(STATE, intptr_t index, intptr_t length);
    String* char_substring(STATE, intptr_t index, intptr_t length);

    intptr_t find_character_byte_index(STATE, intptr_t index, intptr_t start = 0);
    intptr_t find_byte_character_index(STATE, intptr_t index, intptr_t start = 0);

    // Rubinius.primitive :string_character_byte_index
    Fixnum* find_character_byte_index_prim(STATE, Fixnum* index, Fixnum* start);

    // Rubinius.primitive :string_byte_character_index
    Fixnum* find_byte_character_index_prim(STATE, Fixnum* index, Fixnum* start);

    // Rubinius.primitive :string_index
    Fixnum* index(STATE, String* pattern, Fixnum* start);

    // Rubinius.primitive :string_character_index
    Fixnum* character_index(STATE, String* pattern, Fixnum* start);

    // Rubinius.primitive :string_rindex
    Fixnum* rindex(STATE, String* pattern, Fixnum* start);

    // Rubinius.primitive :string_byte_index
    Fixnum* byte_index(STATE, Object* value, Fixnum* start);

    // Rubinius.primitive :string_previous_byte_index
    Fixnum* previous_byte_index(STATE, Fixnum* index);

    // Rubinius.primitive :string_transform
    String* transform(STATE, Tuple* table);

    // Rubinius.primitive :string_find_character
    String* find_character(STATE, Fixnum* offset);

    // Rubinius.primitive :string_awk_split
    Array* awk_split(STATE, Fixnum* limit);

    // Rubinius.primitive :string_unpack
    Array* unpack(STATE, String *directives);

    // Rubinius.primitive :string_resize_capacity
    String* resize_capacity(STATE, Fixnum* count);

    // Rubinius.primitive+ :string_encoding
    Encoding* encoding(STATE);

    // Rubinius.primitive+ :string_ascii_only_p
    Object* ascii_only_p(STATE);

    // Rubinius.primitive+ :string_valid_encoding_p
    Object* valid_encoding_p(STATE);

    int codepoint(STATE, bool* found);

    // Rubinius.primitive :string_codepoint
    Fixnum* codepoint(STATE);

    // Rubinius.primitive :string_chr_at
    Object* chr_at(STATE, Fixnum* byte);

    // Rubinius.primitive :string_reverse
    String* reverse(STATE);

    class Info : public TypeInfo {
    public:
      BASIC_TYPEINFO(TypeInfo)
      virtual void show(STATE, Object* self, int level);
      virtual void show_simple(STATE, Object* self, int level);
    };
  };
};

#endif

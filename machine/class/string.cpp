#include "oniguruma.h" // Must be first.
#include "transcoder.h"
#include "regenc.h"

#include "configuration.hpp"
#include "memory.hpp"
#include "object_utils.hpp"
#include "primitives.hpp"
#include "spinlock.hpp"

#include "class/array.hpp"
#include "class/byte_array.hpp"
#include "class/character.hpp"
#include "class/class.hpp"
#include "class/encoding.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/float.hpp"
#include "class/integer.hpp"
#include "class/native_method.hpp"
#include "class/regexp.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"

#include "util/murmur_hash3.hpp"
#include "util/siphash.h"
#include "util/random.h"

#include "missing/string.h"

#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sstream>

namespace rubinius {

  static uint64_t siphash_key = 0;

  void String::init_hash() {
    uint32_t seed[4];
    random_seed(seed, 4);

    // It's important to pull these out into locals so that the compiler
    // promotes them to 64bits before we do the OR + XOR dance below,
    // otherwise if it's easy for the compiler to not promote and for
    // us to just lose the high bits.

    uint64_t s1 = (uint64_t)seed[0];
    uint64_t s2 = (uint64_t)seed[1];
    uint64_t s3 = (uint64_t)seed[2];
    uint64_t s4 = (uint64_t)seed[3];

    siphash_key = (s1 | (s2 << 32)) ^ (s3 | (s4 << 32));
  }

  void String::bootstrap(STATE) {
    GO(string).set(state->memory()->new_class<Class, String>(state, "String"));
  }

  String* String::allocate(STATE, Object* self) {
    return state->memory()->new_object<String>(state, as<Class>(self));
  }

  /* Creates a String instance with +num_bytes+ == +size+ and
   * having a ByteArray with at least (size + 1) bytes.
   */
  String* String::create(STATE, Fixnum* size) {
    String *so = state->memory()->new_object<String>(state, G(string));

    so->num_bytes(size);

    intptr_t bytes = size->to_native() + 1;
    so->data(state, ByteArray::create(state, bytes));

    return so;
  }

  /*
   * Creates a String instance with +num_bytes+ bytes of storage.
   * It also pins the ByteArray used for storage, so it can be passed
   * to an external function (like ::read)
   */
  String* String::create_pinned(STATE, Fixnum* size) {
    String *so = state->memory()->new_object<String>(state, G(string));

    so->num_bytes(state, size);

    intptr_t bytes = size->to_native() + 1;
    ByteArray* ba = ByteArray::create_pinned(state, bytes);

    so->data(state, ba);

    return so;
  }

  String* String::create_pinned(STATE, const char* str) {
    if(!str) return String::create(state, Fixnum::from(0));

    intptr_t bytes = strlen(str);

    return String::create_pinned(state, str, bytes);
  }

  String* String::create_pinned(STATE, const char* str, intptr_t bytes) {
    String* so = state->memory()->new_object<String>(state, G(string));

    so->num_bytes(Fixnum::from(bytes));

    ByteArray* ba =
      state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), bytes + 1);
    ba->set_pinned(state);

    if(str) {
      memcpy(ba->raw_bytes(), str, bytes);
      ba->raw_bytes()[bytes] = 0;
    } else {
      memset(ba->raw_bytes(), 0, bytes + 1);
    }

    so->data(state, ba);
    return so;
  }

  String* String::create(STATE, const char* str) {
    if(!str) return String::create(state, Fixnum::from(0));

    intptr_t bytes = strlen(str);

    return String::create(state, str, bytes);
  }

  /* +bytes+ should NOT attempt to take the trailing null into account
   * +bytes+ is the number of 'real' characters in the string
   */
  String* String::create(STATE, const char* str, intptr_t bytes) {
    String* so = state->memory()->new_object<String>(state, G(string));

    so->num_bytes(Fixnum::from(bytes));

    ByteArray* ba =
      state->memory()->new_bytes<ByteArray>(state, G(bytearray), bytes + 1);

    if(str) {
      memcpy(ba->raw_bytes(), str, bytes);
      ba->raw_bytes()[bytes] = 0;
    } else {
      memset(ba->raw_bytes(), 0, bytes + 1);
    }

    so->data(state, ba);
    return so;
  }

  String* String::from_bytearray(STATE, ByteArray* ba, intptr_t size) {
    String* s = state->memory()->new_object<String>(state, G(string));

    s->num_bytes(Fixnum::from(size));
    s->data(state, ba);

    return s;
  }

  String* String::from_bytearray(STATE, ByteArray* ba, Fixnum* start,
                                 Fixnum* count)
  {
    String* s = state->memory()->new_object<String>(state, G(string));

    s->num_bytes(state, count);

    // fetch_bytes NULL terminates
    s->data(state, ba->fetch_bytes(state, start, count));

    return s;
  }

  void String::read_rstring() {
    ThreadState* state = ThreadState::current();

    if(memory_handle_p()) {
      get_handle(state)->read_rstring(state);
    }
  }

  void String::write_rstring(STATE) {
    if(memory_handle_p()) {
      get_handle(state)->write_rstring(state);
    }
  }

  static bool byte_compatible_p(Encoding* enc) {
    return enc->nil_p() || ONIGENC_MBC_MAXLEN(enc->encoding()) == 1;
  }

  static bool fixed_width_p(Encoding* enc) {
    if(enc->nil_p()) return true;

    OnigEncodingType* e = enc->encoding();
    return ONIGENC_MBC_MAXLEN(e) == ONIGENC_MBC_MINLEN(e);
  }

  static void invalid_codepoint_error(STATE, unsigned int c) {
    std::ostringstream msg;
    msg << "invalid codepoint: " << c;
    Exception::raise_range_error(state, msg.str().c_str());
  }

  struct convert_escaped_state {
#define CONVERT_BUFSIZE 6

    STATE;
    String* str;
    OnigEncodingType* enc;
    bool ascii;
    bool utf8;
    ByteArray* ba;
    uint8_t* bp;
    uint8_t* be;
    uint8_t* p;
    uint8_t* e;
    uint8_t* lp;
    uint8_t buf[CONVERT_BUFSIZE];

    convert_escaped_state(STATE, String* str)
      : state(state)
      , str(str)
      , enc(0)
      , ascii(true)
      , utf8(false)
      , ba(0)
      , bp(0)
      , be(0)
    {
      p = str->byte_address();
      e = p + str->byte_size();
      lp = p;
      buf[0] = 0;
    }

    void update_index(int delta=0) {
      lp = p + delta;
    }

    void new_bytes(int len) {
      ba = ByteArray::create(state, len + 1);
      bp = ba->raw_bytes();
      be = bp + ba->size();
    }

    void expand_bytes(int len) {
      ByteArray* old_ba = ba;
      uint8_t* old_bp = bp;

      new_bytes(old_ba->size() + len);
      memcpy(ba->raw_bytes(), old_ba->raw_bytes(), old_ba->size());
      bp = ba->raw_bytes() + (old_bp - old_ba->raw_bytes());
    }

    void copy_bytes(int delta=0) {
      if(!ba) new_bytes(str->byte_size());

      int len = (p + delta) - lp;
      if(len > 0) {
        if(len > be - bp) {
          expand_bytes(len);
        }
        memcpy(bp, lp, len);
        bp += len;
      }

      update_index(delta);
    }

    void append_bytes(uint8_t* ptr, int len) {
      if(len > be - bp) {
        expand_bytes(len);
      }
      memcpy(bp, ptr, len);
      bp += len;
    }

    void append_utf8(int value) {
      if(value < 128) {
        int n = snprintf(reinterpret_cast<char*>(buf),
                         CONVERT_BUFSIZE, "\\x%02X", value);
        append_bytes(buf, n);
      } else {
        if(!utf8) {
          ascii = false;
          utf8 = true;
          enc = Encoding::utf8_encoding(state)->encoding();
        }

        int n = ONIGENC_CODE_TO_MBCLEN(enc, value);
        if(n <= 0) invalid_codepoint_error(state, value);

        n = ONIGENC_CODE_TO_MBC(enc, value, (UChar*)bp);
        if(n <= 0) invalid_codepoint_error(state, value);
        bp += n;
      }
    }

    int convert_ctrl(bool ctrl=false, bool meta=false) {
      if(ctrl) raise_error("duplicate control escape");
      if(p == e) raise_error("control escape is too short");

      if((*p & 0x80) == 0) {
        int value = *p++;

        if(value == '\\') {
          value = convert_byte(true, meta);
        }

        return value &= 0x1f;
      }

      raise_error("invalid control escape");

      // not reached
      return 0;
    }

    int convert_meta(bool ctrl=false, bool meta=false) {
      if(meta) raise_error("duplicate meta escape");
      if(p == e) raise_error("meta escape is too short");

      if((*p & 0x80) == 0) {
        int value = *p++;

        if(value == '\\') {
          value = convert_byte(true, meta);
        }

        return value |= 0x80;
      }

      raise_error("invalid meta escape");

      // not reached
      return 0;
    }

    int convert_byte(bool ctrl=false, bool meta=false) {
      if(p == e || *p++ != '\\') {
        raise_error("escape multibyte character is too short");
      }

      switch(*p++) {
        case '\\': return '\\';
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case 'f': return '\f';
        case 'v': return '\013';
        case 'a': return '\007';
        case 'e': return '\033';

        /* \OOO */
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
          p--;
          return convert_octal();

        case 'x': /* \xHH */
          return convert_hex(1, 2);

        case 'M': /* \M-X, \M-\C-X, \M-\cX */
          if(p == e || *p++ != '-') {
            raise_error("meta escape is too short");
          }
          return convert_meta(ctrl);

        case 'C': /* \C-X, \C-\M-X */
          if(p == e || *p++ != '-') {
              raise_error("control escape is too short");
          }
          // fall through
        case 'c': /* \cX, \c\M-X */
          return convert_ctrl(meta);

      default:
        raise_error("unexpected escape sequence");
      }

      // not reached
      return 0;
    }

    void convert_nonascii() {
      int maxlen = ONIGENC_MBC_MAXLEN(enc);
      int i = 0;
      int n = 0;

      do {
        buf[i++] = convert_byte();
        n = Encoding::precise_mbclen(buf, buf + i, enc);
      } while(p < e && i < maxlen && ONIGENC_MBCLEN_NEEDMORE_P(n));

      n = Encoding::precise_mbclen(buf, buf + i, enc);
      if(ONIGENC_MBCLEN_INVALID_P(n)) {
        raise_error("invalid multibyte escape");
      } else if(i > 1 || (*buf & 0x80)) {
        if(i == 1 && utf8) {
          raise_error("escaped non-ASCII character in UTF-8 Regexp");
        }
        ascii = false;

        append_bytes(buf, i);

      } else {
        int n = snprintf(reinterpret_cast<char*>(buf),
                         CONVERT_BUFSIZE, "\\x%02X", *buf);
        append_bytes(buf, n);
      }
    }

    int convert_octal() {
      int value = 0;

      for(int i = 0;
          p < e && i < 3 && *p >= '0' && *p <= '7';
          i++, p++) {
        value <<= 3;
        value |= *p - '0';
      }

      return value;
    }

    int convert_hex(int min, int max) {
      int value = 0;
      int i = 0;

      for(uint8_t d = *p; p < e && i < max; d = *++p, i++) {
        if(d >= '0' && d <= '9') {
          d -= '0';
        } else if(d >= 'A' && d <= 'F') {
          d -= 'A';
          d += 10;
        } else if(d >= 'a' && d <= 'f') {
          d -= 'a';
          d += 10;
        } else {
          break;
        }

        value <<= 4;
        value += d;
      }

      if(i < min) {
        raise_error("hexadecimal number is too short");
      }

      return value;
    }

    void raise_error(const char* reason) {
      std::ostringstream msg;
      msg << reason << ": byte: " << p - str->byte_address()
          << ": " << str->c_str(state);
      Exception::raise_regexp_error(state, msg.str().c_str());
    }

    String* finalize() {
      if(ba) {
        copy_bytes();
        str = String::from_bytearray(state, ba, bp - ba->raw_bytes());
      }

      return str;
    }
  };

  String* String::convert_escaped(STATE, Encoding*& enc, bool& fixed_encoding) {
    struct convert_escaped_state ces(state, this);

    if(!fixed_encoding && !enc) {
      enc = Encoding::ascii8bit_encoding(state);
    } else if(enc == Encoding::utf8_encoding(state)) {
      ces.utf8 = true;
    } else if(enc == Encoding::usascii_encoding(state)) {
      enc = Encoding::ascii8bit_encoding(state);
    }
    ces.enc = enc->encoding();

    while(ces.p < ces.e) {
      int n = Encoding::precise_mbclen(ces.p, ces.e, ces.enc);
      if(!ONIGENC_MBCLEN_CHARFOUND_P(n)) {
        ces.raise_error("invalid multibyte character");
      }

      n = ONIGENC_MBCLEN_CHARFOUND_LEN(n);
      if(n > 1 || (*ces.p & 0x80)) {
        if(n == 1 && ces.utf8) {
          ces.raise_error("non-ASCII character in UTF-8 Regexp");
        }

        ces.ascii = false;
        ces.p += n;
        continue;
      }

      if(*ces.p++ == '\\') {
        if(ces.p == ces.e) {
          ces.raise_error("escape sequence is too short");
        }

        switch(*ces.p++) {
        case '1': case '2': case '3':
        case '4': case '5': case '6':
        case '7': { // \O, \OO, \OOO or backref
          uint8_t* sp = ces.p--;

          if(ces.convert_octal() < 128) {
            continue;
          } else {
            ces.p = sp;
          }
          // fall through
        }

        case '0': // \0, \0O, \0OO
        case 'x': // \xHH
        case 'C': // \C-X, \C-\M-X
        case 'c': // \cX, \c\M-X
        case 'M': // \M-X, \M-\C-X, \M-\cX
          ces.p -= 2;
          ces.copy_bytes();
          ces.convert_nonascii();
          ces.update_index();
          break;

        case 'u':
          if(ces.p == ces.e) {
            ces.raise_error("escape sequence is too short");
          }

          ces.copy_bytes(-2);

          if(*ces.p == '{') {  // \u{H HH HHH HHHH HHHHH HHHHHH ...}
            ces.p++;

            int value = -1;
            while(ces.p < ces.e) {
              if(ISSPACE(*ces.p)) continue;

              value = ces.convert_hex(1, 6);

              if(*ces.p != '}' && !ISSPACE(*ces.p)) {
                ces.raise_error("invalid Unicode list");
              }

              ces.append_utf8(value);

              if(*ces.p == '}') break;
            }

            if(ces.p == ces.e || *ces.p++ != '}' || value < 0) {
              ces.raise_error("invalid Unicode list");
            }
          } else {  // \uHHHH
            ces.append_utf8(ces.convert_hex(4, 4));
          }

          ces.update_index();
          break;

        case 'p': /* \p{Hiragana} */
        case 'P':
          /* TODO
          if(!*encp) {
            *has_property = 1;
          }
          */
          break;

        default: // \n, \\, \d, \9, etc.
          break;
        }
      }
    }

    String* str = ces.finalize();

    if(!fixed_encoding) {
      if(ces.utf8) {
        if(CBOOL(str->ascii_only_p(state))) {
          enc = Encoding::usascii_encoding(state);
        } else {
          enc = Encoding::utf8_encoding(state);
          fixed_encoding = true;
        }
      } else if(ces.ascii || CBOOL(str->ascii_only_p(state))) {
        enc = Encoding::usascii_encoding(state);
      }
    }

    return str;
  }

  intptr_t String::char_size(STATE) {
    if(num_chars()->nil_p()) {
      OnigEncodingType* enc = encoding(state)->encoding();
      if(byte_compatible_p(encoding()) || CBOOL(ascii_only_p(state))) {
        num_chars(state, num_bytes());
      } else if(fixed_width_p(encoding())) {
        num_chars(state, Fixnum::from((byte_size() + ONIGENC_MBC_MINLEN(enc) - 1) / ONIGENC_MBC_MINLEN(enc)));
      } else {
        uint8_t* p = byte_address();
        uint8_t* e = p + byte_size();
        if(enc == ONIG_ENCODING_UTF_8 && CBOOL(valid_encoding_p(state))) {
          num_chars(state, Fixnum::from(Encoding::string_character_length_utf8(p, e)));
        } else {
          num_chars(state, Fixnum::from(Encoding::string_character_length(p, e, enc)));
        }
      }
    }

    return num_chars()->to_native();
  }

  Fixnum* String::size(STATE) {
    return Fixnum::from(char_size(state));
  }

  Encoding* String::encoding(STATE) {
    if(encoding()->nil_p()) {
      encoding(state, Encoding::ascii8bit_encoding(state));
    }
    return encoding();
  }

  hashval String::hash_string(STATE) {
    if(!hash_value()->nil_p()) {
      return (hashval)as<Fixnum>(hash_value())->to_native();
    }

    unsigned char* bp = (unsigned char*)(byte_address());

    hashval h = hash_str(state, bp, byte_size());
    hash_value(state, Fixnum::from(h));

    return h;
  }

  uint64_t String::siphash(const unsigned char *bp, unsigned int sz, uint32_t seed) {
    return siphash24(siphash_key, seed, bp, sz);
  }

  hashval String::hash_str(const unsigned char *bp, unsigned int sz, uint32_t seed) {
#ifdef USE_MURMUR3
#ifdef IS_64BIT_ARCH
    hashval hv[2];
    MurmurHash3_x64_128(bp, sz, seed, hv);
#else
    hashval hv[1];
    MurmurHash3_x86_32(bp, sz, seed, hv);
#endif
    return hv[0] & FIXNUM_MAX;
#else
    uint64_t v = siphash(bp, sz, seed);
    return ((hashval)v) & FIXNUM_MAX;
#endif
  }

  Symbol* String::to_sym(STATE) {
    return state->symbol(this);
  }

  const char* String::c_str(STATE) {
    char* c_string = (char*)byte_address();
    intptr_t current_size = byte_size();

    /*
     * Oh Oh... String's don't need to be \0 terminated..
     * The problem here is that that means we can't just
     * set \0 on the c_string[size()] element since that means
     * we might write outside the ByteArray storage allocated for this!
     *
     * Therefore we need to guard this case just in case so we don't
     * put the VM in a state with corrupted memory.
     */
    if(current_size >= as<ByteArray>(data())->size()) {
      ByteArray* ba =
        state->memory()->new_bytes<ByteArray>(state, G(bytearray), current_size + 1);
      memcpy(ba->raw_bytes(), byte_address(), current_size);
      data(state, ba);
      if(CBOOL(shared())) shared(state, cFalse);
      // We need to read it again since we have a new ByteArray
      c_string = (char*)byte_address();
      c_string[current_size] = 0;
    }

    if(c_string[current_size] != 0) {
      unshare(state);
      // Read it again because unshare might change it.
      c_string = (char*)byte_address();
      c_string[current_size] = 0;
    }

    return c_string;
  }

  const char* String::c_str_null_safe(STATE) {
    const char* str = c_str(state);
    if(byte_size() > (intptr_t) strnlen(str, byte_size())) {
      Exception::raise_argument_error(state, "string contains NULL byte");
    }
    return str;
  }

  String* String::check_null_safe(STATE) {
    c_str_null_safe(state);
    return this;
  }

  Object* String::secure_compare(STATE, String* other) {
    intptr_t s1 = num_bytes()->to_native();
    intptr_t s2 = other->num_bytes()->to_native();
    intptr_t d1 = as<ByteArray>(data())->size();
    intptr_t d2 = as<ByteArray>(other->data())->size();

    if(unlikely(s1 > d1)) {
      s1 = d1;
    }

    if(unlikely(s2 > d2)) {
      s2 = d2;
    }

    intptr_t max = (s2 > s1) ? s2 : s1;

    uint8_t* p1 = byte_address();
    uint8_t* p2 = other->byte_address();

    uint8_t* p1max = p1 + s1;
    uint8_t* p2max = p2 + s2;

    uint8_t sum = 0;

    for(intptr_t i = 0; i < max; i++) {
      uint8_t* c1 = p1 + i;
      uint8_t* c2 = p2 + i;

      uint8_t b1 = (c1 >= p1max) ? 0 : *c1;
      uint8_t b2 = (c2 >= p2max) ? 0 : *c2;

      sum |= (b1 ^ b2);
    }

    return RBOOL(sum == 0);
  }

  String* String::string_dup_slow(STATE) {
    Module* mod = klass();
    Class*  cls = try_as_instance<Class>(mod);

    while(!cls) {
      mod = mod->superclass();

      if(mod->nil_p()) rubinius::bug("Object::class_object() failed to find a class");

      cls = try_as_instance<Class>(mod);
    }

    String* so = state->memory()->new_object<String>(state, cls);

    so->copy_object(state, this);
    so->shared(cTrue);
    shared(cTrue);
    infect(state, so);

    return so;
  }

  void String::unshare(STATE) {
    if(CBOOL(shared())) {
      if(data()->reference_p()) {
        data(state, as<ByteArray>(data()->duplicate(state)));
      }
      shared(state, cFalse);
    }
  }

  String* String::append(STATE, String* other) {
    // Clamp the length of the other string to the maximum byte array size
    intptr_t length = other->byte_size();
    intptr_t data_length = as<ByteArray>(other->data())->size();

    if(!encoding()->equal_p(other->encoding())) {
      Encoding* new_encoding = Encoding::compatible_p(state, this, other);
      if(new_encoding->nil_p()) {
        Exception::raise_encoding_compatibility_error(state, other, this);
      }
      encoding(state, new_encoding);
    }

    if(CBOOL(ascii_only()) && !CBOOL(other->ascii_only_p(state))) {
      ascii_only(state, cFalse);
    }

    if(!CBOOL(other->valid_encoding_p(state))) {
      if(CBOOL(valid_encoding())) {
        valid_encoding(state, cFalse);
      } else {
        valid_encoding(state, cNil);
      }
    }

    if(unlikely(length > data_length)) {
      length = data_length;
    }
    return append(state,
                  reinterpret_cast<const char*>(other->byte_address()),
                  length);
  }

  String* String::append(STATE, const char* other) {
    return append(state, other, strlen(other));
  }

  String* String::byte_append(STATE, String* other) {
    intptr_t length = other->byte_size();
    intptr_t data_length = as<ByteArray>(other->data())->size();

    if(unlikely(length > data_length)) {
      length = data_length;
    }
    if(!other->ascii_only()->true_p()) {
      ascii_only(cNil);
    }
    if(!other->valid_encoding()->true_p()) {
      valid_encoding(cNil);
    }
    return append(state,
                  reinterpret_cast<const char*>(other->byte_address()),
                  length);
  }

  String* String::append(STATE, const char* other, intptr_t length) {
    intptr_t current_size = byte_size();
    intptr_t data_size = as<ByteArray>(data())->size();

    // Clamp the string size to the maximum underlying byte array size
    if(unlikely(current_size > data_size)) {
      current_size = data_size;
    }

    intptr_t new_size = current_size + length;
    intptr_t capacity = data_size;

    // Check for overflow, too big if that happens
    if(new_size < 0) {
      Exception::raise_argument_error(state, "string sizes too big");
    }

    if(capacity <= new_size) {
      // capacity needs one extra byte of room for the trailing null
      if(capacity == 0) capacity = 2;
      do {
        // @todo growth should be more intelligent than doubling
        capacity *= 2;
        // Check for overflow, use max capacity then
        if(capacity < 0) {
          capacity = LONG_MAX - 1;
        }
      } while(capacity < new_size + 1);

      // No need to call unshare and duplicate a ByteArray
      // just to throw it away.
      if(CBOOL(shared())) shared(state, cFalse);

      ByteArray* ba =
        state->memory()->new_bytes<ByteArray>(state, G(bytearray), capacity);
      memcpy(ba->raw_bytes(), byte_address(), current_size);
      memset(ba->raw_bytes() + new_size, 0, capacity - new_size);
      data(state, ba);
    } else {
      if(CBOOL(shared())) unshare(state);
    }

    // Append on top of the null byte at the end of s1, not after it
    memcpy(byte_address() + current_size, other, length);

    // The 0-based index of the last character is new_size - 1
    byte_address()[new_size] = 0;

    num_bytes(state, Fixnum::from(new_size));
    hash_value(state, nil<Fixnum>());

    return this;
  }

  String* String::resize_capacity(STATE, Fixnum* count) {
    intptr_t sz = count->to_native();

    if(sz < 0) {
      Exception::raise_argument_error(state, "negative byte array size");
    }

    ByteArray* ba =
      state->memory()->new_bytes<ByteArray>(state, G(bytearray), sz + 1);
    intptr_t copy_size = sz;
    intptr_t data_size = as<ByteArray>(data())->size();

    // Check that we don't copy any data outside the existing byte array
    if(unlikely(copy_size > data_size)) {
      copy_size = data_size;
    }
    memcpy(ba->raw_bytes(), byte_address(), copy_size);
    memset(ba->raw_bytes() + copy_size, 0, sz + 1 - copy_size);

    // We've unshared
    shared(state, cFalse);
    data(state, ba);
    hash_value(state, nil<Fixnum>());

    // If we shrunk it and num_bytes said there was more than there
    // is, clamp it.
    if(num_bytes()->to_native() > sz) {
      num_bytes(state, count);
    }

    return this;
  }

  String* String::add(STATE, String* other) {
    return string_dup(state)->append(state, other);
  }

  String* String::add(STATE, const char* other) {
    return string_dup(state)->append(state, other);
  }

  Float* String::to_f(STATE, Object* strict) {
    const char* str = reinterpret_cast<const char*>(byte_address());

    if(CBOOL(strict) && byte_size() > (intptr_t)strnlen(str, byte_size())) {
      return nil<Float>();
    }

    return Float::from_cstr(state, str, str + byte_size(), strict);
  }

  // Character-wise logical AND of two strings. Modifies the receiver.
  String* String::apply_and(STATE, String* other) {
    intptr_t count;
    if(num_bytes() > other->num_bytes()) {
      count = other->num_bytes()->to_native();
    } else {
      count = num_bytes()->to_native();
    }

    uint8_t* s = byte_address();
    uint8_t* o = other->byte_address();

    // Use && not & to keep 1's in the table.
    for(intptr_t i = 0; i < count; i++) {
      s[i] = s[i] && o[i];
    }

    return this;
  }

  Fixnum* String::tr_expand(STATE, Object* limit, Object* invalid_as_empty) {
    intptr_t lim = -1;
    if(Fixnum* l = try_as<Fixnum>(limit)) {
      lim = l->to_native();
    }

    uint8_t* str = byte_address();
    intptr_t bytes = byte_size();

    for(intptr_t i = 0; i < bytes;) {
      intptr_t chr = str[i];
      intptr_t seq = ++i < bytes ? str[i] : -1;

      if(chr == '\\' && seq >= 0) {
        continue;
      } else if(seq == '-') {
        intptr_t start = i - 1;
        intptr_t max = ++i < bytes ? str[i] : -1;
        intptr_t next = max >= 0 ? i + 1 : i;
        if(max >= 0 && chr > max) {
          std::ostringstream message;
          if(isprint(chr) && isprint(max)) {
            message << "invalid range \"";
            message << (char)chr << "-" << (char)max;
            message << "\" in string transliteration";
          } else {
            message << "invalid range in string transliteration";
          }
          Exception::raise_argument_error(state, message.str().c_str());
        } else if(max >= 0) {
          if(chr > max) {
            max = CBOOL(invalid_as_empty) ? chr - 1 : chr;
          }
          i = tr_replace(state, chr, max + 1, start, next);
          str = byte_address();
          bytes = byte_size();
        }
      }
    }

    if(lim > 0 && byte_size() > lim) {
      unshare(state);
      num_bytes(state, Fixnum::from(lim));
      byte_address()[byte_size()] = 0;
    }
    return num_bytes();
  }

  intptr_t String::tr_replace(STATE, intptr_t first, intptr_t last, intptr_t start, intptr_t next) {
    intptr_t replace_length = last - first;
    intptr_t added_chars = replace_length - next + start;
    intptr_t new_size = byte_size() + added_chars + 1;

    if(new_size > byte_size() || shared()->true_p()) {
      ByteArray* ba = ByteArray::create(state, new_size);
      memcpy(ba->raw_bytes(), byte_address(), byte_size());

      data(state, ba);
      shared(state, cFalse);
    }

    memmove(byte_address() + start + replace_length,
            byte_address() + next,
            byte_size() - next);

    uint8_t* address_start = byte_address() + start;
    while(first < last) {
      *address_start = first;
      ++address_start;
      ++first;
    }

    num_bytes(state, Fixnum::from(new_size - 1));
    return start + replace_length;
  }

  String* String::transform(STATE, Tuple* tbl) {
    uint8_t invalid[5];

    if(tbl->num_fields() < 256) {
      return force_as<String>(Primitives::failure());
    }

    Object** tbl_ptr = tbl->field;

    // Pointers to iterate input bytes.
    uint8_t* in_p = byte_address();

    intptr_t str_size = byte_size();
    intptr_t data_size = as<ByteArray>(data())->size();
    if(unlikely(str_size > data_size)) {
      str_size = data_size;
    }

    uint8_t* in_end = in_p + str_size;

    // Optimistic estimate that output size will be 1.25 x input.
    intptr_t out_chunk = str_size * 5 / 4;
    intptr_t out_size = out_chunk;
    uint8_t* output = (uint8_t*)malloc(out_size);

    if(!output) {
      Exception::raise_memory_error(state);
      return NULL;
    }

    uint8_t* out_p = output;
    uint8_t* out_end = out_p + out_size;

    while(in_p < in_end) {
      intptr_t len = 0;
      uint8_t byte = *in_p;
      uint8_t* cur_p = 0;

      if(String* str = try_as<String>(tbl_ptr[byte])) {
        cur_p = str->byte_address();
        len = str->byte_size();
        in_p++;
      } else {
        Tuple* tbl = as<Tuple>(tbl_ptr[byte]);

        for(intptr_t i = 0; i < tbl->num_fields(); i += 2) {
          String* key = as<String>(tbl->at(i));

          intptr_t rem = in_end - in_p;
          intptr_t klen = key->byte_size();
          if(rem < klen) continue;

          if(memcmp(in_p, key->byte_address(), klen) == 0) {
            String* str = as<String>(tbl->at(i+1));
            cur_p = str->byte_address();
            len = str->byte_size();
            in_p += klen;
            break;
          }
        }
      }

      // We could not map this byte, so we add it to the output
      // in stringified octal notation (ie \nnn).
      if(!cur_p) {
        snprintf((char*)invalid, 5, "\\%03o", *((char*)in_p) & 0377);
        in_p++;
        cur_p = invalid;
        len = 4;
      }

      if(out_p + len > out_end) {
        intptr_t pos = out_p - output;
        out_size += (len > out_chunk ? len : out_chunk);
        uint8_t* new_output = (uint8_t*)realloc(output, out_size);
        if(!new_output) {
          free(output);
          Exception::raise_memory_error(state);
          return NULL;
        }
        output = new_output;
        out_p = output + pos;
        out_end = output + out_size;
      }

      switch(len) {
      case 1:
        *out_p++ = *cur_p;
        break;
      case 2:
        *out_p++ = *cur_p++;
        *out_p++ = *cur_p;
        break;
      case 3:
        *out_p++ = *cur_p++;
        *out_p++ = *cur_p++;
        *out_p++ = *cur_p;
        break;
      default:
        memcpy(out_p, cur_p, len);
        out_p += len;
        break;
      }
    }

    String* result = String::create(state,
                                    reinterpret_cast<const char*>(output),
                                    out_p - output);

    result->klass(state, class_object(state));
    free(output);

    infect(state, result);
    result->encoding(state, encoding());

    return result;
  }

  String* String::copy_from(STATE, String* other, Fixnum* start,
                            Fixnum* size, Fixnum* dest)
  {
    intptr_t src = start->to_native();
    intptr_t dst = dest->to_native();
    intptr_t cnt = size->to_native();

    intptr_t osz = other->byte_size();
    if(src >= osz) return this;
    if(cnt < 0) return this;
    if(src < 0) src = 0;
    if(cnt > osz - src) cnt = osz - src;

    // This bounds checks on the total capacity rather than the virtual
    // size() of the String. This allows for string adjustment within
    // the capacity without having to change the virtual size first.
    intptr_t sz = as<ByteArray>(data())->size();
    if(dst >= sz) return this;
    if(dst < 0) dst = 0;
    if(cnt > sz - dst) cnt = sz - dst;

    memmove(byte_address() + dst, other->byte_address() + src, cnt);

    return this;
  }

  Fixnum* String::compare_substring(STATE, String* other, Fixnum* start,
                                    Fixnum* size)
  {
    intptr_t src = start->to_native();
    intptr_t cnt = size->to_native();
    intptr_t sz = byte_size();
    intptr_t osz = other->byte_size();
    intptr_t dsz = as<ByteArray>(data())->size();
    intptr_t odsz = as<ByteArray>(other->data())->size();

    if(unlikely(sz > dsz)) {
      sz = dsz;
    }

    if(unlikely(osz > odsz)) {
      osz = odsz;
    }

    if(src < 0) src = osz + src;

    if(src >= osz) {
      Exception::raise_object_bounds_exceeded_error(state, "start exceeds size of other");
    } else if(src < 0) {
      Exception::raise_object_bounds_exceeded_error(state, "start less than zero");
    }

    if(src + cnt > osz) cnt = osz - src;

    if(cnt > sz) cnt = sz;

    intptr_t cmp = memcmp(byte_address(), other->byte_address() + src, cnt);

    if(cmp < 0) {
      return Fixnum::from(-1);
    } else if(cmp > 0) {
      return Fixnum::from(1);
    } else {
      return Fixnum::from(0);
    }
  }

  String* String::pattern(STATE, Object* self, Fixnum* size, Object* pattern) {
    if(!size->positive_p()) {
      Exception::raise_argument_error(state, "size must be positive");
    }

    intptr_t cnt = size->to_native();

    String* s = state->memory()->new_object<String>(state, as<Class>(self));

    s->num_bytes(state, size);

    ByteArray* ba =
      state->memory()->new_bytes<ByteArray>(state, G(bytearray), cnt + 1);

    if(Fixnum* chr = try_as<Fixnum>(pattern)) {
      memset(ba->raw_bytes(), (int)chr->to_native(), cnt);
      s->valid_encoding(state, cTrue);
      s->encoding(state, Encoding::ascii8bit_encoding(state));
    } else if(String* pat = try_as<String>(pattern)) {
      pat->infect(state, s);

      intptr_t psz = pat->byte_size();
      uint8_t* raw = ba->raw_bytes();
      if(psz == 1) {
        memset(raw, pat->byte_address()[0], cnt);
      } else if(psz > 1) {
        intptr_t sz = cnt / psz;
        intptr_t len = sz * psz;
        if(len >= psz) {
          memcpy(raw, pat->byte_address(), psz);
          while(psz <= len / 2) {
            memcpy(raw + psz, raw, psz);
            psz *= 2;
          }
          memcpy(raw + psz, raw, len - psz);
        }
        for(intptr_t i = len, j = 0; i < cnt; i++, j++) {
          raw[i] = pat->byte_address()[j];
        }
      }
      s->ascii_only(state, pat->ascii_only_p(state));
      s->valid_encoding(state, pat->valid_encoding_p(state));
      s->encoding_from(state, pat);
    } else {
      Exception::raise_argument_error(state, "pattern must be a Fixnum or String");
    }

    s->data(state, ba);
    return s;
  }

  String* String::from_codepoint(STATE, Object* self, Integer* code, Encoding* enc) {
    String* s = state->memory()->new_object<String>(state, G(string));

    unsigned int c = code->to_uint();
    int n = ONIGENC_CODE_TO_MBCLEN(enc->encoding(), c);

    if(n <= 0) invalid_codepoint_error(state, c);

    s->num_bytes(state, Fixnum::from(n));
    s->encoding(state, enc);

    ByteArray* ba =
      state->memory()->new_bytes<ByteArray>(state, G(bytearray), n + 1);

    n = ONIGENC_CODE_TO_MBC(enc->encoding(), c, (UChar*)ba->raw_bytes());
    if(Encoding::precise_mbclen(ba->raw_bytes(), ba->raw_bytes() + n, enc->encoding()) != n) {
      invalid_codepoint_error(state, c);
    }

    s->data(state, ba);

    return s;
  }


  static locks::spinlock_mutex crypt_lock;

  String* String::crypt(STATE, String* salt) {
    std::lock_guard<locks::spinlock_mutex> guard(crypt_lock);

    char* result = ::crypt(this->c_str(state), salt->c_str(state));

    if(!result) {
      Exception::raise_errno_error(state, "crypt(3) failed");
      return NULL;
    }

    String* res = String::create(state, result);

    return res;
  }

  Integer* String::to_i(STATE, Fixnum* fix_base, Object* strict) {
    const char* str = reinterpret_cast<const char*>(byte_address());
    int base = fix_base->to_native();

    if(CBOOL(strict)) {
      // In strict mode the string can't have null bytes.
      if(byte_size() > (intptr_t)strnlen(str, byte_size())) return nil<Integer>();
    }

    return Integer::from_cstr(state, str, str + byte_size(), base, strict);
  }

  Integer* String::to_inum_prim(STATE, Fixnum* base, Object* strict) {
    Integer* val = to_i(state, base, strict);
    if(val->nil_p()) return (Integer*)Primitives::failure();

    return val;
  }

  Object* String::aref(STATE, Fixnum* index) {
    intptr_t i = index->to_native();

    if(i < 0) i += byte_size();
    if(i >= byte_size() || i < 0) return cNil;

    intptr_t len = char_size(state);
    if(byte_compatible_p(encoding()) || CBOOL(ascii_only_p(state))) {
      return byte_substring(state, i, 1);
    } else {
      // Assumptions above about size are possibly invalid, recalculate.
      i = index->to_native();
      if(i < 0) i += len;
      if(i >= len || i < 0) return cNil;

      return char_substring(state, i, 1);
    }
  }

  /* Returns the byte index of the character at logical 'index'. The 'start'
   * parameter is the byte index of a character at which to start searching.
   */
  intptr_t String::find_character_byte_index(STATE, intptr_t index,
                                               intptr_t start) {
    OnigEncodingType* enc = encoding(state)->encoding();
    if(byte_compatible_p(encoding()) || CBOOL(ascii_only_p(state))) {
      return start + index;
    } else if(fixed_width_p(encoding())) {
      return start + index * ONIGENC_MBC_MINLEN(enc);
    } else if(enc == ONIG_ENCODING_UTF_8 && CBOOL(valid_encoding_p(state))) {
      intptr_t offset = Encoding::find_character_byte_index_utf8(byte_address() + start,
                                                 byte_address() + byte_size(),
                                                 index);
      return start + offset;
    } else {
      intptr_t offset = Encoding::find_character_byte_index(byte_address() + start,
                                                 byte_address() + byte_size(),
                                                 index,
                                                 enc);
      return start + offset;
    }
  }

  /* Returns the byte index of the character at char index 'index'. The 'start'
   * parameter is the byte index of a character at which to start searching.
   * Returns the byte index of the first character starting at or after the
   * given index.
   */
  Fixnum* String::find_character_byte_index_prim(STATE, Fixnum* index,
                                                        Fixnum* start) {
    return Fixnum::from(this->find_character_byte_index(state, index->to_native(), start->to_native()));
  }

  /* Returns the char index of the character at byte index 'index'. The 'start'
   * parameter is the byte index of a character at which to start searching.
   * Returns the char index of the first character starting at or after the
   * given index.
   */
  intptr_t String::find_byte_character_index(STATE, intptr_t index,
                                               intptr_t start) {
    OnigEncodingType* enc = encoding(state)->encoding();
    if(byte_compatible_p(encoding()) || CBOOL(ascii_only_p(state))) {
      return index;
    } else if(fixed_width_p(encoding())) {
      return index / ONIGENC_MBC_MINLEN(enc);
    } else if(enc == ONIG_ENCODING_UTF_8 && CBOOL(valid_encoding_p(state))) {
      return Encoding::find_byte_character_index_utf8(byte_address() + start,
                                                 byte_address() + byte_size(),
                                                 index);
    } else {
      return Encoding::find_byte_character_index(byte_address() + start,
                                                 byte_address() + byte_size(),
                                                 index,
                                                 enc);
    }
  }

  /* Returns the char index of the character at byte index 'index'. The 'start'
   * parameter is the byte index of a character at which to start searching.
   * Returns the char index of the first character starting at or after the
   * given index.
   */
  Fixnum* String::find_byte_character_index_prim(STATE, Fixnum* index,
                                                    Fixnum* start) {
    return Fixnum::from(this->find_byte_character_index(state, index->to_native(), start->to_native()));
  }

  /* The 'index' and 'length' parameters are byte based. This method is a
   * worker method called from other methods that have already computed the
   * canonical byte (0 <= index < byte_size) and (0 <= length < byte_size).
   */
  String* String::byte_substring(STATE, intptr_t index, intptr_t length) {
    intptr_t data_size = as<ByteArray>(data())->size();

    // Clamp the range to the underlying byte array size
    if(unlikely(index > data_size)) index = data_size;

    if(unlikely(index + length > data_size)) {
      length = data_size - index;
    }

    const char* buf = (const char*)byte_address() + index;
    String* sub = String::create(state, buf, length);
    sub->klass(state, class_object(state));

    infect(state, sub);
    sub->encoding_from(state, this);

    return sub;
  }

  /* The 'index' and 'length' parameters are character based. This method is a
   * worker method called from other methods that have already computed the
   * canonical character (0 <= index < char_size) and (0 <= length < char_size).
   */
  String* String::char_substring(STATE, intptr_t index, intptr_t length) {
    intptr_t i = find_character_byte_index(state, index);
    intptr_t e = find_character_byte_index(state, length - 1, i);

    int c = Encoding::precise_mbclen(byte_address() + e, byte_address() + byte_size(),
                                     encoding(state)->encoding());

    if(ONIGENC_MBCLEN_CHARFOUND_P(c)) {
      e += ONIGENC_MBCLEN_CHARFOUND_LEN(c);
    } else {
      e += 1;
    }

    return byte_substring(state, i, e - i);
  }

  String* String::byte_substring(STATE, Fixnum* index, Fixnum* length) {
    intptr_t i = index->to_native();
    intptr_t n = length->to_native();
    intptr_t size = byte_size();

    if(n < 0) return nil<String>();

    if(i < 0) {
      i += size;
      if(i < 0) return nil<String>();
    } else if(i > size) {
      return nil<String>();
    }

    if(i + n > size) {
      n = size - i;
    }

    return byte_substring(state, i, n);
  }

  String* String::substring(STATE, Fixnum* index, Fixnum* length) {
    intptr_t i = index->to_native();
    intptr_t n = length->to_native();
    intptr_t size = char_size(state);

    if(n < 0) return nil<String>();

    if(i < 0) {
      i += size;
      if(i < 0) return nil<String>();
    } else if(i > size) {
      return nil<String>();
    }

    if(i + n > size) {
      n = size - i;
    }

    if(n == 0 || byte_compatible_p(encoding()) || CBOOL(ascii_only_p(state))) {
      return byte_substring(state, i, n);
    } else {
      return char_substring(state, i, n);
    }
  }

  Fixnum* String::index(STATE, String* pattern, Fixnum* start) {
    intptr_t total = byte_size();
    intptr_t match_size = pattern->byte_size();

    if(start->to_native() < 0) {
      Exception::raise_argument_error(state, "negative start given");
    }

    switch(match_size) {
    case 0:
      return start;
    case 1:
      {
        uint8_t* buf = byte_address();
        uint8_t matcher = pattern->byte_address()[0];

        for(intptr_t pos = start->to_native(); pos < total; pos++) {
          if(buf[pos] == matcher) return Fixnum::from(pos);
        }
      }
      return nil<Fixnum>();
    default:
      {
        uint8_t* buf = byte_address();
        uint8_t* matcher = pattern->byte_address();

        uint8_t* last = buf + (total - match_size);
        uint8_t* pos = buf + start->to_native();

        while(pos <= last) {
          // Checking *pos directly then also checking memcmp is an
          // optimization. It's about 10x faster than just calling memcmp
          // everytime.
          if(*pos == *matcher &&
              memcmp(pos, matcher, match_size) == 0) {
            return Fixnum::from(pos - buf);
          }
          pos++;
        }
      }
      return nil<Fixnum>();
    }
  }

  Fixnum* String::character_index(STATE, String* pattern, Fixnum* start) {
    intptr_t offset = start->to_native();
    if(offset < 0) return nil<Fixnum>();

    intptr_t total = byte_size();

    uint8_t* p = byte_address();
    uint8_t* e = byte_address() + total;
    uint8_t* pp = pattern->byte_address();
    uint8_t* pe = pp + pattern->byte_size();
    uint8_t* s;
    uint8_t* ss;

    if(byte_compatible_p(encoding()) || CBOOL(ascii_only_p(state))) {
      for(s = p += offset, ss = pp; p < e; s = ++p) {
        if(*p != *pp) continue;

        while(p < e && pp < pe && *(++p) == *(++pp))
          ; // memcmp

        if(pp < pe) {
          p = s;
          pp = ss;
        } else {
          return Fixnum::from(s - byte_address());
        }
      }

      return nil<Fixnum>();
    }

    OnigEncodingType* enc = encoding(state)->encoding();
    intptr_t index = 0;
    int c;

    while(p < e && index < offset) {
      c = Encoding::precise_mbclen(p, e, enc);

      if(ONIGENC_MBCLEN_CHARFOUND_P(c)) {
        p += c;
        index++;
      } else {
        return nil<Fixnum>();
      }
    }

    for(s = p, ss = pp; p < e; s = p += c, ++index) {
      c = Encoding::precise_mbclen(p, e, enc);
      if(!ONIGENC_MBCLEN_CHARFOUND_P(c)) return nil<Fixnum>();

      if(*p != *pp) continue;

      while(p < e && pp < pe) {
        for(uint8_t* pc = p + c; p < e && p < pc && pp < pe; ) {
          if(*(++p) != *(++pp)) goto next_search;
        }

        c = Encoding::precise_mbclen(p, e, enc);
        if(!ONIGENC_MBCLEN_CHARFOUND_P(c)) break;
      }

    next_search:

      if(pp < pe) {
        p = s;
        pp = ss;
      } else {
        return Fixnum::from(index);
      }
    }

    return nil<Fixnum>();
  }

  Fixnum* String::rindex(STATE, String* pattern, Fixnum* start) {
    intptr_t total = byte_size();
    intptr_t match_size = pattern->byte_size();
    intptr_t pos = start->to_native();

    if(pos < 0) {
      Exception::raise_argument_error(state, "negative start given");
    }

    if(pos >= total) pos = total - 1;

    switch(match_size) {
    case 0:
      return start;
    case 1:
      {
        uint8_t* buf = byte_address();
        uint8_t matcher = pattern->byte_address()[0];

        while(pos >= 0) {
          if(buf[pos] == matcher) return Fixnum::from(pos);
          pos--;
        }
      }
      return nil<Fixnum>();
    default:
      {
        uint8_t* buf = byte_address();
        uint8_t* matcher = pattern->byte_address();

        if(total - pos < match_size) {
          pos = total - match_size;
        }

        uint8_t* right = buf + pos;
        uint8_t* cur = right;

        while(cur >= buf) {
          if(memcmp(cur, matcher, match_size) == 0) {
            return Fixnum::from(cur - buf);
          }
          cur--;
        }
      }
      return nil<Fixnum>();
    }
  }

  Fixnum* String::byte_index(STATE, Object* value, Fixnum* start) {
    intptr_t total = byte_size();
    intptr_t offset = start->to_native();

    if(String* pattern = try_as<String>(value)) {
      intptr_t match_size = pattern->byte_size();

      if(offset < 0) {
        Exception::raise_argument_error(state, "negative start given");
      }

      if(match_size == 0) return start;

      if(!CBOOL(pattern->valid_encoding_p(state))) return nil<Fixnum>();

      Encoding* encoding = Encoding::compatible_p(state, this, pattern);
      if(encoding->nil_p()) {
        Exception::raise_argument_error(state, "encodings are incompatible");
      }

      OnigEncodingType* enc = encoding->encoding();
      uint8_t* p = byte_address() + offset;
      uint8_t* e = byte_address() + total;
      uint8_t* pp = pattern->byte_address();
      uint8_t* pe = pp + pattern->byte_size();
      uint8_t* s;
      uint8_t* ss;

      for(s = p, ss = pp; p < e; s = ++p) {
        if(*p != *pp) continue;

        while(p < e && pp < pe && *(++p) == *(++pp))
          ; // memcmp

        if(pp < pe) {
          p = s;
          pp = ss;
        } else {
          int c = Encoding::precise_mbclen(s, e, enc);

          if(ONIGENC_MBCLEN_CHARFOUND_P(c)) {
            return Fixnum::from(s - byte_address());
          } else {
            return nil<Fixnum>();
          }
        }
      }

      return nil<Fixnum>();
    } else if(Fixnum* index = try_as<Fixnum>(value)) {
      intptr_t k = index->to_native();

      if(k < 0) {
        Exception::raise_argument_error(state, "character index is negative");
      }

      if(byte_compatible_p(encoding()) || CBOOL(ascii_only_p(state))) {
        if(k > total) {
          return nil<Fixnum>();
        } else {
          return index;
        }
      } else {
        OnigEncodingType* enc = encoding(state)->encoding();
        uint8_t* p = byte_address();
        uint8_t* e = p + total;
        intptr_t i;

        for(i = 0; i < k && p < e; i++) {
          int c = Encoding::precise_mbclen(p, e, enc);

          // If it's an invalid byte, just treat it as a single byte
          if(!ONIGENC_MBCLEN_CHARFOUND_P(c)) {
            ++p;
          } else {
            p += ONIGENC_MBCLEN_CHARFOUND_LEN(c);
          }
        }

        if(i < k) {
          return nil<Fixnum>();
        } else {
          return Fixnum::from(p - byte_address());
        }
      }
    }

    Exception::raise_argument_error(state, "argument is not a String or Fixnum");
    return nil<Fixnum>(); // satisfy compiler
  }

  Fixnum* String::previous_byte_index(STATE, Fixnum* index) {
    intptr_t i = index->to_native();

    if(i < 0) {
      Exception::raise_argument_error(state, "negative index given");
    }

    OnigEncodingType* enc = encoding(state)->encoding();

    uint8_t* s = byte_address();
    uint8_t* p = s + i;
    uint8_t* e = s + byte_size();

    uint8_t* b = onigenc_get_prev_char_head(enc, s, p, e);

    if(!b) return nil<Fixnum>();

    return Fixnum::from(b - s);
  }

  String* String::find_character(STATE, Fixnum* offset) {
    intptr_t o = offset->to_native();
    if(o >= byte_size()) return nil<String>();
    if(o < 0) return nil<String>();

    uint8_t* cur = byte_address() + o;

    String* output = 0;

    OnigEncodingType* enc = encoding(state)->encoding();

    if(ONIGENC_MBC_MAXLEN(enc) == 1) {
      output = String::create(state, reinterpret_cast<const char*>(cur), 1);
    } else {
      int clen = Encoding::precise_mbclen(cur, cur + ONIGENC_MBC_MAXLEN(enc), enc);
      if(ONIGENC_MBCLEN_CHARFOUND_P(clen)) {
        output = String::create(state, reinterpret_cast<const char*>(cur), clen);
      } else {
        output = String::create(state, reinterpret_cast<const char*>(cur), 1);
      }
    }

    output->klass(state, class_object(state));
    infect(state, output);
    output->encoding_from(state, this);

    return output;
  }

  Array* String::awk_split(STATE, Fixnum* f_limit) {
    intptr_t limit = f_limit->to_native();

    intptr_t sz = byte_size();
    uint8_t* start = byte_address();
    int end = 0;
    int begin = 0;

    intptr_t i = 0;
    if(limit > 0) i = 1;

    bool skip = true;

    Array* ary = Array::create(state, 3);

    Class* out_class = class_object(state);

    // Algorithm ported from MRI

    for(uint8_t* ptr = start; ptr < start+sz; ptr++) {
      if(skip) {
        if(ISSPACE(*ptr)) {
          begin++;
        } else {
          end = begin + 1;
          skip = false;
          if(limit > 0 && limit <= i) break;
        }
      } else {
        if(ISSPACE(*ptr)) {
          String* str = String::create(state, (const char*)start+begin, end-begin);
          str->klass(state, out_class);
          infect(state, str);
          str->encoding_from(state, this);

          ary->append(state, str);
          skip = true;
          begin = end + 1;
          if(limit > 0) i++;
        } else {
          end++;
        }
      }
    }

    int fin_sz = sz-begin;

    if(fin_sz > 0 || (limit > 0 && i <= limit) || limit < 0) {
      String* str = String::create(state, (const char*)start+begin, fin_sz);
      str->klass(state, out_class);
      infect(state, str);
      str->encoding_from(state, this);

      ary->append(state, str);
    }
    return ary;
  }

  Object* String::ascii_only_p(STATE) {
    if(ascii_only()->nil_p()) {
      if(byte_size() == 0) {
        ascii_only(state, encoding(state)->ascii_compatible_p(state));
      } else {
        if(!CBOOL(encoding(state)->ascii_compatible_p(state))) {
          ascii_only(state, cFalse);
        } else if(Encoding::find_non_ascii_index(byte_address(), byte_address() + byte_size()) >= 0) {
          ascii_only(state, cFalse);
        } else {
          ascii_only(state, cTrue);
        }
      }
    }

    return ascii_only();
  }

  Object* String::valid_encoding_p(STATE) {
    if(valid_encoding()->nil_p()) {
      if(encoding(state) == Encoding::ascii8bit_encoding(state) || CBOOL(ascii_only_p(state))) {
        valid_encoding(state, cTrue);
        return valid_encoding();
      }

      OnigEncodingType* enc = encoding(state)->encoding();

      uint8_t* p = byte_address();
      uint8_t* e = p + byte_size();

      while(p < e) {
        int n = Encoding::precise_mbclen(p, e, enc);

        if(!ONIGENC_MBCLEN_CHARFOUND_P(n)) {
          valid_encoding(state, cFalse);
          return valid_encoding();
        }

        p += n;
      }

      valid_encoding(state, cTrue);
    }

    return valid_encoding();
  }

  int String::codepoint(STATE, bool* found) {
    OnigEncodingType* enc = encoding(state)->encoding();
    uint8_t* p = byte_address();
    uint8_t* e = p + byte_size();

    int n = Encoding::precise_mbclen(p, e, enc);

    if(ONIGENC_MBCLEN_CHARFOUND_P(n)) {
      *found = true;
      return ONIGENC_MBC_TO_CODE(enc, (UChar*)p, (UChar*)e);
    } else {
      *found = false;
      return 0;
    }
  }

  Fixnum* String::codepoint(STATE) {
    bool found;
    int c = codepoint(state, &found);

    if(!found) {
      return force_as<Fixnum>(Primitives::failure());
    } else {
      return Fixnum::from(c);
    }
  }

  Object* String::chr_at(STATE, Fixnum* byte) {
    return Character::create_from(state, this, byte);
  }

  String* String::reverse(STATE) {
    check_frozen(state);
    if(byte_size() <= 1) return this;

    unshare(state);
    hash_value(state, nil<Fixnum>());

    if(byte_compatible_p(encoding()) || CBOOL(ascii_only_p(state))) {
      data()->reverse(state, Fixnum::from(0), num_bytes());
    } else {
      Encoding::string_reverse(byte_address(),
          byte_address() + byte_size(), encoding()->encoding());
    }
    return this;
  }


  void String::Info::show(STATE, Object* self, int level) {
    String* str = as<String>(self);
    std::cout << "\"" << str->c_str(state) << "\"" << std::endl;
  }

  void String::Info::show_simple(STATE, Object* self, int level) {
    show(state, self, level);
  }
}

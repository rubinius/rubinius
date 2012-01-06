#include "oniguruma.h" // Must be first.
#include "regenc.h"

#include "builtin/string.hpp"
#include "builtin/array.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/integer.hpp"
#include "builtin/regexp.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"

#include "configuration.hpp"
#include "vm.hpp"
#include "object_utils.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"

#include "windows_compat.h"

#include "ontology.hpp"

#include <gdtoa.h>

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <ctype.h>
#include <stdint.h>

#include <sstream>

namespace rubinius {

  void String::init(STATE) {
    GO(string).set(ontology::new_class(state, "String", G(object)));
    G(string)->set_object_type(state, StringType);
  }

  /* Creates a String instance with +num_bytes+ == +size+ and
   * having a ByteArray with at least (size + 1) bytes.
   */
  String* String::create(STATE, Fixnum* size) {
    String *so;

    so = state->new_object<String>(G(string));

    so->num_bytes(state, size);
    so->hash_value(state, nil<Fixnum>());
    so->shared(state, Qfalse);

    native_int bytes = size->to_native() + 1;
    ByteArray* ba = ByteArray::create(state, bytes);
    ba->raw_bytes()[bytes-1] = 0;

    so->data(state, ba);

    return so;
  }

  String* String::create_reserved(STATE, native_int bytes) {
    String *so;

    so = state->new_object<String>(G(string));

    so->num_bytes(state, Fixnum::from(0));
    so->hash_value(state, nil<Fixnum>());
    so->shared(state, Qfalse);

    ByteArray* ba = ByteArray::create(state, bytes+1);
    ba->raw_bytes()[bytes] = 0;

    so->data(state, ba);

    return so;
  }

  /*
   * Creates a String instance with +num_bytes+ bytes of storage.
   * It also pins the ByteArray used for storage, so it can be passed
   * to an external function (like ::read)
   */
  String* String::create_pinned(STATE, Fixnum* size) {
    String *so;

    so = state->new_object<String>(G(string));

    so->num_bytes(state, size);
    so->hash_value(state, nil<Fixnum>());
    so->shared(state, Qfalse);

    native_int bytes = size->to_native() + 1;
    ByteArray* ba = ByteArray::create_pinned(state, bytes);
    ba->raw_bytes()[bytes-1] = 0;

    so->data(state, ba);

    return so;
  }

  /* +bytes+ should NOT attempt to take the trailing null into account
   * +bytes+ is the number of 'real' characters in the string
   */
  String* String::create(STATE, const char* str) {
    if(!str) return String::create(state, Fixnum::from(0));

    native_int bytes = strlen(str);

    return String::create(state, str, bytes);
  }

  /* +bytes+ should NOT attempt to take the trailing null into account
   * +bytes+ is the number of 'real' characters in the string
   */
  String* String::create(STATE, const char* str, native_int bytes) {
    String* so = String::create(state, Fixnum::from(bytes));

    if(str) memcpy(so->byte_address(), str, bytes);

    return so;
  }

  String* String::from_bytearray(STATE, ByteArray* ba, Fixnum* start,
                                 Fixnum* count)
  {
    String* s = state->new_object<String>(G(string));

    s->num_bytes(state, count);
    s->hash_value(state, nil<Fixnum>());
    s->shared(state, Qfalse);

    // fetch_bytes NULL terminates
    s->data(state, ba->fetch_bytes(state, start, count));

    return s;
  }

  static bool byte_compatible_p(Encoding* enc) {
    return enc->nil_p() || ONIGENC_MBC_MAXLEN(enc->get_encoding()) == 1;
  }

  static bool fixed_width_p(Encoding* enc) {
    if(enc->nil_p()) return true;

    OnigEncodingType* e = enc->get_encoding();
    return ONIGENC_MBC_MAXLEN(e) == ONIGENC_MBC_MINLEN(e);
  }

  static const uint8_t* find_non_ascii(const uint8_t* data, int num) {
    for(int i = 0; i < num; i++) {
      if(!ISASCII(data[i])) {
        return data + i;
      }
    }
    return 0;
  }

  static int mbclen(const uint8_t* p, const uint8_t* e, OnigEncodingType* enc)
  {
    int n = ONIGENC_PRECISE_MBC_ENC_LEN(enc, (UChar*)p, (UChar*)e);

    if (ONIGENC_MBCLEN_CHARFOUND_P(n) && ONIGENC_MBCLEN_CHARFOUND_LEN(n) <= e-p) {
      return ONIGENC_MBCLEN_CHARFOUND_LEN(n);
    } else {
      int min = ONIGENC_MBC_MINLEN(enc);
      return min <= e-p ? min : (int)(e-p);
    }
  }

  static int precise_mbclen(const uint8_t* p, const uint8_t* e, OnigEncodingType* enc) {
    int n;

    if (e <= p) {
      return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(1);
    }

    n = ONIGENC_PRECISE_MBC_ENC_LEN(enc, (UChar*)p, (UChar*)e);
    if (e-p < n) {
      return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(n-(int)(e-p));
    }

    return n;
  }

  native_int String::char_size(STATE) {
    if(num_chars_->nil_p()) {
      if(LANGUAGE_18_ENABLED(state)) {
        num_chars_ = num_bytes_;
      } else {
        if(byte_compatible_p(encoding_)) {
          num_chars_ = num_bytes_;
        } else {
          OnigEncodingType* enc = encoding_->get_encoding();
          native_int chars;

          if(fixed_width_p(encoding_)) {
            chars = (byte_size() + ONIGENC_MBC_MINLEN(enc) - 1) / ONIGENC_MBC_MINLEN(enc);
          } else {
            uint8_t* p = byte_address();
            uint8_t* e = p + byte_size();

            for(chars = 0; p < e; chars++) {
              int n = precise_mbclen(p, e, enc);

              if(ONIGENC_MBCLEN_CHARFOUND_P(n)) {
                p += ONIGENC_MBCLEN_CHARFOUND_LEN(n);
              } else if(p + ONIGENC_MBC_MINLEN(enc) <= e) {
                p += ONIGENC_MBC_MINLEN(enc);
              } else {
                p = e;
              }
            }
          }

          num_chars_ = Fixnum::from(chars);
        }
      }
    }

    return num_chars_->to_native();
  }

  Fixnum* String::size(STATE) {
    return Fixnum::from(char_size(state));
  }

  Encoding* String::encoding(STATE) {
    if(encoding_->nil_p()) {
      encoding(state, Encoding::usascii_encoding(state));
    }
    return encoding_;
  }

  hashval String::hash_string(STATE) {
    if(!hash_value_->nil_p()) {
      return (hashval)as<Fixnum>(hash_value_)->to_native();
    }

    unsigned char* bp = (unsigned char*)(byte_address());

    hashval h = hash_str(bp, byte_size());
    hash_value(state, Fixnum::from(h));

    return h;
  }

  // see http://isthe.com/chongo/tech/comp/fnv/#FNV-param
#ifdef _LP64
  const static unsigned long FNVOffsetBasis = 14695981039346656037UL;
  const static unsigned long FNVHashPrime = 1099511628211UL;
#else
  const static unsigned long FNVOffsetBasis = 2166136261UL;
  const static unsigned long FNVHashPrime = 16777619UL;
#endif

  static inline unsigned long update_hash(unsigned long hv,
                                          unsigned char byte)
  {
    return (hv ^ byte) * FNVHashPrime;
  }

  static inline unsigned long finish_hash(unsigned long hv) {
    return (hv>>FIXNUM_WIDTH) ^ (hv & FIXNUM_MAX);
  }

  hashval String::hash_str(const char *bp) {
    hashval hv;

    hv = FNVOffsetBasis;

    while(*bp) {
      hv = update_hash(hv, *bp++);
    }

    return finish_hash(hv);
  }

  hashval String::hash_str(const unsigned char *bp, unsigned int sz) {
    unsigned char* be = (unsigned char*)bp + sz;
    hashval hv = FNVOffsetBasis;

    while(bp < be) {
      hv = update_hash(hv, *bp++);
    }

    return finish_hash(hv);
  }

  Symbol* String::to_sym(STATE) {
    return state->symbol(this);
  }

  const char* String::c_str(STATE) {
    char* c_string = (char*)byte_address();
    native_int current_size = byte_size();

    /*
     * Oh Oh... String's don't need to be \0 terminated..
     * The problem here is that that means we can't just
     * set \0 on the c_string[size()] element since that means
     * we might write outside the ByteArray storage allocated for this!
     *
     * Therefore we need to guard this case just in case so we don't
     * put the VM in a state with corrupted memory.
     */
    if(current_size >= as<ByteArray>(data_)->size()) {
      ByteArray* ba = ByteArray::create(state, current_size + 1);
      memcpy(ba->raw_bytes(), byte_address(), current_size);
      data(state, ba);
      if(shared_ == Qtrue) shared(state, Qfalse);
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

  Object* String::secure_compare(STATE, String* other) {
    native_int s1 = num_bytes()->to_native();
    native_int s2 = other->num_bytes()->to_native();
    native_int d1 = as<ByteArray>(data_)->size();
    native_int d2 = as<ByteArray>(other->data_)->size();

    if(unlikely(s1 > d1)) {
      s1 = d1;
    }

    if(unlikely(s2 > d2)) {
      s2 = d2;
    }

    native_int max = (s2 > s1) ? s2 : s1;

    uint8_t* p1 = byte_address();
    uint8_t* p2 = other->byte_address();

    uint8_t* p1max = p1 + s1;
    uint8_t* p2max = p2 + s2;

    uint8_t sum = 0;

    for(native_int i = 0; i < max; i++) {
      uint8_t* c1 = p1 + i;
      uint8_t* c2 = p2 + i;

      uint8_t b1 = (c1 >= p1max) ? 0 : *c1;
      uint8_t b2 = (c2 >= p2max) ? 0 : *c2;

      sum |= (b1 ^ b2);
    }

    return (sum == 0) ? Qtrue : Qfalse;
  }

  String* String::string_dup(STATE) {
    Module* mod = klass_;
    Class*  cls = try_as_instance<Class>(mod);

    if(unlikely(!cls)) {
      while(!cls) {
        mod = mod->superclass();

        if(mod->nil_p()) rubinius::bug("Object::class_object() failed to find a class");

        cls = try_as_instance<Class>(mod);
      }
    }

    String* so = state->new_object<String>(cls);

    so->set_tainted(is_tainted_p());

    so->num_bytes(state, num_bytes());
    so->data(state, data());
    so->hash_value(state, hash_value());

    so->encoding(state, encoding());
    so->valid_encoding(state, valid_encoding());
    so->ascii_only(state, ascii_only());

    so->shared(state, Qtrue);
    shared(state, Qtrue);

    return so;
  }

  void String::unshare(STATE) {
    if(shared_ == Qtrue) {
      if(data_->reference_p()) {
        data(state, as<ByteArray>(data_->duplicate(state)));
      }
      shared(state, Qfalse);
    }
  }

  String* String::append(STATE, String* other) {
    // Clamp the length of the other string to the maximum byte array size
    native_int length = other->byte_size();
    native_int data_length = as<ByteArray>(other->data_)->size();
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

  String* String::append(STATE, const char* other, native_int length) {
    native_int current_size = byte_size();
    native_int data_size = as<ByteArray>(data_)->size();

    // Clamp the string size the maximum underlying byte array size
    if(unlikely(current_size > data_size)) {
      current_size = data_size;
    }

    native_int new_size = current_size + length;
    native_int capacity = data_size;

    if(capacity < new_size + 1) {
      // capacity needs one extra byte of room for the trailing null
      do {
        // @todo growth should be more intelligent than doubling
        capacity *= 2;
      } while(capacity < new_size + 1);

      // No need to call unshare and duplicate a ByteArray
      // just to throw it away.
      if(shared_ == Qtrue) shared(state, Qfalse);

      ByteArray* ba = ByteArray::create(state, capacity);
      memcpy(ba->raw_bytes(), byte_address(), current_size);
      data(state, ba);
    } else {
      if(shared_ == Qtrue) unshare(state);
    }

    // Append on top of the null byte at the end of s1, not after it
    memcpy(byte_address() + current_size, other, length);

    // The 0-based index of the last character is new_size - 1
    byte_address()[new_size] = 0;

    num_bytes(state, Fixnum::from(new_size));
    num_chars(state, nil<Fixnum>());
    hash_value(state, nil<Fixnum>());

    return this;
  }

  String* String::resize_capacity(STATE, Fixnum* count) {
    native_int sz = count->to_native();

    if(sz < 0) {
      Exception::argument_error(state, "negative byte array size");
    } else if(sz >= INT32_MAX) {
      // >= is used deliberately because we use a size of + 1
      // for the byte array
      Exception::argument_error(state, "too large byte array size");
    }

    ByteArray* ba = ByteArray::create(state, sz + 1);
    native_int copy_size = sz;
    native_int data_size = as<ByteArray>(data_)->size();

    // Check that we don't copy any data outside the existing byte array
    if(unlikely(copy_size > data_size)) {
      copy_size = data_size;
    }
    memcpy(ba->raw_bytes(), byte_address(), copy_size);

    // We've unshared
    shared(state, Qfalse);
    data(state, ba);
    hash_value(state, nil<Fixnum>());

    // If we shrunk it and num_bytes said there was more than there
    // is, clamp it.
    if(num_bytes()->to_native() > sz) {
      num_bytes(state, count);
      num_chars(state, nil<Fixnum>());
    }

    return this;
  }

  String* String::add(STATE, String* other) {
    return string_dup(state)->append(state, other);
  }

  String* String::add(STATE, const char* other) {
    return string_dup(state)->append(state, other);
  }

  Float* String::to_f(STATE) {
    return Float::create(state, to_double(state));
  }

  double String::to_double(STATE) {
    double value;
    char *ba = data_->to_chars(state, num_bytes_);
    char *p, *n, *rest;
    int e_seen = 0;

    p = ba;
    while(ISSPACE(*p)) p++;
    n = p;

    while(*p) {
      if(*p == '_') {
        p++;
      } else {
        if(*p == 'e' || *p == 'E') {
          if(e_seen) {
            *n = 0;
            break;
          }
          e_seen = 1;
        } else if(!(ISDIGIT(*p) || *p == '.' || *p == '-' || *p == '+')) {
          *n = 0;
          break;
        }

        *n++ = *p++;
      }
    }
    *n = 0;

    value = ruby_strtod(ba, &rest);
    free(ba);

    return value;
  }

  // Character-wise logical AND of two strings. Modifies the receiver.
  String* String::apply_and(STATE, String* other) {
    native_int count;
    if(num_bytes_ > other->num_bytes()) {
      count = other->num_bytes()->to_native();
    } else {
      count = num_bytes_->to_native();
    }

    uint8_t* s = byte_address();
    uint8_t* o = other->byte_address();

    // Use && not & to keep 1's in the table.
    for(native_int i = 0; i < count; i++) {
      s[i] = s[i] && o[i];
    }

    return this;
  }

  struct tr_data {
    char tr[256];
    native_int set[256];
    native_int steps;
    native_int last;
    native_int limit;

    bool assign(native_int chr) {
      int j, i = set[chr];

      if(limit >= 0 && steps >= limit) return true;

      if(i < 0) {
        tr[last] = chr;
      } else {
        last--;
        for(j = i + 1; j <= last; j++) {
          set[(native_int)tr[j]]--;
          tr[j-1] = tr[j];
        }
        tr[last] = chr;
      }
      set[chr] = last++;
      steps++;

      return false;
    }
  };

  Fixnum* String::tr_expand(STATE, Object* limit, Object* invalid_as_empty) {
    struct tr_data tr_data;

    tr_data.last = 0;
    tr_data.steps = 0;

    if(Fixnum* lim = try_as<Fixnum>(limit)) {
      tr_data.limit = lim->to_native();
    } else {
      tr_data.limit = -1;
    }

    uint8_t* str = byte_address();
    native_int bytes = byte_size();
    native_int start = bytes > 1 && str[0] == '^' ? 1 : 0;
    memset(tr_data.set, -1, sizeof(native_int) * 256);

    for(native_int i = start; i < bytes;) {
      native_int chr = str[i];
      native_int seq = ++i < bytes ? str[i] : -1;

      if(chr == '\\' && seq >= 0) {
        continue;
      } else if(seq == '-') {
        native_int max = ++i < bytes ? str[i] : -1;
        if(max >= 0 && chr > max && RTEST(invalid_as_empty)) {
          i++;
        } else if(max >= 0) {
          do {
            if(tr_data.assign(chr)) return tr_replace(state, &tr_data);
            chr++;
          } while(chr <= max);
          i++;
        } else {
          if(tr_data.assign(chr)) return tr_replace(state, &tr_data);
          if(tr_data.assign(seq)) return tr_replace(state, &tr_data);
        }
      } else {
        if(tr_data.assign(chr)) return tr_replace(state, &tr_data);
      }
    }

    return tr_replace(state, &tr_data);
  }

  Fixnum* String::tr_replace(STATE, struct tr_data* tr_data) {
    if(tr_data->last + 1 > byte_size() || shared_->true_p()) {
      ByteArray* ba = ByteArray::create(state, tr_data->last + 1);

      data(state, ba);
      shared(state, Qfalse);
    }

    memcpy(byte_address(), tr_data->tr, tr_data->last);
    byte_address()[tr_data->last] = 0;

    num_bytes(state, Fixnum::from(tr_data->last));
    num_chars(state, nil<Fixnum>());

    return Fixnum::from(tr_data->steps);
  }

  String* String::transform(STATE, Tuple* tbl, Object* respect_kcode) {
    uint8_t invalid[5];

    if(tbl->num_fields() < 256) {
      return force_as<String>(Primitives::failure());
    }

    Object** tbl_ptr = tbl->field;

    kcode::table* kcode_tbl = 0;
    if(RTEST(respect_kcode)) {
      kcode_tbl = state->shared().kcode_table();
    } else {
      kcode_tbl = kcode::null_table();
    }

    // Pointers to iterate input bytes.
    uint8_t* in_p = byte_address();

    native_int str_size = byte_size();
    native_int data_size = as<ByteArray>(data_)->size();
    if(unlikely(str_size > data_size)) {
      str_size = data_size;
    }

    uint8_t* in_end = in_p + str_size;

    // Optimistic estimate that output size will be 1.25 x input.
    native_int out_chunk = str_size * 5 / 4;
    native_int out_size = out_chunk;
    uint8_t* output = (uint8_t*)malloc(out_size);

    uint8_t* out_p = output;
    uint8_t* out_end = out_p + out_size;

    while(in_p < in_end) {
      native_int len = 0;
      uint8_t byte = *in_p;
      uint8_t* cur_p = 0;

      if(kcode::mbchar_p(kcode_tbl, byte)) {
        len = kcode::mbclen(kcode_tbl, byte);
        native_int rem = in_end - in_p;

        // if the character length is greater than the remaining
        // bytes, we have a malformed character. Handled below.
        if(rem >= len) {
          cur_p = in_p;
          in_p += len;
        }
      } else if(String* str = try_as<String>(tbl_ptr[byte])) {
        cur_p = str->byte_address();
        len = str->byte_size();
        in_p++;
      } else {
        Tuple* tbl = as<Tuple>(tbl_ptr[byte]);

        for(native_int i = 0; i < tbl->num_fields(); i += 2) {
          String* key = as<String>(tbl->at(i));

          native_int rem = in_end - in_p;
          native_int klen = key->byte_size();
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
        native_int pos = out_p - output;
        out_size += (len > out_chunk ? len : out_chunk);
        output = (uint8_t*)realloc(output, out_size);
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
    free(output);

    infect(state, result);
    return result;
  }

  String* String::copy_from(STATE, String* other, Fixnum* start,
                            Fixnum* size, Fixnum* dest)
  {
    native_int src = start->to_native();
    native_int dst = dest->to_native();
    native_int cnt = size->to_native();

    native_int osz = other->byte_size();
    if(src >= osz) return this;
    if(cnt < 0) return this;
    if(src < 0) src = 0;
    if(cnt > osz - src) cnt = osz - src;

    // This bounds checks on the total capacity rather than the virtual
    // size() of the String. This allows for string adjustment within
    // the capacity without having to change the virtual size first.
    native_int sz = as<ByteArray>(data_)->size();
    if(dst >= sz) return this;
    if(dst < 0) dst = 0;
    if(cnt > sz - dst) cnt = sz - dst;

    memmove(byte_address() + dst, other->byte_address() + src, cnt);

    return this;
  }

  Fixnum* String::compare_substring(STATE, String* other, Fixnum* start,
                                    Fixnum* size)
  {
    native_int src = start->to_native();
    native_int cnt = size->to_native();
    native_int sz = byte_size();
    native_int osz = other->byte_size();
    native_int dsz = as<ByteArray>(data_)->size();
    native_int odsz = as<ByteArray>(other->data_)->size();

    if(unlikely(sz > dsz)) {
      sz = dsz;
    }

    if(unlikely(osz > odsz)) {
      osz = odsz;
    }

    if(src < 0) src = osz + src;

    if(src >= osz) {
      Exception::object_bounds_exceeded_error(state, "start exceeds size of other");
    } else if(src < 0) {
      Exception::object_bounds_exceeded_error(state, "start less than zero");
    }

    if(src + cnt > osz) cnt = osz - src;

    if(cnt > sz) cnt = sz;

    native_int cmp = memcmp(byte_address(), other->byte_address() + src, cnt);

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
      Exception::argument_error(state, "size must be positive");
    }

    String* s = String::create(state, size);
    s->klass(state, (Class*)self);

    self->infect(state, s);

    native_int cnt = size->to_native();

    if(Fixnum* chr = try_as<Fixnum>(pattern)) {
      memset(s->byte_address(), (int)chr->to_native(), cnt);
    } else if(String* pat = try_as<String>(pattern)) {
      pat->infect(state, s);

      native_int psz = pat->byte_size();
      if(psz == 1) {
        memset(s->byte_address(), pat->byte_address()[0], cnt);
      } else if(psz > 1) {
        native_int i, j, n;

        native_int sz = cnt / psz;
        for(n = i = 0; i < sz; i++) {
          for(j = 0; j < psz; j++, n++) {
            s->byte_address()[n] = pat->byte_address()[j];
          }
        }
        for(i = n, j = 0; i < cnt; i++, j++) {
          s->byte_address()[i] = pat->byte_address()[j];
        }
      }
    } else {
      Exception::argument_error(state, "pattern must be a Fixnum or String");
    }

    return s;
  }

  static void invalid_codepoint_error(STATE, unsigned int c) {
    std::ostringstream msg;
    msg << "invalid codepoint: " << c;
    Exception::range_error(state, msg.str().c_str());
  }

  String* String::from_codepoint(STATE, Object* self, Integer* code, Encoding* enc) {
    String* s = state->new_object<String>(G(string));

    unsigned int c = code->to_uint();
    int n = ONIGENC_CODE_TO_MBCLEN(enc->get_encoding(), c);

    if(n <= 0) invalid_codepoint_error(state, c);

    s->num_bytes(state, Fixnum::from(n));
    s->hash_value(state, nil<Fixnum>());
    s->shared(state, Qfalse);
    s->encoding(state, enc);

    ByteArray* ba = ByteArray::create(state, n);

    n = ONIGENC_CODE_TO_MBC(enc->get_encoding(), c ,(UChar*)ba->raw_bytes());
    if(n <= 0) invalid_codepoint_error(state, c);

    s->data(state, ba);

    return s;
  }

  String* String::crypt(STATE, String* salt) {
    return String::create(state, ::crypt(this->c_str(state), salt->c_str(state)));
  }

  Integer* String::to_i(STATE, Fixnum* fix_base, Object* strict) {
    const char* str = c_str(state);
    int base = fix_base->to_native();

    if(strict == Qtrue) {
      // In strict mode the string can't have null bytes.
      if(byte_size() > (native_int)strlen(str)) return nil<Integer>();
    }

    return Integer::from_cstr(state, str, base, strict);
  }

  Integer* String::to_inum_prim(STATE, Fixnum* base, Object* strict) {
    Integer* val = to_i(state, base, strict);
    if(val->nil_p()) return (Integer*)Primitives::failure();

    return val;
  }

  Object* String::aref(STATE, Fixnum* index) {
    native_int i = index->to_native();

    if(i < 0) i += byte_size();
    if(i >= byte_size() || i < 0) return Qnil;

    if(LANGUAGE_18_ENABLED(state)) {
      return Fixnum::from(byte_address()[i]);
    }

    if(byte_compatible_p(encoding_)) {
      return byte_substring(state, i, 1);
    } else {
      // Assumptions above about size are possibly invalid, recalculate.
      i = index->to_native();
      if(i < 0) i += char_size(state);
      if(i >= char_size(state) || i < 0) return Qnil;

      return char_substring(state, i, 1);
    }
  }

  /* Returns the byte index of the character at logical 'index'. The 'start'
   * parameter is the byte index of a character at which to start searching.
   */
  native_int String::find_character_byte_index(STATE, native_int index,
                                               native_int start)
  {
    if(byte_compatible_p(encoding_)) {
      return index;
    } else if(fixed_width_p(encoding_)) {
      return index * ONIGENC_MBC_MINLEN(encoding_->get_encoding());
    } else {
      OnigEncodingType* enc = encoding_->get_encoding();
      uint8_t* p = byte_address() + start;
      uint8_t* e = byte_address() + byte_size();

      while(p < e && index--) {
        p += mbclen(p, e, enc);
      }

      if(p > e) p = e;
      return p - byte_address();
    }
  }

  /* The 'index' and 'length' parameters are byte based. This method is a
   * worker method called from other methods that have already computed the
   * canonical byte (0 <= index < byte_size) and (0 <= length < byte_size).
   */
  String* String::byte_substring(STATE, native_int index, native_int length) {
    native_int data_size = as<ByteArray>(data_)->size();

    // Clamp the range to the underlying byte array size
    if(unlikely(index > data_size)) index = data_size;

    if(unlikely(index + length > data_size)) {
      length = data_size - index;
    }

    String* sub = String::create(state, Fixnum::from(length));
    sub->klass(state, class_object(state));

    uint8_t* buf = byte_address() + index;
    memcpy(sub->byte_address(), buf, length);

    if(tainted_p(state)->true_p()) sub->taint(state);
    if(untrusted_p(state)->true_p()) sub->untrust(state);
    sub->encoding(state, encoding());

    return sub;
  }

  /* The 'index' and 'length' parameters are character based. This method is a
   * worker method called from other methods that have already computed the
   * canonical character (0 <= index < char_size) and (0 <= length < char_size).
   */
  String* String::char_substring(STATE, native_int index, native_int length) {
    native_int i = find_character_byte_index(state, index);
    native_int e = find_character_byte_index(state, length - 1, i);

    int c = precise_mbclen(byte_address() + e, byte_address() + byte_size(),
                           encoding_->get_encoding());

    if(ONIGENC_MBCLEN_CHARFOUND_P(c)) {
      e += ONIGENC_MBCLEN_CHARFOUND_LEN(c);
    } else {
      e += 1;
    }

    return byte_substring(state, i, e - i);
  }

  String* String::byte_substring(STATE, Fixnum* index, Fixnum* length) {
    native_int i = index->to_native();
    native_int n = length->to_native();
    native_int size = byte_size();

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
    native_int i = index->to_native();
    native_int n = length->to_native();
    native_int size;

    if(n < 0) return nil<String>();

    if(byte_compatible_p(encoding_)) {
      size = byte_size();
    } else {
      size = char_size(state);
    }

    if(i < 0) {
      i += size;
      if(i < 0) return nil<String>();
    } else if(i > size) {
      return nil<String>();
    }

    if(i + n > size) {
      n = size - i;
    }

    if(n == 0 || byte_compatible_p(encoding_)) {
      return byte_substring(state, i, n);
    } else {
      return char_substring(state, i, n);
    }
  }

  Fixnum* String::index(STATE, String* pattern, Fixnum* start) {
    native_int total = byte_size();
    native_int match_size = pattern->byte_size();

    if(start->to_native() < 0) {
      Exception::argument_error(state, "negative start given");
    }

    switch(match_size) {
    case 0:
      return start;
    case 1:
      {
        uint8_t* buf = byte_address();
        uint8_t matcher = pattern->byte_address()[0];

        for(native_int pos = start->to_native(); pos < total; pos++) {
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

  Fixnum* String::rindex(STATE, String* pattern, Fixnum* start) {
    native_int total = byte_size();
    native_int match_size = pattern->byte_size();
    native_int pos = start->to_native();

    if(pos < 0) {
      Exception::argument_error(state, "negative start given");
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

  String* String::find_character(STATE, Fixnum* offset) {
    native_int o = offset->to_native();
    if(o >= byte_size()) return nil<String>();
    if(o < 0) return nil<String>();

    uint8_t* cur = byte_address() + o;

    String* output = 0;

    kcode::table* tbl = state->shared().kcode_table();
    if(kcode::mbchar_p(tbl, *cur)) {
      native_int clen = kcode::mbclen(tbl, *cur);
      if(o + clen <= byte_size()) {
        output = String::create(state, reinterpret_cast<const char*>(cur), clen);
      }
    }

    if(!output) {
      output = String::create(state, reinterpret_cast<const char*>(cur), 1);
    }

    output->klass(state, class_object(state));
    if(RTEST(tainted_p(state))) output->taint(state);

    return output;
  }

  Array* String::awk_split(STATE, Fixnum* f_limit) {
    native_int limit = f_limit->to_native();

    native_int sz = byte_size();
    uint8_t* start = byte_address();
    int end = 0;
    int begin = 0;

    native_int i = 0;
    if(limit > 0) i = 1;

    bool skip = true;

    Array* ary = Array::create(state, 3);

    Class* out_class = class_object(state);
    int taint = (is_tainted_p() ? 1 : 0);

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
          str->set_tainted(taint);


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
      str->set_tainted(taint);

      ary->append(state, str);
    }
    return ary;
  }

  Object* String::ascii_only_p(STATE) {
    if(ascii_only_->nil_p()) {
      if(byte_size() == 0) {
        ascii_only(state, encoding(state)->ascii_compatible_p(state));
      } else {
        if(find_non_ascii(byte_address(), byte_size())) {
          ascii_only(state, Qfalse);
        } else {
          ascii_only(state, Qtrue);
        }
      }
    }

    return ascii_only_;
  }

  Object* String::valid_encoding_p(STATE) {
    if(valid_encoding_->nil_p()) {
      if(encoding(state) == Encoding::from_index(state, Encoding::eBinary)) {
        valid_encoding(state, Qtrue);
        return valid_encoding_;
      }

      OnigEncodingType* enc = encoding_->get_encoding();

      uint8_t* p = byte_address();
      uint8_t* e = p + byte_size();

      while(p < e) {
        int n = precise_mbclen(p, e, enc);

        if(!ONIGENC_MBCLEN_CHARFOUND_P(n)) {
          valid_encoding(state, Qfalse);
          return valid_encoding_;
        }

        p += n;
      }

      valid_encoding(state, Qtrue);
    }

    return valid_encoding_;
  }

  Fixnum* String::codepoint(STATE) {
    if(char_size(state) == 0) return force_as<Fixnum>(Primitives::failure());

    if(byte_compatible_p(encoding_)) {
      return Fixnum::from(byte_address()[0]);
    } else {
      OnigEncodingType* enc = encoding_->get_encoding();
      uint8_t* p = byte_address();
      uint8_t* e = p + byte_size();

      int n = precise_mbclen(p, e, enc);

      if(ONIGENC_MBCLEN_CHARFOUND_P(n)) {
        return Fixnum::from(ONIGENC_MBC_TO_CODE(enc, (UChar*)p, (UChar*)e));
      }
    }

    return force_as<Fixnum>(Primitives::failure());
  }

  void String::Info::show(STATE, Object* self, int level) {
    String* str = as<String>(self);
    std::cout << "\"" << str->c_str(state) << "\"" << std::endl;
  }

  void String::Info::show_simple(STATE, Object* self, int level) {
    show(state, self, level);
  }
}

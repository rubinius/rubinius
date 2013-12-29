#include "oniguruma.h" // Must be first.
#include "transcoder.h"
#include "regenc.h"

#include "builtin/array.hpp"
#include "builtin/byte_array.hpp"
#include "builtin/character.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/integer.hpp"
#include "builtin/native_method.hpp"
#include "builtin/regexp.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "capi/handle.hpp"
#include "configuration.hpp"
#include "object_utils.hpp"
#include "object_memory.hpp"
#include "ontology.hpp"
#include "util/murmur_hash3.hpp"
#include "util/siphash.h"
#include "util/spinlock.hpp"
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

  void String::init(STATE) {
    GO(string).set(ontology::new_class(state, "String", G(object)));
    G(string)->set_object_type(state, StringType);
  }

  String* String::allocate(STATE, Object* self) {
    String* str = state->new_object<String>(G(string));
    str->klass(state, as<Class>(self));
    return str;
  }

  /* Creates a String instance with +num_bytes+ == +size+ and
   * having a ByteArray with at least (size + 1) bytes.
   */
  String* String::create(STATE, Fixnum* size) {
    String *so;

    so = state->new_object_dirty<String>(G(string));

    so->num_bytes_      = size;
    so->hash_value_     = nil<Fixnum>();
    so->shared_         = cFalse;

    native_int bytes = size->to_native() + 1;
    ByteArray* ba = ByteArray::create(state, bytes);
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
    so->shared(state, cFalse);

    native_int bytes = size->to_native() + 1;
    ByteArray* ba = ByteArray::create_pinned(state, bytes);

    so->data(state, ba);

    return so;
  }

  String* String::create(STATE, const char* str) {
    if(!str) return String::create(state, Fixnum::from(0));

    native_int bytes = strlen(str);

    return String::create(state, str, bytes);
  }

  /* +bytes+ should NOT attempt to take the trailing null into account
   * +bytes+ is the number of 'real' characters in the string
   */
  String* String::create(STATE, const char* str, native_int bytes) {

    String* so = state->new_object_dirty<String>(G(string));

    so->num_bytes_      = Fixnum::from(bytes);
    so->hash_value_     = nil<Fixnum>();
    so->shared_         = cFalse;

    ByteArray* ba = ByteArray::create_dirty(state, bytes + 1);

    if(str) {
      memcpy(ba->raw_bytes(), str, bytes);
      ba->raw_bytes()[bytes] = 0;
    } else {
      memset(ba->raw_bytes(), 0, bytes + 1);
    }

    so->data(state, ba);
    return so;
  }

  String* String::from_bytearray(STATE, ByteArray* ba, native_int size) {
    String* s = state->new_object_dirty<String>(G(string));

    s->num_bytes_      = Fixnum::from(size);
    s->hash_value_     = nil<Fixnum>();
    s->shared_         = cFalse;

    s->data(state, ba);

    return s;
  }

  String* String::from_bytearray(STATE, ByteArray* ba, Fixnum* start,
                                 Fixnum* count)
  {
    String* s = state->new_object<String>(G(string));

    s->num_bytes(state, count);
    s->hash_value(state, nil<Fixnum>());
    s->shared(state, cFalse);

    // fetch_bytes NULL terminates
    s->data(state, ba->fetch_bytes(state, start, count));

    return s;
  }

  void String::update_handle(VM* vm) {
    State state(vm);
    update_handle(&state);
  }

  void String::update_handle(STATE) {
    capi::Handle* handle = this->handle(state);
    if(!handle) return;

    handle->update(state->vm()->native_method_environment);
  }

  Fixnum* String::size(STATE) {
    return Fixnum::from(num_bytes());
  }

  hashval String::hash_string(STATE) {
    if(!hash_value_->nil_p()) {
      return (hashval)as<Fixnum>(hash_value_)->to_native();
    }

    unsigned char* bp = (unsigned char*)(byte_address());

    hashval h = hash_str(state, bp, byte_size());
    hash_value(state, Fixnum::from(h));

    return h;
  }

  hashval String::hash_str(const unsigned char *bp, unsigned int sz, uint32_t seed) {
#ifdef USE_MURMUR3
#ifdef IS_X8664
    hashval hv[2];
    MurmurHash3_x64_128(bp, sz, seed, hv);
#else
    hashval hv[1];
    MurmurHash3_x86_32(bp, sz, seed, hv);
#endif
    return hv[0] & FIXNUM_MAX;
#else
    uint64_t v = siphash24(siphash_key, seed, bp, sz);
    return ((hashval)v) & FIXNUM_MAX;
#endif
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
      ByteArray* ba = ByteArray::create_dirty(state, current_size + 1);
      memcpy(ba->raw_bytes(), byte_address(), current_size);
      data(state, ba);
      if(CBOOL(shared_)) shared(state, cFalse);
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
    if(byte_size() > (native_int) strnlen(str, byte_size())) {
      Exception::argument_error(state, "string contains NULL byte");
    }
    return str;
  }

  String* String::check_null_safe(STATE) {
    c_str_null_safe(state);
    return this;
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

    return RBOOL(sum == 0);
  }

  String* String::string_dup_slow(STATE) {
    Module* mod = klass_;
    Class*  cls = try_as_instance<Class>(mod);

    if(!cls) {
      while(!cls) {
        mod = mod->superclass();

        if(mod->nil_p()) rubinius::bug("Object::class_object() failed to find a class");

        cls = try_as_instance<Class>(mod);
      }
    }

    String* so = state->new_object_dirty<String>(cls);

    so->copy_object(state, this);
    so->shared(state, cTrue);
    shared(state, cTrue);

    return so;
  }

  void String::unshare(STATE) {
    if(CBOOL(shared_)) {
      if(data_->reference_p()) {
        data(state, as<ByteArray>(data_->duplicate(state)));
      }
      shared(state, cFalse);
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

  String* String::byte_append(STATE, String* other) {
    native_int length = other->byte_size();
    native_int data_length = as<ByteArray>(other->data_)->size();

    if(unlikely(length > data_length)) {
      length = data_length;
    }
    if(!other->ascii_only()->true_p()) {
      ascii_only_ = cNil;
    }
    if(!other->valid_encoding()->true_p()) {
      valid_encoding_ = cNil;
    }
    return append(state,
                  reinterpret_cast<const char*>(other->byte_address()),
                  length);
  }

  String* String::append(STATE, const char* other, native_int length) {
    native_int current_size = byte_size();
    native_int data_size = as<ByteArray>(data_)->size();

    // Clamp the string size to the maximum underlying byte array size
    if(unlikely(current_size > data_size)) {
      current_size = data_size;
    }

    native_int new_size = current_size + length;
    native_int capacity = data_size;

    // Check for overflow, too big if that happens
    if(new_size < 0) {
      Exception::argument_error(state, "string sizes too big");
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
      if(CBOOL(shared_)) shared(state, cFalse);

      ByteArray* ba = ByteArray::create_dirty(state, capacity);
      memcpy(ba->raw_bytes(), byte_address(), current_size);
      memset(ba->raw_bytes() + new_size, 0, capacity - new_size);
      data(state, ba);
    } else {
      if(CBOOL(shared_)) unshare(state);
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
    native_int sz = count->to_native();

    if(sz < 0) {
      Exception::argument_error(state, "negative byte array size");
    }

    ByteArray* ba = ByteArray::create_dirty(state, sz + 1);
    native_int copy_size = sz;
    native_int data_size = as<ByteArray>(data_)->size();

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

    if(CBOOL(strict) && byte_size() > (native_int)strnlen(str, byte_size())) {
      return nil<Float>();
    }

    return Float::from_cstr(state, str, str + byte_size(), strict);
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

  Fixnum* String::tr_expand(STATE, Object* limit, Object* invalid_as_empty) {
    native_int lim = -1;
    if(Fixnum* l = try_as<Fixnum>(limit)) {
      lim = l->to_native();
    }

    uint8_t* str = byte_address();
    native_int bytes = byte_size();

    for(native_int i = 0; i < bytes;) {
      native_int chr = str[i];
      native_int seq = ++i < bytes ? str[i] : -1;

      if(chr == '\\' && seq >= 0) {
        continue;
      } else if(seq == '-') {
        native_int start = i - 1;
        native_int max = ++i < bytes ? str[i] : -1;
        native_int next = max >= 0 ? i + 1 : i;
        if(max >= 0) {
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

  native_int String::tr_replace(STATE, native_int first, native_int last, native_int start, native_int next) {
    native_int replace_length = last - first;
    native_int added_chars = replace_length - next + start;
    native_int new_size = byte_size() + added_chars + 1;

    if(new_size > byte_size() || shared_->true_p()) {
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

  String* String::transform(STATE, Tuple* tbl, Object* respect_kcode) {
    uint8_t invalid[5];

    if(tbl->num_fields() < 256) {
      return force_as<String>(Primitives::failure());
    }

    Object** tbl_ptr = tbl->field;

    kcode::table* kcode_tbl = 0;
    if(CBOOL(respect_kcode)) {
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
        uint8_t* new_output = (uint8_t*)realloc(output, out_size);
        if(!new_output) {
          free(output);
          Exception::memory_error(state);
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

    native_int cnt = size->to_native();

    String* s = state->new_object_dirty<String>(as<Class>(self));

    s->num_bytes(state, size);
    s->num_chars(state, nil<Fixnum>());
    s->hash_value(state, nil<Fixnum>());
    s->shared(state, cFalse);

    ByteArray* ba = ByteArray::create_dirty(state, cnt + 1);

    if(Fixnum* chr = try_as<Fixnum>(pattern)) {
      memset(ba->raw_bytes(), (int)chr->to_native(), cnt);
    } else if(String* pat = try_as<String>(pattern)) {
      pat->infect(state, s);

      native_int psz = pat->byte_size();
      uint8_t* raw = ba->raw_bytes();
      if(psz == 1) {
        memset(raw, pat->byte_address()[0], cnt);
      } else if(psz > 1) {
        native_int sz = cnt / psz;
        native_int len = sz * psz;
        if(len >= psz) {
          memcpy(raw, pat->byte_address(), psz);
          while(psz <= len / 2) {
            memcpy(raw + psz, raw, psz);
            psz *= 2;
          }
          memcpy(raw + psz, raw, len - psz);
        }
        for(native_int i = len, j = 0; i < cnt; i++, j++) {
          raw[i] = pat->byte_address()[j];
        }
      }
      s->ascii_only(state, pat->ascii_only_p(state));
      s->valid_encoding(state, pat->valid_encoding_p(state));
      s->encoding_from(state, pat);
    } else {
      Exception::argument_error(state, "pattern must be a Fixnum or String");
    }

    s->data(state, ba);
    return s;
  }

  static int crypt_lock = RBX_SPINLOCK_UNLOCKED;
  String* String::crypt(STATE, String* salt) {
    rbx_spinlock_lock(&crypt_lock);
    char* result = ::crypt(this->c_str(state), salt->c_str(state));
    if(!result) {
      rbx_spinlock_unlock(&crypt_lock);
      Exception::errno_error(state, "crypt(3) failed");
      return NULL;
    }
    String* res = String::create(state, result);
    rbx_spinlock_unlock(&crypt_lock);
    return res;
  }

  Integer* String::to_i(STATE, Fixnum* fix_base, Object* strict) {
    const char* str = reinterpret_cast<const char*>(byte_address());
    int base = fix_base->to_native();

    if(CBOOL(strict)) {
      // In strict mode the string can't have null bytes.
      if(byte_size() > (native_int)strnlen(str, byte_size())) return nil<Integer>();
    }

    return Integer::from_cstr(state, str, str + byte_size(), base, strict);
  }

  Integer* String::to_inum_prim(STATE, Fixnum* base, Object* strict) {
    Integer* val = to_i(state, base, strict);
    if(val->nil_p()) return (Integer*)Primitives::failure();

    return val;
  }

  Object* String::aref(STATE, Fixnum* index) {
    native_int i = index->to_native();

    if(i < 0) i += byte_size();
    if(i >= byte_size() || i < 0) return cNil;

    return Fixnum::from(byte_address()[i]);
  }

  /* Returns the byte index of the character at logical 'index'. The 'start'
   * parameter is the byte index of a character at which to start searching.
   */
  native_int String::find_character_byte_index(STATE, native_int index,
                                               native_int start) {
    return start + index;
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

    const char* buf = (const char*)byte_address() + index;
    String* sub = String::create(state, buf, length);
    sub->klass(state, class_object(state));

    infect(state, sub);
    sub->encoding_from(state, this);

    return sub;
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
    native_int size = char_size(state);

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

  Encoding* String::get_encoding_kcode_fallback(STATE) {
    switch(state->shared().kcode_page()) {
    default:
    case kcode::eAscii:
      return Encoding::ascii8bit_encoding(state);
    case kcode::eEUC:
      return Encoding::find(state, "EUC-JP");
    case kcode::eSJIS:
      return Encoding::find(state, "Windows-31J");
    case kcode::eUTF8:
      return Encoding::utf8_encoding(state);
    }
  }

  String* String::find_character(STATE, Fixnum* offset) {
    native_int o = offset->to_native();
    if(o >= byte_size()) return nil<String>();
    if(o < 0) return nil<String>();

    uint8_t* cur = byte_address() + o;

    String* output = 0;

    OnigEncodingType* enc = get_encoding_kcode_fallback(state)->get_encoding();

    if(ONIGENC_MBC_MAXLEN(enc) == 1) {
      output = String::create(state, reinterpret_cast<const char*>(cur), 1);
    } else {
      kcode::table* kcode_tbl = state->shared().kcode_table();
      int len = kcode::mbclen(kcode_tbl, *cur);
      output = String::create(state, reinterpret_cast<const char*>(cur), len);
    }

    output->klass(state, class_object(state));
    infect(state, output);
    output->encoding_from(state, this);

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

  String* String::reverse(STATE) {
    if(byte_size() <= 1) return this;
    check_frozen(state);

    unshare(state);
    hash_value(state, nil<Fixnum>());

    if(byte_compatible_p(encoding_) || CBOOL(ascii_only_p(state))) {
      data_->reverse(state, Fixnum::from(0), num_bytes_);
    } else {
      Encoding::string_reverse(byte_address(), byte_address() + byte_size(), encoding_->get_encoding());
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

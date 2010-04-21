#include "builtin/string.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/float.hpp"
#include "builtin/integer.hpp"
#include "builtin/tuple.hpp"

#include "vm.hpp"
#include "object_utils.hpp"
#include "objectmemory.hpp"
#include "primitives.hpp"

#include "util/base64.hpp"

#include <gdtoa.h>

#include <unistd.h>
#include <iostream>
#include <ctype.h>

namespace rubinius {

  void String::init(STATE) {
    GO(string).set(state->new_class("String", G(object)));
    G(string)->set_object_type(state, StringType);
  }

  /* Creates a String instance with +num_bytes+ == +size+ and
   * having a ByteArray with at least (size + 1) bytes.
   */
  String* String::create(STATE, Fixnum* size) {
    String *so;

    so = state->new_object<String>(G(string));

    so->num_bytes(state, size);
    so->characters(state, size);
    so->encoding(state, Qnil);
    so->hash_value(state, (Integer*)Qnil);
    so->shared(state, Qfalse);

    size_t bytes = size->to_native() + 1;
    ByteArray* ba = ByteArray::create(state, bytes);
    ba->raw_bytes()[bytes-1] = 0;

    so->data(state, ba);

    return so;
  }

  String* String::create_reserved(STATE, size_t bytes) {
    String *so;

    so = state->new_object<String>(G(string));

    so->num_bytes(state, Fixnum::from(0));
    so->characters(state, Fixnum::from(0));
    so->encoding(state, Qnil);
    so->hash_value(state, (Integer*)Qnil);
    so->shared(state, Qfalse);

    ByteArray* ba = ByteArray::create(state, bytes);
    ba->raw_bytes()[bytes-1] = 0;

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
    so->characters(state, size);
    so->encoding(state, Qnil);
    so->hash_value(state, (Integer*)Qnil);
    so->shared(state, Qfalse);

    size_t bytes = size->to_native() + 1;
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

    size_t bytes = strlen(str);

    return String::create(state, str, bytes);
  }

  /* +bytes+ should NOT attempt to take the trailing null into account
   * +bytes+ is the number of 'real' characters in the string
   */
  String* String::create(STATE, const char* str, size_t bytes) {
    String* so = String::create(state, Fixnum::from(bytes));

    if(str) std::memcpy(so->byte_address(), str, bytes);

    return so;
  }

  String* String::from_bytearray(STATE, ByteArray* ba, Integer* start, Integer* count) {
    String* s = state->new_object<String>(G(string));

    s->num_bytes(state, count);
    s->characters(state, count);
    s->encoding(state, Qnil);
    s->hash_value(state, (Integer*)Qnil);
    s->shared(state, Qfalse);

    // fetch_bytes NULL terminates
    s->data(state, ba->fetch_bytes(state, start, count));

    return s;
  }

  hashval String::hash_string(STATE) {
    unsigned char *bp;

    if(!hash_value_->nil_p()) {
      return (hashval)as<Integer>(hash_value_)->to_native();
    }
    bp = (unsigned char*)(byte_address());
    size_t sz = size();

    hashval h = hash_str(bp, sz);
    hash_value(state, Integer::from(state, h));

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

  static inline unsigned long update_hash(unsigned long hv, unsigned char byte) {
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
    unsigned char *be;
    hashval hv;

    be = (unsigned char*)bp + sz;

    hv = FNVOffsetBasis;

    while(bp < be) {
      hv = update_hash(hv, *bp++);
    }

    return finish_hash(hv);
  }

  Symbol* String::to_sym(STATE) {
    return state->symbol(this);
  }

  const char* String::c_str() {
    char* c_string = (char*)byte_address();
    if(c_string[size()] != 0) {
      c_string[size()] = 0;
    }

    return c_string;
  }

  bool String::string_equal_p(STATE, Object* a, Object* b) {
    String* self = as<String>(a);
    String* other = as<String>(b);

    if(self->num_bytes() != other->num_bytes()) return false;
    if(memcmp(self->byte_address(), other->byte_address(), self->num_bytes()->to_native())) {
      return false;
    }

    return true;
  }

  Object* String::equal(STATE, String* other) {
    bool ret = String::string_equal_p(state, this, other);
    return ret ? Qtrue : Qfalse;
  }

  String* String::string_dup(STATE) {
    String* ns;

    ns = as<String>(duplicate(state));
    ns->shared(state, Qtrue);
    shared(state, Qtrue);

    // Fix for subclassing
    ns->klass(state, class_object(state));

    return ns;
  }

  void String::unshare(STATE) {
    if(shared_->true_p()) {
      if(data_->reference_p()) {
        data(state, as<ByteArray>(data_->duplicate(state)));
      }
      shared(state, Qfalse);
    }
  }

  String* String::append(STATE, String* other) {
    return append(state, reinterpret_cast<const char*>(other->byte_address()), other->size());
  }

  String* String::append(STATE, const char* other) {
    return append(state, other, std::strlen(other));
  }

  String* String::append(STATE, const char* other, std::size_t length) {
    size_t new_size = size() + length;
    size_t capacity = data_->size();

    if(capacity < (new_size + 1)) {
      // capacity needs one extra byte of room for the trailing null
      do {
        // @todo growth should be more intelligent than doubling
        capacity *= 2;
      } while(capacity < (new_size + 1));

      // No need to call unshare and duplicate a ByteArray
      // just to throw it away.
      if(shared_ == Qtrue) shared(state, Qfalse);

      ByteArray *ba = ByteArray::create(state, capacity);
      std::memcpy(ba->raw_bytes(), byte_address(), size());
      data(state, ba);
    } else {
      if(shared_ == Qtrue) unshare(state);
    }

    // Append on top of the null byte at the end of s1, not after it
    std::memcpy(byte_address() + size(), other, length);

    // The 0-based index of the last character is new_size - 1
    byte_address()[new_size] = 0;

    num_bytes(state, Integer::from(state, new_size));
    hash_value(state, (Integer*)Qnil);

    return this;
  }

  String* String::add(STATE, String* other) {
    return string_dup(state)->append(state, other);
  }

  String* String::add(STATE, const char* other) {
    return string_dup(state)->append(state, other);
  }

  Float* String::to_f(STATE) {
    return Float::create(state, this->to_double(state));
  }

  double String::to_double(STATE) {
    double value;
    char *ba = data_->to_chars(state);
    char *p, *n, *rest;
    int e_seen = 0;

    p = ba;
    while (ISSPACE(*p)) p++;
    n = p;

    while (*p) {
      if (*p == '_') {
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

    value = ::ruby_strtod(ba, &rest);
    free(ba);

    return value;
  }

  // Character-wise logical AND of two strings. Modifies the receiver.
  String* String::apply_and(STATE, String* other) {
    native_int count, i;
    uint8_t* s = byte_address();
    uint8_t* o = other->byte_address();

    if(num_bytes_ > other->num_bytes()) {
      count = other->num_bytes()->to_native();
    } else {
      count = num_bytes_->to_native();
    }

    // Use && not & to keep 1's in the table.
    for(i = 0; i < count; i++) {
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
      int j, i = this->set[chr];

      if(this->limit >= 0 && this->steps >= this->limit) return true;

      if(i < 0) {
        this->tr[this->last] = chr;
      } else {
        this->last--;
        for(j = i + 1; j <= this->last; j++) {
          this->set[(native_int)this->tr[j]]--;
          this->tr[j-1] = this->tr[j];
        }
        this->tr[this->last] = chr;
      }
      this->set[chr] = this->last++;
      this->steps++;

      return false;
    }
  };

  Fixnum* String::tr_expand(STATE, Object* limit) {
    struct tr_data tr_data;
    native_int seq;
    native_int max;
    native_int chr;

    tr_data.last = 0;
    tr_data.steps = 0;

    if(Integer* lim = try_as<Integer>(limit)) {
      tr_data.limit = lim->to_native();
    } else {
      tr_data.limit = -1;
    }

    uint8_t* str = byte_address();
    native_int bytes = (native_int)this->size();
    native_int start = bytes > 1 && str[0] == '^' ? 1 : 0;
    std::memset(tr_data.set, -1, sizeof(native_int) * 256);

    for(native_int i = start; i < bytes;) {
      chr = str[i];
      seq = ++i < bytes ? str[i] : -1;

      if(seq == '-') {
        max = ++i < bytes ? str[i] : -1;
        if(max >= 0) {
          while(chr <= max) {
            if(tr_data.assign(chr)) return tr_replace(state, &tr_data);
            chr++;
          }
          i++;
        } else {
          if(tr_data.assign(chr)) return tr_replace(state, &tr_data);
          if(tr_data.assign(seq)) return tr_replace(state, &tr_data);
        }
      } else if(chr == '\\' && seq >= 0) {
        continue;
      } else {
        if(tr_data.assign(chr)) return tr_replace(state, &tr_data);
      }
    }

    return tr_replace(state, &tr_data);
  }

  Fixnum* String::tr_replace(STATE, struct tr_data* tr_data) {
    if(tr_data->last > (native_int)size() || shared_->true_p()) {
      ByteArray* ba = ByteArray::create(state, tr_data->last + 1);

      data(state, ba);
      shared(state, Qfalse);
    }

    std::memcpy(byte_address(), tr_data->tr, tr_data->last);
    byte_address()[tr_data->last] = 0;

    num_bytes(state, Fixnum::from(tr_data->last));
    characters(state, num_bytes_);

    return Fixnum::from(tr_data->steps);
  }

  String* String::transform(STATE, Tuple* tbl, Object* respect_kcode) {
    uint8_t* cur = byte_address();
    uint8_t* fin = cur + size();

    if(tbl->num_fields() < 256) {
      return force_as<String>(Primitives::failure());
    }

    Object** tbl_ptr = tbl->field;

    kcode::table* kcode_tbl = 0;
    if(RTEST(respect_kcode)) {
      kcode_tbl = state->shared.kcode_table();
    } else {
      kcode_tbl = kcode::null_table();
    }

    // Calculate the final size of result
    size_t size = 0;

    while(cur < fin) {
      uint8_t byte = *cur;
      if(kcode::mbchar_p(kcode_tbl, byte)) {
        size_t clen = kcode::mbclen(kcode_tbl, byte);
        size += clen;
        cur += clen;
        continue;
      } else {
        size += as<String>(tbl_ptr[byte])->size();
      }
      cur++;
    }

    cur = byte_address();
    String* result = String::create(state, Fixnum::from(size));

    // Since we precalculated the size, we can write directly into result
    uint8_t* output = result->byte_address();

    while(cur < fin) {
      uint8_t byte = *cur;
      if(kcode::mbchar_p(kcode_tbl, byte)) {
        size_t len = kcode::mbclen(kcode_tbl, byte);
        memcpy(output, cur, len);
        output += len;
        cur += len;
        continue;
      } else {
        // Not unsafe, because we've type checked tbl_ptr above
        String* what = force_as<String>(tbl_ptr[byte]);
        uint8_t* what_buf = what->byte_address();

        switch(what->size()) {
        case 1:
          *output++ = *what_buf;
          break;
        case 2:
          *output++ = *what_buf++;
          *output++ = *what_buf;
          break;
        case 3:
          *output++ = *what_buf++;
          *output++ = *what_buf++;
          *output++ = *what_buf;
          break;
        default:
          memcpy(output, what_buf, what->size());
          output += what->size();
          break;
        }
      }
      cur++;
    }

    if(tainted_p(state)) result->taint(state);
    return result;
  }

  String* String::copy_from(STATE, String* other, Fixnum* start, Fixnum* size, Fixnum* dest) {
    native_int src = start->to_native();
    native_int dst = dest->to_native();
    native_int cnt = size->to_native();

    native_int osz = other->size();
    if(src >= osz) return this;
    if(src < 0) src = 0;
    if(cnt > osz - src) cnt = osz - src;

    native_int sz = this->size();
    if(dst >= sz) return this;
    if(dst < 0) dst = 0;
    if(cnt > sz - dst) cnt = sz - dst;

    std::memcpy(byte_address() + dst, other->byte_address() + src, cnt);

    return this;
  }

  Fixnum* String::compare_substring(STATE, String* other, Fixnum* start, Fixnum* size) {
    native_int src = start->to_native();
    native_int cnt = size->to_native();
    native_int sz = (native_int)this->size();
    native_int osz = (native_int)other->size();

    if(src < 0) src = osz + src;
    if(src >= osz) {
      Exception::object_bounds_exceeded_error(state, "start exceeds size of other");
    } else if(src < 0) {
      Exception::object_bounds_exceeded_error(state, "start less than zero");
    }
    if(src + cnt > osz) cnt = osz - src;

    if(cnt > sz) cnt = sz;

    native_int cmp = std::memcmp(byte_address(), other->byte_address() + src, cnt);

    if(cmp < 0) {
      return Fixnum::from(-1);
    } else if(cmp > 0) {
      return Fixnum::from(1);
    } else {
      return Fixnum::from(0);
    }
  }

  String* String::pattern(STATE, Object* self, Fixnum* size, Object* pattern) {
    String* s = String::create(state, size);
    s->klass(state, (Class*)self);

    self->infect(state, s);

    native_int cnt = size->to_native();

    if(Fixnum* chr = try_as<Fixnum>(pattern)) {
      std::memset(s->byte_address(), (int)chr->to_native(), cnt);
    } else if(String* pat = try_as<String>(pattern)) {
      pat->infect(state, s);

      native_int psz = pat->size();
      if(psz == 1) {
        std::memset(s->byte_address(), pat->byte_address()[0], cnt);
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
      Exception::argument_error(state, "pattern must be Fixnum of String");
    }

    return s;
  }

  String* String::crypt(STATE, String* salt) {
    const char* s = ::crypt(this->c_str(), salt->c_str());
    return String::create(state, s);
  }

  Integer* String::to_i(STATE, Fixnum* fix_base, Object* strict) {
    const char* str = c_str();
    int base = fix_base->to_native();
    bool negative = false;
    Integer* value = Fixnum::from(0);

    if(strict == Qtrue) {
      // In strict mode the string can't have null bytes.
      if(size() > strlen(str)) return (Integer*)Qnil;
    }

    if(base == 1 || base > 36) return (Integer*)Qnil;
    // Strict mode can only be invoked from Ruby via Kernel#Integer()
    // which does not allow bases other than 0.
    if(base != 0 && strict == Qtrue) return (Integer*)Qnil;

    // Skip any combination of leading whitespace and underscores.
    // Leading whitespace is OK in strict mode, but underscores are not.
    while(isspace(*str) || *str == '_') {
      if(*str == '_' && strict == Qtrue) {
        return (Integer*)Qnil;
      } else {
        str++;
      }
    }

    if(*str == '-') {
      str++;
      negative = true;
    } else if(*str == '+') {
      str++;
    }

    char chr;
    int detected_base = 0;
    const char* str_start = str;

    // Try and detect a base prefix on the front. We have to do this
    // even though we might have been told the base, because we have
    // to know if we should discard the bytes that make up the prefix
    // if it's redundent with passed in base.
    //
    // For example, if base == 16 and str == "0xa", we return
    // to return 10. But if base == 10 and str == "0xa", we fail
    // because we rewind and try to process 0x as part of the
    // base 10 string.
    //
    if(*str == '0') {
      str++;
      switch(chr = *str++) {
      case 'b': case 'B':
        detected_base = 2;
        break;
      case 'o': case 'O':
        detected_base = 8;
        break;
      case 'd': case 'D':
        detected_base = 10;
        break;
      case 'x': case 'X':
        detected_base = 16;
        break;
      default:
        // If passed "017" and a base of 0, that is octal 15.
        // Otherwise, it is whatever those digits would be in the
        // specified base.
        str--;
        detected_base = 8;
      }
    }

    // If base is less than 0, then it's just a hint for how to process it
    // if there is no base detected.
    if(base < 0) {
      if(detected_base == 0) {
        // Ok, no detected because, use the base hint and start over.
        base = -base;
        str = str_start;
      } else {
        base = detected_base;
      }

    // If 0 was passed in as the base, we use the detected base.
    } else if(base == 0) {

      // Default to 10 if there is no input and no detected base.
      if(detected_base == 0) {
        base = 10;
        str = str_start;

      } else {
        base = detected_base;
      }

    // If the passed in base and the detected base contradict
    // each other, then rewind and process the whole string as
    // digits of the passed in base.
    } else if(base != detected_base) {
      // rewind the stream, and try and consume the prefix as
      // digits in the number.
      str = str_start;
    }


    bool underscore = false;

    while(*str) {
      chr = *str++;

      // If we see space characters
      if(chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r') {

        // Eat them all
        while(chr == ' ' || chr == '\t' || chr == '\n' || chr == '\r') {
          chr = *str++;
        }

        // If there is more stuff after the spaces, get out of dodge.
        if(chr) {
          if(strict == Qtrue) {
            return (Integer*)Qnil;
          } else {
            goto return_value;
          }
        }

        break;
      }

      // If it's an underscore, remember that. An underscore is valid iff
      // it followed by a valid character for this base.
      if(chr == '_') {
        if(underscore) {
          // Double underscore is forbidden in strict mode.
          if(strict == Qtrue) {
            return (Integer*)Qnil;
          } else {
            // Stop parse number after two underscores in a row
            goto return_value;
          }
        }
        underscore = true;
        continue;
      } else {
        underscore = false;
      }

      // We use A-Z (and a-z) here so we support up to base 36.
      if(chr >= '0' && chr <= '9') {
        chr -= '0';
      } else if(chr >= 'A' && chr <= 'Z') {
        chr -= ('A' - 10);
      } else if(chr >= 'a' && chr <= 'z') {
        chr -= ('a' - 10);
      } else {
        //Invalid character, stopping right here.
        if(strict == Qtrue) {
          return (Integer*)Qnil;
        } else {
          break;
        }
      }

      // Bail if the current chr is greater or equal to the base,
      // mean it's invalid.
      if(chr >= base) {
        if(strict == Qtrue) {
          return (Integer*)Qnil;
        } else {
          break;
        }
      }

      if(value != Fixnum::from(0)) {
        if(Fixnum *fix = try_as<Fixnum>(value)) {
          value = fix->mul(state, Fixnum::from(base));
        } else {
          value = as<Bignum>(value)->mul(state, Fixnum::from(base));
        }
      }

      if(Fixnum *fix = try_as<Fixnum>(value)) {
        value = fix->add(state, Fixnum::from(chr));
      } else {
        value = as<Bignum>(value)->add(state, Fixnum::from(chr));
      }
    }

    // If we last saw an underscore and we're strict, bail.
    if(underscore && strict == Qtrue) {
      return (Integer*)Qnil;
    }

return_value:
    if(negative) {
      if(Fixnum* fix = try_as<Fixnum>(value)) {
        value = fix->neg(state);
      } else {
        value = as<Bignum>(value)->neg(state);
      }
    }

    return value;
  }

  Integer* String::to_inum_prim(STATE, Fixnum* base, Object* strict) {
    Integer* val = to_i(state, base, strict);
    if(val->nil_p()) return (Integer*)Primitives::failure();

    return val;
  }

  String* String::substring(STATE, Fixnum* start_f, Fixnum* count_f) {
    native_int start = start_f->to_native();
    native_int count = count_f->to_native();
    native_int total = num_bytes_->to_native();

    if(count < 0) return nil<String>();

    if(start < 0) {
      start += total;
      if(start < 0) return nil<String>();
    }

    if(start > total) return nil<String>();

    if(start + count > total) {
      count = total - start;
    }

    if(count < 0) count = 0;

    String* sub = String::create(state, Fixnum::from(count));
    sub->klass(state, class_object(state));

    uint8_t* buf = byte_address() + start;

    memcpy(sub->byte_address(), buf, count);

    if(tainted_p(state) == Qtrue) sub->taint(state);

    return sub;
  }

  Fixnum* String::index(STATE, String* pattern, Fixnum* start) {
    native_int total = size();
    native_int match_size = pattern->size();

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
      return (Fixnum*)Qnil;
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
      return (Fixnum*)Qnil;
    }
  }

  Fixnum* String::rindex(STATE, String* pattern, Fixnum* start) {
    native_int total = size();
    native_int match_size = pattern->size();
    native_int pos = start->to_native();

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
      return (Fixnum*)Qnil;
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
          if(memcmp(cur, matcher, match_size) == 0) return Fixnum::from(cur - buf);
          cur--;
        }
      }
      return (Fixnum*)Qnil;
    }
  }

  String* String::find_character(STATE, Fixnum* offset) {
    size_t o = (size_t)offset->to_native();
    if(o >= size()) return nil<String>();

    uint8_t* cur = byte_address() + o;

    String* output = 0;

    kcode::table* tbl = state->shared.kcode_table();
    if(kcode::mbchar_p(tbl, *cur)) {
      size_t clen = kcode::mbclen(tbl, *cur);
      if(o + clen <= size()) {
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

  String* String::base64_encode(STATE, String* str, Fixnum* line_length) {
    size_t ll = line_length->to_native();

    /* Match's MRI semantics. If the request line length is 2 or less, use
     * 45 bytes. Otherwise, round the line length to the closest multiple
     * of 3. */
    if(ll <= 2) {
      ll = 45;
    } else {
      ll = ll / 3 * 3;
    }

    uint8_t* buf = str->byte_address();
    size_t left = str->size();

    std::string out;

    while(left > 0) {
      size_t amt = (left > ll ? ll : left);
      out += base64::encode(buf, amt);
      out += "\n";

      buf += amt;
      left -= amt;
    }

    return String::create(state, out.c_str(), out.size());
  }

  String* String::base64_decode(STATE, String* str) {
    std::string out = base64::decode(str->byte_address(), str->size());
    return String::create(state, out.c_str(), out.size());
  }

  void String::Info::show(STATE, Object* self, int level) {
    String* str = as<String>(self);
    std::cout << "\"" << str->c_str() << "\"" << std::endl;
  }

  void String::Info::show_simple(STATE, Object* self, int level) {
    show(state, self, level);
  }
}

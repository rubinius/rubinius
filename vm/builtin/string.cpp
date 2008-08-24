#include "builtin/string.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/float.hpp"

#include "objects.hpp"
#include "vm.hpp"
#include "primitives.hpp"
#include "objectmemory.hpp"

#define HashPrime 16777619
#define MASK_28 (((unsigned int)1<<28)-1)

namespace rubinius {

  void String::init(STATE) {
    GO(string).set(state->new_class("String", G(object), String::fields));
    G(string)->set_object_type(StringType);
  }

  /* String::size returns the actual number of bytes, without consideration
   * for a trailing null byte.  String::create(state, "foo")->size() is 3.
   */
  size_t String::size(STATE) {
    return num_bytes->to_native();
  }

  size_t String::size() {
    return num_bytes->to_native();
  }

  String* String::allocate(STATE, FIXNUM size) {
    String *so;

    so = (String*)state->om->new_object(G(string), String::fields);

    SET(so, num_bytes, size);
    SET(so, characters, so->num_bytes);
    SET(so, encoding, Qnil);

    size_t bytes = size->to_native();
    OBJECT ba = ByteArray::create(state, bytes);
    ba->bytes[bytes] = 0;

    SET(so, data, ba);

    return so;
  }

  /* +bytes+ should NOT attempt to take the trailing null into account
   * +bytes+ is the number of 'real' characters in the string
   */
  String* String::create(STATE, const char* str, size_t bytes) {
    String *so;

    if(bytes == 0 && str) bytes = strlen(str);

    so = (String*)state->om->new_object(G(string), String::fields);

    SET(so, num_bytes, Fixnum::from(bytes));
    SET(so, characters, so->num_bytes);
    SET(so, encoding, Qnil);

    OBJECT ba = ByteArray::create(state, bytes + 1);
    if(str) memcpy(ba->bytes, str, bytes);
    ba->bytes[bytes] = 0;

    SET(so, data, ba);

    return so;
  }

  hashval String::hash_string(STATE) {
    unsigned char *bp;

    if(hash != Qnil) {
      return (hashval)as<Integer>(hash)->to_native();
    }
    bp = (unsigned char*)(data->bytes);
    size_t sz = size();

    hashval h = hash_str(bp, sz);
    SET(this, hash, Integer::from(state, h));

    return h;
  }

  hashval String::hash_str(const unsigned char *bp, unsigned int sz) {
    unsigned char *be;
    unsigned int hv;

    be = (unsigned char*)bp + sz;

    hv = 0;

    while(bp < be) {
      hv *= HashPrime;
      hv ^= *bp++;
    }
    hv = (hv>>28) ^ (hv & MASK_28);

    return hv;
  }

  SYMBOL String::to_sym(STATE) {
    return state->symbol(this);
  }

  char* String::byte_address() {
    return (char*)data->bytes;
  }

  char* String::byte_address(STATE) {
    return (char*)data->bytes;
  }

  bool String::string_equal_p(STATE, OBJECT a, OBJECT b) {
    String* self = as<String>(a);
    String* other = as<String>(b);

    if(self->num_bytes != other->num_bytes) return false;
    if(strncmp(self->byte_address(state), other->byte_address(state), self->num_bytes->to_native())) {
      return false;
    }

    return true;
  }

  OBJECT String::equal(STATE, String* other) {
    bool ret = String::string_equal_p(state, this, other);
    return ret ? Qtrue : Qfalse;
  }

  String* String::string_dup(STATE) {
    String* ns;

    ns = (String*)dup(state);
    SET(ns, shared, Qtrue);
    SET(this, shared, Qtrue);

    state->om->set_class(ns, class_object(state));

    return ns;
  }

  void String::unshare(STATE) {
    SET(this, data, as<ByteArray>(data->dup(state)));
    SET(this, shared, Qfalse);
  }

  String* String::append(STATE, String* other) {
    return append(state, other->byte_address());
  }

  String* String::append(STATE, const char* other) {
    if(shared) unshare(state);

    size_t len = strlen(other);
    size_t new_size = size() + len;

    // Leave one extra byte of room for the trailing null
    ByteArray *d2 = ByteArray::create(state, new_size + 1);
    std::memcpy(d2->bytes, data->bytes, size());
    // Append on top of the null byte at the end of s1, not after it
    std::memcpy(d2->bytes + size(), other, len);

    // This looks like it is off by one, but think about it.
    d2->bytes[new_size] = 0;

    SET(this, num_bytes, Integer::from(state, new_size));
    SET(this, data, d2);
    SET(this, hash, Qnil);

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
    char *ba = this->data->to_chars(state);
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

    /* Some implementations of strtod() don't guarantee to
     * set errno, so we need to reset it ourselves.
     */
    errno = 0;

    value = strtod(ba, &rest);
    if (errno == ERANGE) {
      printf("Float %s out of range\n", ba);
    }

    free(ba);


    return value;
  }

  // Character-wise logical AND of two strings. Modifies the receiver.
  String* String::apply_and(STATE, String* other) {
    native_int count, i;
    ByteArray* s = this->data;
    ByteArray* o = other->data;

    if(this->num_bytes > other->num_bytes) {
      count = other->num_bytes->to_native();
    } else {
      count = this->num_bytes->to_native();
    }

    // Use && not & to keep 1's in the table.
    for(i = 0; i < count; i++) {
      s->bytes[i] = s->bytes[i] && o->bytes[i];
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

  FIXNUM String::tr_expand(STATE, OBJECT limit) {
    struct tr_data data;
    native_int seq;
    native_int max;
    native_int chr;

    data.last = 0;
    data.steps = 0;

    if(Integer* lim = try_as<Integer>(limit)) {
      data.limit = lim->to_native();
    } else {
      data.limit = -1;
    }

    unsigned char* str = this->data->bytes;
    native_int bytes = (native_int)this->size();
    native_int start = bytes > 1 && str[0] == '^' ? 1 : 0;
    std::memset(data.set, -1, sizeof(native_int) * 256);

    for(native_int i = start; i < bytes;) {
      chr = str[i];
      seq = ++i < bytes ? str[i] : -1;

      if(seq == '-') {
        max = ++i < bytes ? str[i] : -1;
        if(max >= 0) {
          while(chr <= max) {
            if(data.assign(chr)) return tr_replace(state, &data);
            chr++;
          }
          i++;
        } else {
          if(data.assign(chr)) return tr_replace(state, &data);
          if(data.assign(seq)) return tr_replace(state, &data);
        }
      } else if(chr == '\\' && seq >= 0) {
        continue;
      } else {
        if(data.assign(chr)) return tr_replace(state, &data);
      }
    }

    return tr_replace(state, &data);
  }

  FIXNUM String::tr_replace(STATE, struct tr_data* data) {
    if(data->last > (native_int)this->size() || this->shared->true_p()) {
      ByteArray* ba = ByteArray::create(state, this->size() + 1);

      SET(this, data, ba);
      SET(this, shared, Qfalse);
    }

    memcpy(this->data->bytes, data->tr, data->last);
    this->data->bytes[data->last] = 0;

    SET(this, num_bytes, Fixnum::from(data->last));
    SET(this, characters, this->num_bytes);

    return Fixnum::from(data->steps);
  }

  String* String::copy_from(STATE, String* other, FIXNUM start, FIXNUM size, FIXNUM dest) {
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

    std::memcpy(this->data->bytes + dst, other->data->bytes + src, cnt);

    return this;
  }

  FIXNUM String::compare_substring(STATE, String* other, FIXNUM start, FIXNUM size) {
    native_int src = start->to_native();
    native_int cnt = size->to_native();
    native_int sz = (native_int)this->size();
    native_int osz = (native_int)other->size();

    if(src < 0) src = osz + src;
    if(src >= osz || src < 0) {
      throw PrimitiveFailed();
    }
    if(src + cnt > osz) cnt = osz - src;

    if(cnt > sz) cnt = sz;

    native_int cmp = std::memcmp(this->data->bytes, other->data->bytes + src, cnt);

    if(cmp < 0) {
      return Fixnum::from(-1);
    } else if(cmp > 0) {
      return Fixnum::from(1);
    } else {
      return Fixnum::from(0);
    }
  }

  String* String::pattern(STATE, OBJECT self, FIXNUM size, OBJECT pattern) {
    String* s = String::allocate(state, size);
    SET(s, klass, self);
    s->IsTainted = self->IsTainted;

    native_int cnt = size->to_native();

    if(FIXNUM chr = try_as<Fixnum>(pattern)) {
      std::memset(s->data->bytes, (int)chr->to_native(), cnt);
    } else if(String* pat = try_as<String>(pattern)) {
      s->IsTainted |= pat->IsTainted;

      native_int psz = pat->size();
      if(psz == 1) {
        std::memset(s->data->bytes, pat->data->bytes[0], cnt);
      } else if(psz > 1) {
        native_int i, j, n;

        native_int sz = cnt / psz;
        for(n = i = 0; i < sz; i++) {
          for(j = 0; j < psz; j++, n++) {
            s->data->bytes[n] = pat->data->bytes[j];
          }
        }
        for(i = n, j = 0; i < cnt; i++, j++) {
          s->data->bytes[i] = pat->data->bytes[j];
        }
      }
    } else {
      throw PrimitiveFailed();
    }

    return s;
  }

  void String::Info::show(STATE, OBJECT self) {
    String* str = as<String>(self);
    std::cout << str->byte_address() << std::endl;
  }

}

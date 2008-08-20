#include "builtin/string.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/symbol.hpp"
#include "builtin/float.hpp"

#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#define HashPrime 16777619
#define MASK_28 (((unsigned int)1<<28)-1)

namespace rubinius {

  size_t String::size(STATE) {
    return num_bytes->to_native();
  }

  size_t String::size() {
    return num_bytes->to_native();
  }

  /* TODO: since we're technically say it's ok to change this, we might
   * want to copy it first. */
  String::operator char *() {
    return (char*)(data->bytes);
  }

  String* String::allocate(STATE, FIXNUM size) {
    String *so;
    size_t bytes = size->to_native();

    so = (String*)state->om->new_object(G(string), String::fields);

    SET(so, num_bytes, Fixnum::from(bytes));
    SET(so, characters, so->num_bytes);
    SET(so, encoding, Qnil);

    OBJECT ba = ByteArray::create(state, bytes);
    ba->bytes[bytes] = 0;

    SET(so, data, ba);

    return so;
  }

  String* String::create(STATE, const char* str, size_t bytes) {
    String *so;

    if(!bytes) bytes = strlen(str);

    so = (String*)state->om->new_object(G(string), String::fields);

    SET(so, num_bytes, Fixnum::from(bytes));
    SET(so, characters, so->num_bytes);
    SET(so, encoding, Qnil);

    OBJECT ba = ByteArray::create(state, bytes);
    if(str) {
      memcpy(ba->bytes, str, bytes);
      ba->bytes[bytes] = 0;
    }

    SET(so, data, ba);

    return so;
  }

  hashval String::hash_string(STATE) {
    unsigned char *bp;

    if(hash != Qnil) {
      return (hashval)as<Integer>(hash)->to_native();
    }
    bp = (unsigned char*)(data->bytes);
    size_t sz = size(state);

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
    if(shared) unshare(state);

    size_t new_size = size() + other->size();

    ByteArray *d2 = ByteArray::create(state, new_size + 1);
    std::memcpy(d2->bytes, data->bytes, size());
    std::memcpy(d2->bytes + size(), other->data->bytes, other->size());

    d2->bytes[new_size] = 0;

    num_bytes = Integer::from(state, new_size);
    SET(this, data, d2);
    SET(this, hash, Qnil);

    return this;
  }

  String* String::append(STATE, const char* other) {
    if(shared) unshare(state);

    size_t len = strlen(other);
    size_t new_size = size() + len;

    ByteArray *d2 = ByteArray::create(state, new_size + 1);
    std::memcpy(d2->bytes, data->bytes, size());
    std::memcpy(d2->bytes + size(), other, len);

    d2->bytes[new_size] = 0;

    num_bytes = Integer::from(state, new_size);
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

  void String::Info::show(STATE, OBJECT self) {
    String* str = as<String>(self);
    std::cout << *str << std::endl;
  }

}

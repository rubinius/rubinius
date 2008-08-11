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
    return num_bytes->n2i();
  }

  size_t String::size() {
    return num_bytes->n2i();
  }

  /* TODO: since we're technically say it's ok to change this, we might
   * want to copy it first. */
  String::operator char *() {
    return (char*)(data->bytes);
  }

  String* String::allocate(STATE, FIXNUM size) {
    String *so;
    size_t bytes = size->n2i();

    so = (String*)state->om->new_object(G(string), String::fields);

    so->num_bytes = Fixnum::from(bytes);
    so->characters = so->num_bytes;
    so->encoding = Qnil;

    OBJECT ba = ByteArray::create(state, bytes);
    ba->bytes[bytes] = 0;

    SET(so, data, ba);

    return so;
  }

  String* String::create(STATE, const char* str, size_t bytes) {
    String *so;

    if(!bytes) bytes = strlen(str);

    so = (String*)state->om->new_object(G(string), String::fields);

    so->num_bytes = Fixnum::from(bytes);
    so->characters = so->num_bytes;
    so->encoding = Qnil;

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
      return (hashval)as<Integer>(hash)->n2i();
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
    if(strncmp(self->byte_address(state), other->byte_address(state), self->num_bytes->n2i())) {
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
    char* s = this->data->to_chars(state);
    char* o = other->data->to_chars(state);

    if(this->num_bytes > other->num_bytes) {
      count = other->num_bytes->n2i();
    } else {
      count = this->num_bytes->n2i();
    }

    for(i = 0; i < count; i++) {
      s[i] = s[i] && o[i];
    }
    ByteArray* ba = ByteArray::create(state, count);

    std::memcpy(ba->bytes, s, count);
    ba->bytes[count] = 0;

    SET(this, data, ba);

    return this;
  }

  void String::Info::show(STATE, OBJECT self) {
    String* str = as<String>(self);
    std::cout << *str << std::endl;
  }

}

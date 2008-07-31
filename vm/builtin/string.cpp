#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "builtin/bytearray.hpp"

#define HashPrime 16777619
#define MASK_28 (((unsigned int)1<<28)-1)

namespace rubinius {

  String* String::allocate(STATE, FIXNUM size) {
    String *so;
    size_t bytes = size->n2i();

    so = (String*)state->om->new_object(G(string), String::fields);

    so->num_bytes = Object::i2n(bytes);
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

    so->num_bytes = Object::i2n(bytes);
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
    SET(this, hash, Object::ui2n(state, h));

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

  // Ruby.primitive :symbol_lookup
  SYMBOL String::to_sym(STATE) {
    return G(symbols)->lookup(state, this);
  }

  char* String::byte_address(STATE) {
    return (char*)data->bytes;
  }

  int String::string_equal_p(STATE, OBJECT a, OBJECT b) {
    String* self = as<String>(a);
    String* other = as<String>(b);

    if(self->bytes != other->bytes) return FALSE;
    if(strcmp(self->byte_address(state), other->byte_address(state))) {
      return FALSE;
    }

    return TRUE;
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

    num_bytes = Object::i2n(state, new_size);
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

    num_bytes = Object::i2n(state, new_size);
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

}

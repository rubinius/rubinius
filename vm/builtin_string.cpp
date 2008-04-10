#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#define HashPrime 16777619
#define MASK_28 (((unsigned int)1<<28)-1)

namespace rubinius {

  String* String::create(STATE, const char* str, size_t bytes) {
    String *so;

    if(!bytes) bytes = strlen(str);

    so = (String*)state->om->new_object(state->globals.string, String::fields);

    so->num_bytes = Object::i2n(bytes);
    so->characters = so->num_bytes;
    so->encoding = Qnil;

    size_t fields = (bytes + 1) / 4;
    if(fields % sizeof(OBJECT) != 0) fields++;

    OBJECT ba = state->om->new_object(state->globals.bytearray, fields);
    memset(ba->bytes, 0, ba->size_in_bytes());
    memcpy(ba->bytes, str, bytes);
    ba->bytes[bytes] = 0;

    SET(so, data, ba);

    return so;
  }

  hashval String::hash_string(STATE) {
    unsigned char *bp;

    if(hash != Qnil) {
      return (hashval)hash->n2i();
    }
    bp = (unsigned char*)(data->bytes);
    size_t sz = size(state);

    hashval h = hash_str(bp, sz);
    hash = Object::ui2n(state, h);

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

  OBJECT String::to_sym(STATE) {
    return state->globals.symbols->lookup(state, this);
  }

  char* String::byte_address(STATE) {
    return (char*)data->bytes;
  }
  
  int String::string_equal_p(STATE, OBJECT a, OBJECT b) {
    String* self = (String*)a;
    String* other = (String*)b;

    if(self->bytes != other->bytes) return FALSE;
    if(strcmp(self->byte_address(state), other->byte_address(state))) {
      return FALSE;
    }

    return TRUE;
  }

  String* String::string_dup(STATE) {
    String* ns;

    ns = (String*)dup(state);
    ns->shared = Qtrue;
    shared = Qtrue;

    state->om->set_class(ns, class_object());

    return ns;
  }

}

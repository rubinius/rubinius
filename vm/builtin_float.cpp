#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

namespace rubinius {
  Float* Float::create(STATE, double val) {
    Float* flt = (Float*)state->new_struct(G(floatpoint), sizeof(Float));
    flt->val = val;
    return flt;
  }

  Float* Float::coerce(STATE, OBJECT value) {
    if(value->fixnum_p()) {
      return Float::create(state, (double)(as<Fixnum>(value)->to_nint()));
    } else if(kind_of<Bignum>(value)) {
      return Float::create(state, as<Bignum>(value)->to_double(state));
    }

    /* FIXME this used to just return value, but this wants to coerce, so
     * we give a default float value of 0.0 back instead. */
    return Float::create(state, 0.0);
  }

  void Float::into_string(STATE, char* buf, size_t sz) {
    snprintf(buf, sz, "%+.17e", val);
  }
}

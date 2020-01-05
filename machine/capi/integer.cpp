#include "class/string.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
  /* This is a really wacky MRI API, which is actually marked internal
   * despite being used in external code. The existing Fixnum#size and
   * Bignum#size methods are sufficient for the 3-4 places it's used in MRI's
   * code itself, where the 2nd argument isn't even used.
   *
   * We provide a "good enough" implementation.
   */
  size_t rb_absint_size(VALUE value, int* nlz_bits) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    size_t size = 0;

    if(RB_TYPE_P(value, T_FIXNUM)) {
      if(value > 0) {
        size = FIXNUM_MAX_WIDTH;
      } else {
        size = FIXNUM_MIN_WIDTH;
      }
    } else if(RB_TYPE_P(value, T_BIGNUM)) {
      Bignum* big = MemoryHandle::object<Bignum>(value);
      size = big->size(env->state())->to_native();
    }

    // Within bounds
    if(nlz_bits) *nlz_bits = 0;

    return size;
  }

  int rb_integer_pack(VALUE value, void *words, size_t numwords, size_t wordsize,
                      size_t nails, int flags)
  {
    if(nails != 0) {
      rb_raise(rb_eNotImpError, "non-zero bit padding is unsupported");
    }

    if(((flags & INTEGER_PACK_MSWORD_FIRST) != 0 &&
        (flags & INTEGER_PACK_LSBYTE_FIRST) != 0) ||
       ((flags & INTEGER_PACK_LSWORD_FIRST) != 0 &&
        (flags & INTEGER_PACK_MSBYTE_FIRST) != 0)) {
      rb_raise(rb_eNotImpError, "mixed byte and word ordering is unsupported");
    }

    value = rb_to_int(value);

    size_t numbytes = numwords * wordsize;
    memset(words, 0, numbytes);

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();

    if(Fixnum* f = MemoryHandle::try_as<Fixnum>(value)) {
      intptr_t v = f->to_native();

      if(v == 0) return 0;

      if((numbytes >= FIXNUM_MIN_WIDTH / 8) &&
          ((flags & INTEGER_PACK_NATIVE_BYTE_ORDER) ||
#ifdef RBX_LITTLE_ENDIAN
          ((flags & INTEGER_PACK_LSWORD_FIRST) &&
          (flags & INTEGER_PACK_LSBYTE_FIRST))
#else
          ((flags & INTEGER_PACK_MSWORD_FIRST) &&
          (flags & INTEGER_PACK_MSBYTE_FIRST))
#endif
          ))
      {
        memcpy(words, &v, numbytes);
        return v < 0 ? -1 : 1;
      }
    }

    String* bytes = String::create_pinned(env->state(), Fixnum::from(numbytes));
    memset(bytes->byte_address(), 0, numbytes);

    VALUE result = rb_funcall(rb_mCAPI, rb_intern("rb_integer_pack"), 6,
        value, MemoryHandle::value(bytes), INT2FIX(numwords), INT2FIX(wordsize),
        INT2FIX(nails), INT2FIX(flags));

    memcpy(words, bytes->byte_address(), numbytes);

    return FIX2INT(result);
  }
}

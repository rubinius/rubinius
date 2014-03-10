#include "builtin/string.hpp"

#include "capi/capi.hpp"
#include "capi/ruby.h"

using namespace rubinius;
using namespace rubinius::capi;

extern "C" {
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

    if(Fixnum* f = try_as<Fixnum>(env->get_object(value))) {
      native_int v = f->to_native();

      if(v == 0) return 0;

      if((numbytes >= FIXNUM_WIDTH / 8) &&
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

    String* bytes = String::create(env->state(), Fixnum::from(numbytes));
    memset(bytes->byte_address(), 0, numbytes);

    VALUE result = rb_funcall(rb_mCAPI, rb_intern("rb_integer_pack"), 6,
        value, env->get_handle(bytes), INT2FIX(numwords), INT2FIX(wordsize),
        INT2FIX(nails), INT2FIX(flags));

    memcpy(words, bytes->byte_address(), numbytes);

    return FIX2INT(result);
  }
}

#include <cctype>
#include <cstring>
#include <cmath>
#include <iomanip>

#include "objectmemory.hpp"
#include "marshal.hpp"

#include "object_utils.hpp"

#include "tommath.h"
#include "gdtoa.h"

#include "builtin/sendsite.hpp"
#include "builtin/array.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/iseq.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"


namespace rubinius {

  using std::endl;

  unsigned long UnMarshaller::get_varint(bool* done) {
    unsigned long val = 0;
    unsigned int bytes = 0;
    int shift = 0;

    *done = false;

    while(!*done && bytes < sizeof(long)) {
      unsigned int byte = stream.get();

      val += (byte & ~128) << shift;

      shift += 7;
      bytes++;

      *done = byte < 128;
    }

    return val;
  }

  unsigned long UnMarshaller::get_varint() {
    bool done;
    unsigned long val = get_varint(&done);

    if(!done) {
      std::ostringstream msg;

      msg << "Unable to unmarshal variable length integer: exceeds " << sizeof(long) << " bytes";

      Exception::type_error(state, msg.str().c_str());
    }

    return val;
  }

  Object* UnMarshaller::get_positive_varint() {
    bool done;
    unsigned long val = get_varint(&done);

    bool is_fixnum = done;
    mp_int mp_val;

    // do we need to switch to bignum math?
    if(!done) {
      int shift = sizeof(long) * 7;
      mp_int a;

      mp_init_set_int(&mp_val, val);
      mp_init(&a);

      while(!done) {
        unsigned int byte = stream.get();

        mp_set_int(&a, byte & ~128);
        mp_mul_2d(&a, shift, &a);
        mp_add(&a, &mp_val, &mp_val);

        shift += 7;
        done = byte < 128;
      }

      mp_clear(&a);
    }

    Integer *result;

    if(is_fixnum) {
      result = Integer::from(state, val);
    } else {
      result = Bignum::from(state, &mp_val);

      mp_clear(&mp_val);
    }

    return result;
  }

  Object* UnMarshaller::get_negative_varint() {
    Object* result = get_positive_varint();

    // negate:
    if(Bignum* bignum = try_as<Bignum>(result)) {
      mp_neg (bignum->mp_val(), bignum->mp_val());
      return bignum;
    } else {
      return as<Fixnum>(result)->neg(state);
    }
  }

  String* UnMarshaller::get_string() {
    size_t count = get_varint();

    // String::create adds room for a trailing null on its own
    String* str = String::create(state, NULL, count);

    stream.read(str->byte_address(), count);

    return str;
  }

  Symbol* UnMarshaller::get_symbol() {
    char data[1024];
    size_t count = get_varint();

    stream.read(data, count);
    data[count] = 0; // clamp

    return state->symbol(data);
  }

  SendSite* UnMarshaller::get_sendsite() {
    char data[1024];
    size_t count = get_varint();

    stream.read(data, count);
    data[count] = 0; // clamp

    Symbol* sym = state->symbol(data);

    return SendSite::create(state, sym);
  }

  Tuple* UnMarshaller::get_tuple() {
    size_t count = get_varint();

    Tuple* tup = Tuple::create(state, count);

    for(size_t i = 0; i < count; i++) {
      tup->put(state, i, unmarshal());
    }

    return tup;
  }

#define FLOAT_EXP_OFFSET    58

  Float* UnMarshaller::get_float() {
    char data[1024];

    stream.getline(data, 1024);
    if(stream.fail()) {
      Exception::type_error(state, "Unable to unmarshal Float: failed to read value");
    }

    if(data[0] == ' ') {
      double x;
      long   e;

      x = ::ruby_strtod(data, NULL);
      e = strtol(data+FLOAT_EXP_OFFSET, NULL, 10);

      // This is necessary because exp2(1024) yields inf
      if(e == 1024) {
        double root_exp = ::exp2(512);
        return Float::create(state, x * root_exp * root_exp);
      } else {
        return Float::create(state, x * ::exp2(e));
      }
    } else {
      // avoid compiler warning
      double zero = 0.0;
      double val = 0.0;

      if(!strncasecmp(data, "Infinity", 8U)) {
        val = 1.0;
      } else if(!strncasecmp(data, "-Infinity", 9U)) {
        val = -1.0;
      } else if(!strncasecmp(data, "NaN", 3U)) {
        val = zero;
      } else {
        Exception::type_error(state, "Unable to unmarshal Float: invalid format");
      }

      return Float::create(state, val / zero);
    }
  }

  InstructionSequence* UnMarshaller::get_iseq() {
    size_t count = get_varint();

    InstructionSequence* iseq = InstructionSequence::create(state, count);
    Tuple* ops = iseq->opcodes();

    for(size_t i = 0; i < count; i++) {
      unsigned long op = get_varint();

      ops->put(state, i, Fixnum::from(op));
    }

    iseq->post_marshal(state);

    return iseq;
  }

  CompiledMethod* UnMarshaller::get_cmethod() {
    /* size_t version = */ get_varint();

    CompiledMethod* cm = CompiledMethod::create(state);

    cm->ivars(state, unmarshal());
    cm->primitive(state, (Symbol*)unmarshal());
    cm->name(state, (Symbol*)unmarshal());
    cm->iseq(state, (InstructionSequence*)unmarshal());
    cm->stack_size(state, (Fixnum*)unmarshal());
    cm->local_count(state, (Fixnum*)unmarshal());
    cm->required_args(state, (Fixnum*)unmarshal());
    cm->total_args(state, (Fixnum*)unmarshal());
    cm->splat(state, unmarshal());
    cm->literals(state, (Tuple*)unmarshal());
    cm->exceptions(state, (Tuple*)unmarshal());
    cm->lines(state, (Tuple*)unmarshal());
    cm->file(state, (Symbol*)unmarshal());
    cm->local_names(state, (Tuple*)unmarshal());

    cm->post_marshal(state);

    return cm;
  }

  Object* UnMarshaller::unmarshal() {
    char code = stream.get();

    switch(code) {
    case 'n':
      return Qnil;
    case 't':
      return Qtrue;
    case 'f':
      return Qfalse;
    case 'I':
      return get_positive_varint();
    case 'J':
      return get_negative_varint();
    case 's':
      return get_string();
    case 'x':
      return get_symbol();
    case 'S':
      return get_sendsite();
    case 'p':
      return get_tuple();
    case 'd':
      return get_float();
    case 'i':
      return get_iseq();
    case 'M':
      return get_cmethod();
    default:
      std::string str = "unknown marshal code: ";
      str.append( 1, code );
      Exception::type_error(state, str.c_str());
      return Qnil;    // make compiler happy
    }
  }
}

#include <cctype>
#include <cstring>
#include <cmath>
#include <iomanip>

#include "objectmemory.hpp"
#include "marshal.hpp"

#include "object_utils.hpp"

#include <tommath.h>
#include <gdtoa.h>

#include "builtin/array.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/iseq.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"

#include "detection.hpp"

namespace rubinius {

  using std::endl;

  Object* UnMarshaller::get_int() {
    char data[1024];

    stream >> data;
    data[sizeof(data) - 1] = '\0';

    return Bignum::from_string(state, data, 16);
  }

  String* UnMarshaller::get_string() {
    size_t count;

    stream >> count;
    // String::create adds room for a trailing null on its own
    // using pinned here allows later stages to optimize these literal
    // strings better.
    String* str = String::create(state, NULL, count);

    stream.get(); // read off newline
    stream.read(reinterpret_cast<char*>(str->byte_address()), count);
    stream.get(); // read off newline

    return str;
  }

  Symbol* UnMarshaller::get_symbol() {
    char data[1024];
    size_t count;

    stream >> count;
    stream.get();
    stream.read(data, count + 1);
    data[count] = 0; // clamp

    return state->symbol(data);
  }

  Tuple* UnMarshaller::get_tuple() {
    size_t count;
    stream >> count;

    Tuple* tup = Tuple::create(state, count);

    for(size_t i = 0; i < count; i++) {
      tup->put(state, i, unmarshal());
    }

    return tup;
  }

#define FLOAT_EXP_OFFSET    58

  Float* UnMarshaller::get_float() {
    char data[1024];

    // discard the delimiter
    stream.get();

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
    size_t count;
    long op;
    stream >> count;

    InstructionSequence* iseq = InstructionSequence::create(state, count);
    Tuple* ops = iseq->opcodes();

    for(size_t i = 0; i < count; i++) {
      stream >> op;
      ops->put(state, i, Fixnum::from(op));
    }

    iseq->post_marshal(state);

    return iseq;
  }

  CompiledMethod* UnMarshaller::get_cmethod() {
    size_t ver;
    stream >> ver;

    CompiledMethod* cm = CompiledMethod::create(state);

    cm->metadata(state, unmarshal());
    cm->primitive(state, (Symbol*)unmarshal());
    cm->name(state, (Symbol*)unmarshal());
    cm->iseq(state, (InstructionSequence*)unmarshal());
    cm->stack_size(state, (Fixnum*)unmarshal());
    cm->local_count(state, (Fixnum*)unmarshal());
    cm->required_args(state, (Fixnum*)unmarshal());
    cm->total_args(state, (Fixnum*)unmarshal());
    cm->splat(state, unmarshal());
    cm->literals(state, (Tuple*)unmarshal());
    cm->lines(state, (Tuple*)unmarshal());
    cm->file(state, (Symbol*)unmarshal());
    cm->local_names(state, (Tuple*)unmarshal());

    cm->post_marshal(state);

    return cm;
  }

  Object* UnMarshaller::unmarshal() {
    char code;

    stream >> code;

    switch(code) {
    case 'n':
      return Qnil;
    case 't':
      return Qtrue;
    case 'f':
      return Qfalse;
    case 'I':
      return get_int();
    case 's':
      return get_string();
    case 'x':
      return get_symbol();
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

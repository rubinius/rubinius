#include <ctype.h>
#include <string.h>
#include <math.h>
#include <iomanip>

#include "objectmemory.hpp"
#include "marshal.hpp"

#include "object_utils.hpp"

#include <tommath.h>
#include <double-conversion.h>
#include <ieee.h>

#include "builtin/array.hpp"
#include "builtin/compiledcode.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/iseq.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"

#include "detection.hpp"

#define STACK_BUF_SZ 1024

namespace rubinius {

  Object* UnMarshaller::get_constant() {
    char stack_data[STACK_BUF_SZ];
    char *malloc_data = NULL;
    char *data = stack_data;

    size_t count;

    stream >> count;
    stream.get();

    if(count >= STACK_BUF_SZ) {
      malloc_data = (char*)malloc(count + 1);
      data = malloc_data;
    }

    stream.read(data, count + 1);
    data[count] = 0; // clamp

    Object* cls = state->vm()->path2class(data);

    if(malloc_data) {
      free(malloc_data);
    }
    return cls;
  }

  Object* UnMarshaller::get_encoding() {
    char stack_data[STACK_BUF_SZ];
    char *malloc_data = NULL;
    char *data = stack_data;
    size_t count;

    stream >> count;
    stream.get();

    if(count >= STACK_BUF_SZ) {
      malloc_data = (char*)malloc(count + 1);
      data = malloc_data;
    }

    stream.read(data, count + 1);
    data[count] = 0; // clamp

    if(count > 0) {
      Encoding* enc = Encoding::find(state, data);
      if(malloc_data) {
        free(malloc_data);
      }
      return enc;
    } else {
      return cNil;
    }
  }

  Object* UnMarshaller::get_int() {
    std::string data;
    stream >> data;

    return Bignum::from_string(state, data.c_str(), 16);
  }

  String* UnMarshaller::get_string() {
    size_t count;

    Encoding* enc = try_as<Encoding>(unmarshal());

    stream >> count;
    // String::create adds room for a trailing null on its own
    // using pinned here allows later stages to optimize these literal
    // strings better.
    String* str = String::create(state, NULL, count);

    stream.get(); // read off newline
    stream.read(reinterpret_cast<char*>(str->byte_address()), count);
    stream.get(); // read off newline

    if(enc) str->encoding(state, enc);

    return str;
  }

  Symbol* UnMarshaller::get_symbol() {
    char stack_data[STACK_BUF_SZ];
    char *malloc_data = NULL;
    char *data = stack_data;
    size_t count;

    stream >> count;
    stream.get();

    if(count >= STACK_BUF_SZ) {
      malloc_data = (char*)malloc(count + 1);
      data = malloc_data;
    }

    stream.read(data, count + 1);
    data[count] = 0; // clamp

    Symbol* sym = state->symbol(data, count);

    if(malloc_data) {
      free(malloc_data);
    }

    return sym;
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
    std::string line;
    const char* data;

    // discard the delimiter
    stream.get();

    std::getline(stream, line);
    if(stream.fail()) {
      Exception::type_error(state, "Unable to unmarshal Float: failed to read value");
    }

    data = line.c_str();
    if(data[0] == ' ') {
      double x;
      long   e;

      int flags = double_conversion::StringToDoubleConverter::ALLOW_LEADING_SPACES |
          double_conversion::StringToDoubleConverter::ALLOW_SPACES_AFTER_SIGN |
          double_conversion::StringToDoubleConverter::ALLOW_TRAILING_SPACES;
      double_conversion::StringToDoubleConverter sd(flags, 0.0, double_conversion::Double::NaN(), NULL, NULL);

      int processed;
      x = sd.StringToDouble(data, FLOAT_EXP_OFFSET - 1, &processed);
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

#define OPCODE_LENGTH 32

  InstructionSequence* UnMarshaller::get_iseq() {
    size_t count;
    char data[OPCODE_LENGTH];
    stream >> count;

    // Read off newline
    stream.get();

    InstructionSequence* iseq = InstructionSequence::create(state, count);
    Tuple* ops = iseq->opcodes();

    for(size_t i = 0; i < count; i++) {
      stream.getline(data, OPCODE_LENGTH);
      long op = strtol(data, NULL, 10);
      ops->put(state, i, Fixnum::from(op));
    }

    iseq->post_marshal(state);

    return iseq;
  }

  CompiledCode* UnMarshaller::get_compiled_code() {
    size_t ver;
    stream >> ver;

    CompiledCode* code = CompiledCode::create(state);

    code->metadata(state, unmarshal());
    code->primitive(state, force_as<Symbol>(unmarshal()));
    code->name(state, force_as<Symbol>(unmarshal()));
    code->iseq(state, force_as<InstructionSequence>(unmarshal()));
    code->stack_size(state, force_as<Fixnum>(unmarshal()));
    code->local_count(state, force_as<Fixnum>(unmarshal()));
    code->required_args(state, force_as<Fixnum>(unmarshal()));
    code->post_args(state, force_as<Fixnum>(unmarshal()));
    code->total_args(state, force_as<Fixnum>(unmarshal()));
    code->splat(state, force_as<Fixnum>(unmarshal()));
    code->literals(state, force_as<Tuple>(unmarshal()));
    code->lines(state, force_as<Tuple>(unmarshal()));
    code->file(state, force_as<Symbol>(unmarshal()));
    code->local_names(state, force_as<Tuple>(unmarshal()));

    code->post_marshal(state);

    return code;
  }

  Object* UnMarshaller::unmarshal() {
    char code;

    stream >> code;

    switch(code) {
    case 'n':
      return cNil;
    case 't':
      return cTrue;
    case 'f':
      return cFalse;
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
      return get_compiled_code();
    case 'c':
      return get_constant();
    case 'E':
      return get_encoding();
    default:
      std::string str = "unknown marshal code: ";
      str.append( 1, code );
      Exception::type_error(state, str.c_str());
      return cNil;    // make compiler happy
    }
  }
}

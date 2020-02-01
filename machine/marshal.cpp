#include <ctype.h>
#include <string.h>
#include <math.h>
#include <iomanip>

#include "arguments.hpp"
#include "dispatch.hpp"
#include "memory.hpp"
#include "marshal.hpp"
#include "on_stack.hpp"

#include "object_utils.hpp"

#include <tommath.h>
#include <double-conversion.h>
#include <ieee.h>

#include "class/array.hpp"
#include "class/class.hpp"
#include "class/compiled_code.hpp"
#include "class/encoding.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/float.hpp"
#include "class/iseq.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"

#include "missing/math.h"

#include "detection.hpp"

#define STACK_BUF_SZ 1024

namespace rubinius {
  void Marshaller::marshal() {
    put_compiled_code(code);
  }

  void Marshaller::marshal(Object* obj) {
    OnStack<1> os(state, obj);

    if(obj->true_p()) {
      stream << "t\n";
    } else if(obj->false_p()) {
      stream << "f\n";
    } else if(obj->nil_p()) {
      stream << "n\n";
    } else if(obj->fixnum_p()) {
      put_fixnum(obj);
    } else if(Bignum* n = try_as<Bignum>(obj)) {
      stream << "I" << n->to_s(state, Fixnum::from(16))->c_str(state) << "\n";
    } else if(String* s = try_as<String>(obj)) {
      put_string("s", s);
    } else if(Symbol* s = try_as<Symbol>(obj)) {
      put_string("x", s->to_str(state));
    } else if(Tuple* t = try_as<Tuple>(obj)) {
      stream << "p\n";
      for(int i = 0; i < t->num_fields(); i++) {
        marshal(t->at(i));
      }
    } else if(Float* f = try_as<Float>(obj)) {
      stream << "d\n";
      if(isinf(f->value())) {
        if(f->value() < 0.0) stream << "-";
        stream << "Infinity";
      } else if(isnan(f->value())) {
        stream << "NaN";
      } else {
#define MARSHAL_FLOAT_BUFLEN  65
        int e;
        double v = frexp(f->value(), &e);
        char buf[MARSHAL_FLOAT_BUFLEN];

        snprintf(buf, MARSHAL_FLOAT_BUFLEN, " %+.54f %5d", v, e);
        stream << buf;
      }
      stream << "\n";
    } else if(obj->kind_of_p(state, G(object)->get_const(state, "Rational"))) {
      put_rational(obj);
    } else if(obj->kind_of_p(state, G(object)->get_const(state, "Complex"))) {
      put_complex(obj);
    } else if(InstructionSequence* iseq = try_as<InstructionSequence>(obj)) {
      intptr_t size = iseq->opcodes()->num_fields();

      stream << "i\n" << size << "\n";

      for(int i = 0; i < size; i++) {
        Object* op = iseq->opcodes()->at(i);

        if(!op->fixnum_p()) {
          std::string str =  "InstructionSequence contains non Fixnum: ";
          str.append(op->to_string(state, true));

          Exception::raise_type_error(state, str.c_str());
        }

        put_fixnum(op);
      }
    } else if(CompiledCode* c = try_as<CompiledCode>(obj)) {
      put_compiled_code(c);
    } else {
      if(CBOOL(obj->respond_to(state, state->symbol("rbx_marshal_constant"), G(sym_public)))) {
        put_constant(obj);
      } else {
        std::string str = "unknown type: ";
        str.append(obj->class_object(state)->module_name()->cpp_str(state));

        Exception::raise_type_error(state, str.c_str());
      }
    }
  }

  void Marshaller::put_fixnum(Object* obj) {
    stream << "I" << std::hex << as<Fixnum>(obj)->to_native() << "\n";
  }

  void Marshaller::put_string(const char* type, String* str) {
    String* enc = str->encoding(state)->name();
    stream << type << "\nE\n"
           << enc->byte_size() << "\n"
           << enc->c_str(state) << "\n"
           << str->byte_size() << "\n"
           << str->c_str(state) << "\n";
  }

  void Marshaller::put_rational(Object* obj) {
    Symbol* name = state->symbol("rbx_marshal_rational");
    Arguments args(name, G(runtime), obj);
    Dispatch dispatch(name);

    Tuple* tuple;
    OnStack<1> os(state, tuple);

    Object* result = dispatch.send(state, args);

    if(result && (tuple = try_as<Tuple>(result))) {
      if(tuple->num_fields() == 2) {
        marshal(tuple->at(0));
        marshal(tuple->at(1));
      }
    }
  }

  void Marshaller::put_complex(Object* obj) {
    Symbol* name = state->symbol("rbx_marshal_rational");
    Arguments args(name, G(runtime), obj);
    Dispatch dispatch(name);

    Tuple* tuple;
    OnStack<1> os(state, tuple);

    Object* result = dispatch.send(state, args);

    if(result && (tuple = try_as<Tuple>(result))) {
      if(tuple->num_fields() == 2) {
        marshal(tuple->at(0));
        marshal(tuple->at(1));
      }
    }
  }

  void Marshaller::put_constant(Object* obj) {
    Symbol* name = state->symbol("rbx_marshal_rational");
    Arguments args(name, G(runtime), obj);
    Dispatch dispatch(name);

    if(Object* result = dispatch.send(state, args)) {
      marshal(result);
    }
  }

  void Marshaller::put_compiled_code(CompiledCode* code) {
    stream << "M\n1\n";
    marshal(code->metadata());
    marshal(code->primitive());
    marshal(code->name());
    marshal(code->iseq());
    marshal(code->stack_size());
    marshal(code->local_count());
    marshal(code->required_args());
    marshal(code->post_args());
    marshal(code->total_args());
    marshal(code->splat());
    marshal(code->keywords());
    marshal(code->arity());
    marshal(code->literals());
    marshal(code->lines());
    marshal(code->file());
    marshal(code->local_names());
  }

  Object* UnMarshaller::get_constant() {
    char stack_data[STACK_BUF_SZ];
    char *malloc_data = NULL;
    char *data = stack_data;

    size_t count;

    stream >> count;
    stream.get();

    if(count >= STACK_BUF_SZ) {
      malloc_data = (char*)malloc(count + 1);
      if(!malloc_data) rubinius::bug();
      data = malloc_data;
    }

    stream.read(data, count + 1);
    data[count] = 0; // clamp

    Object* cls = state->path2class(state, data);

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
      if(!malloc_data) rubinius::bug();
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

    return Integer::from_cppstr(state, data, 16);
  }

  Object* UnMarshaller::get_rational() {
    Object* a;
    Object* b;
    OnStack<2> os(state, a, b);

    a = unmarshal();
    b = unmarshal();

    Symbol* name = state->symbol("unmarshal_rational");
    Object* objs[2] = { a, b };

    Arguments args(name, G(runtime), 2, objs);
    Dispatch dispatch(name);

    if(Object* r = dispatch.send(state, args)) {
      return r;
    } else {
      return cNil;
    }
  }

  Object* UnMarshaller::get_complex() {
    Object* a;
    Object* b;
    OnStack<2> os(state, a, b);

    a = unmarshal();
    b = unmarshal();

    Symbol* name = state->symbol("unmarshal_complex");
    Object* objs[2] = { a, b };

    Arguments args(name, G(runtime), 2, objs);
    Dispatch dispatch(name);

    if(Object* c = dispatch.send(state, args)) {
      return c;
    } else {
      return cNil;
    }
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

    Encoding* enc = try_as<Encoding>(unmarshal());

    stream >> count;
    stream.get();

    if(count >= STACK_BUF_SZ) {
      malloc_data = (char*)malloc(count + 1);
      if(!malloc_data) rubinius::bug();
      data = malloc_data;
    }

    stream.read(data, count + 1);
    data[count] = 0; // clamp

    String* str = String::create(state, data, count);
    if(enc) str->encoding(state, enc);

    Symbol* sym = state->symbol(str);

    if(malloc_data) {
      free(malloc_data);
    }

    return sym;
  }

  Tuple* UnMarshaller::get_tuple() {
    size_t count;
    stream >> count;

    Tuple* tup = Tuple::create(state, count);
    OnStack<1> os(state, tup);

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
      Exception::raise_type_error(state, "Unable to unmarshal Float: failed to read value");
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
        Exception::raise_type_error(state, "Unable to unmarshal Float: invalid format");
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
    OnStack<2> os(state, iseq, ops);

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
    OnStack<1> os(state, code);

    code->metadata(state, unmarshal());
    code->primitive(state, force_as<Symbol>(unmarshal()));
    code->name(state, force_as<Symbol>(unmarshal()));
    code->iseq(state, force_as<InstructionSequence>(unmarshal()));
    code->stack_size(state, force_as<Fixnum>(unmarshal()));
    code->local_count(state, force_as<Fixnum>(unmarshal()));
    code->registers(state, force_as<Fixnum>(unmarshal()));
    code->required_args(state, force_as<Fixnum>(unmarshal()));
    code->post_args(state, force_as<Fixnum>(unmarshal()));
    code->total_args(state, force_as<Fixnum>(unmarshal()));
    code->splat(state, force_as<Fixnum>(unmarshal()));
    code->keywords(state, force_as<Tuple>(unmarshal()));
    code->arity(state, force_as<Fixnum>(unmarshal()));
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
    case 'R':
      return get_rational();
    case 'C':
      return get_complex();
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
      Exception::raise_type_error(state, str.c_str());
    }
  }
}

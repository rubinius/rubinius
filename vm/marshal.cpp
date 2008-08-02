#include "objectmemory.hpp"
#include "marshal.hpp"

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

  void Marshaller::set_int(OBJECT obj) {
    stream << "I" << endl << as<Integer>(obj)->n2i() << endl;
  }

  void Marshaller::set_bignum(Bignum* big) {
    char buf[1024];

    memset(buf, 0, 1024);
    big->into_string(state, 10, buf, 1023);

    stream << "I" << endl << buf << endl;
  }

  OBJECT UnMarshaller::get_int() {
    char data[1024];
    memset(data, 0, 1024);

    stream >> data;

    return Bignum::from_string(state, data, 10);
  }

  void Marshaller::set_string(String* str) {
    stream << "s" << endl << str->size() << endl;
    stream.write((char*)*str, str->size()) << endl;
  }

  String* UnMarshaller::get_string() {
    size_t count;

    stream >> count;
    String* str = String::create(state, NULL, count + 1);

    stream.get(); // read off newline
    stream.read((char*)*str, count);
    stream.get(); // read off newline

    return str;
  }

  void Marshaller::set_symbol(SYMBOL sym) {
    String* str = sym->to_str(state);
    stream << "x" << endl << str->size() << endl;
    stream.write((char*)*str, str->size()) << endl;
  }

  SYMBOL UnMarshaller::get_symbol() {
    char data[1024];
    size_t count;

    stream >> count;
    stream.get();
    stream.read(data, count + 1);
    data[count] = 0; // clamp

    return G(symbols)->lookup(state, data);
  }

  void Marshaller::set_sendsite(SendSite* ss) {
    String* str = ss->name->to_str(state);
    stream << "S" << endl << str->size() << endl;
    stream.write((char*)*str, str->size()) << endl;
  }

  SendSite* UnMarshaller::get_sendsite() {
    char data[1024];
    size_t count;

    stream >> count;
    stream.get();
    stream.read(data, count + 1);
    data[count] = 0; // clamp

    SYMBOL sym = G(symbols)->lookup(state, data);

    return SendSite::create(state, sym);
  }

  void Marshaller::set_array(Array* ary) {
    size_t count = ary->size();

    stream << "A" << endl << count << endl;

    for(size_t i = 0; i < count; i++) {
      marshal(ary->get(state, i));
    }
  }

  Array* UnMarshaller::get_array() {
    size_t count;
    stream >> count;

    Array* ary = Array::create(state, count);

    for(size_t i = 0; i < count; i++) {
      ary->set(state, i, unmarshal());
    }

    return ary;
  }

  void Marshaller::set_tuple(Tuple* tup) {
    stream << "p" << endl << tup->field_count << endl;

    for(size_t i = 0; i < tup->field_count; i++) {
      marshal(tup->at(i));
    }
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

  void Marshaller::set_float(Float* flt) {
    stream << "d" << endl << flt->val << endl;
  }

  Float* UnMarshaller::get_float() {
    double val;
    stream >> val;

    return Float::create(state, val);
  }

  void Marshaller::set_iseq(InstructionSequence* iseq) {
    Tuple* ops = iseq->opcodes;
    stream << "i" << endl << ops->field_count << endl;
    for(size_t i = 0; i < ops->field_count; i++) {
      stream << as<Fixnum>(ops->at(i))->to_nint() << endl;
    }
  }

  InstructionSequence* UnMarshaller::get_iseq() {
    size_t count;
    long op;
    stream >> count;

    InstructionSequence* iseq = InstructionSequence::create(state, count);
    Tuple* ops = iseq->opcodes;

    for(size_t i = 0; i < count; i++) {
      stream >> op;
      ops->put(state, i, Object::i2n(op));
    }

    iseq->post_marshal(state);

    return iseq;
  }

  void Marshaller::set_cmethod(CompiledMethod* cm) {
    assert(0);
  }

  CompiledMethod* UnMarshaller::get_cmethod() {
    size_t ver;
    stream >> ver;

    CompiledMethod* cm = CompiledMethod::create(state);

    SET(cm, __ivars__, unmarshal());
    SET(cm, primitive, unmarshal());
    SET(cm, name,      unmarshal());
    SET(cm, iseq,      unmarshal());
    SET(cm, stack_size, unmarshal());
    SET(cm, local_count, unmarshal());
    SET(cm, required_args, unmarshal());
    SET(cm, total_args, unmarshal());
    SET(cm, splat,     unmarshal());
    SET(cm, literals,  unmarshal());
    SET(cm, exceptions, unmarshal());
    SET(cm, lines,     unmarshal());
    SET(cm, file,      unmarshal());
    SET(cm, local_names, unmarshal());

    cm->post_marshal(state);

    return cm;
  }

  OBJECT UnMarshaller::unmarshal() {
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
    case 'S':
      return get_sendsite();
    case 'A':
      return get_array();
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
      throw str;
    }
  }

  void Marshaller::marshal(OBJECT obj) {
    if(obj == Qnil) {
      stream << "n" << endl;
    } else if(obj == Qtrue) {
      stream << "t" << endl;
    } else if(obj == Qfalse) {
      stream << "f" << endl;
    } else if(obj->fixnum_p()) {
      set_int(obj);
    } else if(obj->symbol_p()) {
      set_symbol((SYMBOL)obj);
    } else if(kind_of<Bignum>(obj)) {
      set_bignum(as<Bignum>(obj));
    } else if(kind_of<String>(obj)) {
      set_string(as<String>(obj));
    } else if(kind_of<SendSite>(obj)) {
      set_sendsite(as<SendSite>(obj));
    } else if(kind_of<Array>(obj)) {
      set_array(as<Array>(obj));
    } else if(kind_of<Tuple>(obj)) {
      set_tuple(as<Tuple>(obj));
    } else if(kind_of<Float>(obj)) {
      set_float(as<Float>(obj));
    } else if(kind_of<InstructionSequence>(obj)) {
      set_iseq(as<InstructionSequence>(obj));
    } else if(kind_of<CompiledMethod>(obj)) {
      set_cmethod(as<CompiledMethod>(obj));
    } else {
      throw "unknown object";
    }
  }


}

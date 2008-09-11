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
    stream << "I" << endl << as<Integer>(obj)->to_native() << endl;
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
    stream.write(str->byte_address(), str->size()) << endl;
  }

  String* UnMarshaller::get_string() {
    size_t count;

    stream >> count;
    // String::create adds room for a trailing null on its own
    String* str = String::create(state, NULL, count);

    stream.get(); // read off newline
    stream.read(str->byte_address(), count);
    stream.get(); // read off newline

    return str;
  }

  void Marshaller::set_symbol(SYMBOL sym) {
    String* str = sym->to_str(state);
    stream << "x" << endl << str->size() << endl;
    stream.write(str->byte_address(), str->size()) << endl;
  }

  SYMBOL UnMarshaller::get_symbol() {
    char data[1024];
    size_t count;

    stream >> count;
    stream.get();
    stream.read(data, count + 1);
    data[count] = 0; // clamp

    return state->symbol(data);
  }

  void Marshaller::set_sendsite(SendSite* ss) {
    String* str = ss->name()->to_str(state);
    stream << "S" << endl << str->size() << endl;
    stream.write(str->c_str(), str->size()) << endl;
  }

  SendSite* UnMarshaller::get_sendsite() {
    char data[1024];
    size_t count;

    stream >> count;
    stream.get();
    stream.read(data, count + 1);
    data[count] = 0; // clamp

    SYMBOL sym = state->symbol(data);

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
    Tuple* ops = iseq->opcodes();
    stream << "i" << endl << ops->field_count << endl;
    for(size_t i = 0; i < ops->field_count; i++) {
      stream << as<Fixnum>(ops->at(i))->to_native() << endl;
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

  void Marshaller::set_cmethod(CompiledMethod* cm) {
    assert(0);
  }

  CompiledMethod* UnMarshaller::get_cmethod() {
    size_t ver;
    stream >> ver;

    CompiledMethod* cm = CompiledMethod::create(state);

    cm->ivars(state, unmarshal());
    cm->primitive(state, (SYMBOL)unmarshal());
    cm->name(state, (SYMBOL)unmarshal());
    cm->iseq(state, (InstructionSequence*)unmarshal());
    cm->stack_size(state, (FIXNUM)unmarshal());
    cm->local_count(state, (FIXNUM)unmarshal());
    cm->required_args(state, (FIXNUM)unmarshal());
    cm->total_args(state, (FIXNUM)unmarshal());
    cm->splat(state, unmarshal());
    cm->literals(state, (Tuple*)unmarshal());
    cm->exceptions(state, (Tuple*)unmarshal());
    cm->lines(state, (Tuple*)unmarshal());
    cm->file(state, (SYMBOL)unmarshal());
    cm->local_names(state, (Tuple*)unmarshal());

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
      throw std::runtime_error(str);
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
      throw std::runtime_error("unknown object");
    }
  }


}

#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "marshal.hpp"

namespace rubinius {

  using std::endl;

  void Marshaller::set_int(OBJECT obj) {
    stream << "I" << endl << obj->n2i() << endl;
  }

  void Marshaller::set_bignum(Bignum* big) {
    char buf[1024];

    memset(buf, 0, 1024);
    big->into_string(state, 10, buf, 1023);

    stream << "I" << endl << buf << endl;
  }

  OBJECT UnMarshaller::get_int() {
    long num;

    stream >> num;
    return Object::i2n(state, num);
  }

  void Marshaller::set_string(String* str) {
    stream << "s" << endl << str->size() << endl;
    stream.write((char*)*str, str->size()) << endl;
  }

  String* UnMarshaller::get_string() {
    size_t count;

    stream >> count;
    String* str = String::create(state, NULL, count + 1);

    stream >> (char*)*str;

    return str;
  }

  void Marshaller::set_symbol(SYMBOL sym) {
    String* str = sym->to_str(state);
    stream << "x" << endl << (char*)*str << endl;
  }

  SYMBOL UnMarshaller::get_symbol() {
    char data[1024];

    stream >> data;

    return G(symbols)->lookup(state, data);
  }

  void Marshaller::set_sendsite(SendSite* ss) {
    String* str = ss->name->to_str(state);
    stream << "S" << endl << (char*)*str << endl;
  }

  SendSite* UnMarshaller::get_sendsite() {
    char data[1024];
    stream >> data;

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

  void Marshaller::set_iseq(ISeq* iseq) {
    stream << "i" << endl << iseq->body_in_bytes() << endl;
    stream.write(iseq->bytes, iseq->body_in_bytes()) << endl;
  }

  ISeq* UnMarshaller::get_iseq() {
    size_t bytes;
    stream >> bytes;

    ISeq* iseq = ISeq::create(state, bytes);
    stream.read(iseq->bytes, bytes);

    iseq->post_marshal(state);

    return iseq;
  }

  void Marshaller::set_cmethod(CompiledMethod* cm) {
    stream << "M" << endl << 1 << endl;

    for(size_t i = 0; i < CompiledMethod::saved_fields; i++) {
      marshal(cm->at(i));
    }
  }

  CompiledMethod* UnMarshaller::get_cmethod() {
    size_t ver;
    stream >> ver;

    CompiledMethod* cm = CompiledMethod::create(state);

    for(size_t i = 0; i < CompiledMethod::saved_fields; i++) {
      SET(cm, field[i], unmarshal());
    }

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
      throw "unknown marshal code";
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
    } else if(kind_of<ISeq>(obj)) {
      set_iseq(as<ISeq>(obj));
    } else if(kind_of<CompiledMethod>(obj)) {
      set_cmethod(as<CompiledMethod>(obj));
    } else {
      throw "unknown object";
    }
  }


}

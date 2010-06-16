#include "vm/test/test.hpp"

#include "marshal.hpp"

#include <iostream>
#include <sstream>
#include <cmath>

class StringUnMarshaller : public UnMarshaller {
public:
  std::istringstream sstream;

  StringUnMarshaller(STATE) : UnMarshaller(state, sstream) { }
};

class TestUnMarshal : public CxxTest::TestSuite, public VMTest {
public:
  StringUnMarshaller* mar;

  void setUp() {
    create();
    mar = new StringUnMarshaller(state);
  }

  void tearDown() {
    destroy();
    delete mar;
  }

  bool tuple_equals(Tuple* x, Tuple* y) {
    if(x->num_fields() != y->num_fields()) return false;
    for(size_t i = 0; i < x->num_fields(); i++) {
      Object* x1 = x->at(state, i);
      Object* y1 = y->at(state, i);

      if(kind_of<Tuple>(x1)) {
        if(!tuple_equals(as<Tuple>(x1), as<Tuple>(y1))) return false;
      } else {
        if(x1 != y1) return false;
      }
    }

    return true;
  }

  void test_nil() {
    mar->sstream.str(std::string("n\n"));
    Object* obj = mar->unmarshal();

    TS_ASSERT_EQUALS(obj, Qnil);
  }

  void test_true() {
    mar->sstream.str(std::string("t\n"));
    Object* obj = mar->unmarshal();

    TS_ASSERT_EQUALS(obj, Qtrue);
  }

  void test_false() {
    mar->sstream.str(std::string("f\n"));
    Object* obj = mar->unmarshal();

    TS_ASSERT_EQUALS(obj, Qfalse);
  }

  void test_int() {
    mar->sstream.str(std::string("I\n42\n"));
    Object* obj = mar->unmarshal();

    TS_ASSERT(obj->fixnum_p());
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_native(), 66);
  }

  void test_large_int() {
    mar->sstream.str(std::string("I\n100000000\n"));
    Object* obj = mar->unmarshal();

    TS_ASSERT(kind_of<Integer>(obj));
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_ulong_long(), 4294967296ULL);
  }

  void test_larger_int() {
    mar->sstream.str(std::string("I\n9d89d89d89d89dc\n"));
    Object* obj = mar->unmarshal();

    TS_ASSERT(kind_of<Integer>(obj));
    TS_ASSERT_EQUALS(as<Integer>(obj)->to_ulong_long(), 709490156681136604ULL);
  }

  void test_string() {
    mar->sstream.str(std::string("s\n4\nblah\n"));
    Object* obj = mar->unmarshal();

    TS_ASSERT(kind_of<String>(obj));
    String *str = as<String>(obj);
    TS_ASSERT_EQUALS(std::string(str->c_str()), "blah");
  }

  void test_symbol() {
    mar->sstream.str(std::string("x\n4\nblah\n"));
    Object* obj = mar->unmarshal();

    TS_ASSERT(obj->symbol_p());
    TS_ASSERT_EQUALS(obj, state->symbol("blah"));
  }

  void test_tuple() {
    mar->sstream.str(std::string("p\n2\nI\n2\nI\n2f\n"));

    Object* obj = mar->unmarshal();

    TS_ASSERT(kind_of<Tuple>(obj));

    Tuple* tup = as<Tuple>(obj);

    TS_ASSERT_EQUALS(tup->at(state, 0), Fixnum::from(2));
    TS_ASSERT_EQUALS(tup->at(state, 1), Fixnum::from(47));
  }

  void test_float() {
    mar->sstream.str(
        std::string("d\n +0.666666666666666629659232512494781985878944396972656250    -2\n"));

    Object* obj = mar->unmarshal();

    TS_ASSERT(kind_of<Float>(obj));

    Float* flt = as<Float>(obj);

    TS_ASSERT_EQUALS(flt->val, 1.0 / 6.0);

    mar->sstream.str(
        std::string("d\n +0.999999999999999888977697537484345957636833190917968750  1024\n"));
    obj = mar->unmarshal();

    TS_ASSERT(kind_of<Float>(obj));

    flt = as<Float>(obj);

    double diff = flt->val - DBL_MAX;
    TS_ASSERT(diff < 0.00000001 && diff > - 0.00000001);
  }

  void test_float_infinity() {
    mar->sstream.str(std::string("d\nInfinity\n"));

    Object* obj = mar->unmarshal();

    TS_ASSERT(kind_of<Float>(obj));

    Float* flt = as<Float>(obj);

    TS_ASSERT(std::isinf(flt->val));
  }

  void test_float_neg_infinity() {
    mar->sstream.str(std::string("d\n-Infinity\n"));

    Object* obj = mar->unmarshal();

    TS_ASSERT(kind_of<Float>(obj));

    Float* flt = as<Float>(obj);

    TS_ASSERT(std::isinf(flt->val));
    TS_ASSERT(flt->val < 0.0);
  }

  void test_float_nan() {
    mar->sstream.str(std::string("d\nNaN\n"));

    Object* obj = mar->unmarshal();

    TS_ASSERT(kind_of<Float>(obj));

    Float* flt = as<Float>(obj);

    TS_ASSERT(std::isnan(flt->val));
  }

  void test_iseq() {
    mar->sstream.str(std::string("i\n1\n0\n"));

    Object* obj = mar->unmarshal();

    TS_ASSERT(kind_of<InstructionSequence>(obj));

    InstructionSequence* seq = as<InstructionSequence>(obj);

    TS_ASSERT(kind_of<Tuple>(seq->opcodes()));

    TS_ASSERT_EQUALS(seq->opcodes()->num_fields(), 1U);

    TS_ASSERT_EQUALS(seq->opcodes()->at(state, 0), Fixnum::from(0));
  }

  void test_cmethod() {
    std::string str = "M\n1\nn\nx\n12\nobject_equal\nx\n4\ntest\ni\n1\n0\nI\na\nI\n0\nI\n0\nI\n0\nn\np\n2\nI\n1\nI\n2\np\n1\np\n3\nI\n0\nI\n1\nI\n1\nx\n8\nnot_real\np\n1\nx\n4\nblah\n";
    mar->sstream.str(str);

    Object* obj = mar->unmarshal();

    TS_ASSERT(kind_of<CompiledMethod>(obj));

    CompiledMethod* cm = as<CompiledMethod>(obj);

    TS_ASSERT_EQUALS(cm->ivars(), Qnil);
    TS_ASSERT_EQUALS(cm->primitive(), state->symbol("object_equal"));
    TS_ASSERT_EQUALS(cm->name(), state->symbol("test"));
    TS_ASSERT(tuple_equals(cm->iseq()->opcodes(), Tuple::from(state, 1, Fixnum::from(0))));
    TS_ASSERT_EQUALS(cm->stack_size(), Fixnum::from(10));
    TS_ASSERT_EQUALS(cm->local_count(), Fixnum::from(0));
    TS_ASSERT_EQUALS(cm->required_args(), Fixnum::from(0));
    TS_ASSERT_EQUALS(cm->total_args(), Fixnum::from(0));
    TS_ASSERT_EQUALS(cm->splat(), Qnil);
    TS_ASSERT(tuple_equals(cm->literals(), Tuple::from(state, 2, Fixnum::from(1), Fixnum::from(2))));
    TS_ASSERT(tuple_equals(cm->lines(), Tuple::from(state, 1,
          Tuple::from(state, 3, Fixnum::from(0), Fixnum::from(1), Fixnum::from(1)))));

    TS_ASSERT_EQUALS(cm->file(), state->symbol("not_real"));
    TS_ASSERT(tuple_equals(cm->local_names(), Tuple::from(state, 1, state->symbol("blah"))));
  }

};

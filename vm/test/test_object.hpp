#include "vm/test/test.hpp"

#include "builtin/object.hpp"
#include "builtin/compactlookuptable.hpp"

class TestObject : public CxxTest::TestSuite, public VMTest {
public:

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_kind_of() {
    Object* obj = util_new_object();
    TS_ASSERT(kind_of<Object>(obj));
    TS_ASSERT(kind_of<Object>(Fixnum::from(1)));
    TS_ASSERT(kind_of<Object>(String::create(state, "blah")));
  }

  void test_instance_of() {
    Object* obj = util_new_object();
    TS_ASSERT(instance_of<Object>(obj));
    TS_ASSERT(!instance_of<Object>(Fixnum::from(1)));
    TS_ASSERT(!instance_of<Object>(String::create(state, "blah")));
  }

  void test_instance_variables() {
    Object* obj = util_new_object();
    Symbol* t1 = state->symbol("@test1");
    Symbol* t2 = state->symbol("@test2");

    obj->set_ivar(state, t1, cTrue);
    obj->set_ivar(state, t2, cTrue);

    TS_ASSERT_EQUALS(cTrue, as<CompactLookupTable>(obj->ivars())->fetch(state, t1));
    TS_ASSERT_EQUALS(cTrue, as<CompactLookupTable>(obj->ivars())->fetch(state, t2));
  }

  void test_instance_variables_none() {
    Object* obj = util_new_object();

    TS_ASSERT_EQUALS(cNil, obj->ivars());
  }

  void test_as() {
    Object* obj = util_new_object();
    Fixnum* fix = Fixnum::from(1);

    Object* nil = cNil;

    // OK
    TS_ASSERT_EQUALS(as<Object>(obj), obj);

    TS_ASSERT_EQUALS(as<Integer>(fix), fix);
    TS_ASSERT_EQUALS(as<Fixnum>(fix), fix);
    TS_ASSERT_EQUALS(as<Object>(fix), fix);

    // Fail
    TS_ASSERT_THROWS(as<String>(nil), TypeError);
    TS_ASSERT_THROWS(as<String>(obj), TypeError);
    TS_ASSERT_THROWS(as<String>(fix), TypeError);
  }

  void test_try_as() {
    Object* obj = util_new_object();
    Fixnum* fix = Fixnum::from(1);

    Object* nil = cNil;

    // OK
    TS_ASSERT_EQUALS(try_as<Object>(obj), obj);
    TS_ASSERT_EQUALS(try_as<Object>(fix), fix);

    // OK, but returns NULL because there is no conversion
    TS_ASSERT_EQUALS(try_as<String>(nil), static_cast<String*>(NULL));
    TS_ASSERT_EQUALS(try_as<String>(obj), static_cast<String*>(NULL));
    TS_ASSERT_EQUALS(try_as<String>(fix), static_cast<String*>(NULL));
  }

  void test_duplicate() {
    Tuple* tup = Tuple::create(state, 1);
    tup->put(state, 0, cTrue);

    tup->set_ivar(state, state->symbol("@name"), state->symbol("foo"));

    Tuple* tup2 = as<Tuple>(tup->duplicate(state));

    TS_ASSERT_EQUALS(tup2->at(state, 0), cTrue);
    TS_ASSERT_DIFFERS(tup->id(state), tup2->id(state));

    TS_ASSERT(tup->ivars_ != tup2->ivars_);

    TS_ASSERT_EQUALS(tup2->get_ivar(state, state->symbol("@name")),
        state->symbol("foo"));

    tup->ivars_ = as<CompactLookupTable>(tup->ivars_)->to_lookuptable(state);
    Tuple* tup3 = as<Tuple>(tup->duplicate(state));

    TS_ASSERT(tup->ivars_ != tup2->ivars_);
    TS_ASSERT_EQUALS(tup3->get_ivar(state, state->symbol("@name")),
        state->symbol("foo"));
  }

  void test_dup_ignores_singleton_class() {
    Tuple* tup = Tuple::create(state, 1);
    tup->put(state, 0, cTrue);

    // Force it to exist.
    tup->singleton_class(state);

    Tuple* tup2 = as<Tuple>(tup->duplicate(state));

    TS_ASSERT(!try_as<SingletonClass>(tup2->klass_));

    TS_ASSERT_DIFFERS(tup->singleton_class(state), tup2->singleton_class(state));
  }

  void test_dup_bytes() {
    ByteArray* obj = ByteArray::create(state, 1);

    obj->raw_bytes()[0] = 8;

    ByteArray* obj2 = as<ByteArray>(obj->duplicate(state));

    TS_ASSERT_EQUALS(obj2->raw_bytes()[0], 8);
  }

  void test_kind_of_p() {
    String* str = String::create(state, "blah");

    TS_ASSERT(str->kind_of_p(state, G(string)));
    TS_ASSERT(!str->kind_of_p(state, G(tuple)));
  }

  void test_kind_of_prim() {
    String* str = String::create(state, "thingy");

    TS_ASSERT_EQUALS(cTrue, str->kind_of_prim(state, G(string)));
    TS_ASSERT_EQUALS(cFalse, str->kind_of_prim(state, G(tuple)));
  }

  void test_hash() {
    TS_ASSERT_EQUALS(cNil->hash(state), cNil->hash(state));
    TS_ASSERT(cNil->hash(state) > 0);

    TS_ASSERT_EQUALS(cTrue->hash(state), cTrue->hash(state));
    TS_ASSERT(cTrue->hash(state) > 0);

    TS_ASSERT_EQUALS(cFalse->hash(state), cFalse->hash(state));
    TS_ASSERT(cFalse->hash(state) > 0);

    TS_ASSERT_DIFFERS(Fixnum::from(1)->hash(state), Fixnum::from(2)->hash(state));
    TS_ASSERT_DIFFERS(Fixnum::from(-1)->hash(state), Fixnum::from(1)->hash(state));
    TS_ASSERT_DIFFERS(Fixnum::from(-2)->hash(state), Fixnum::from(-1)->hash(state));

    TS_ASSERT_EQUALS(Bignum::from(state, (native_int)13)->hash(state), Bignum::from(state, (native_int)13)->hash(state));
    TS_ASSERT(Bignum::from(state, (native_int)13)->hash(state) > 0);

    TS_ASSERT_EQUALS(Float::create(state, 15.0)->hash(state), Float::create(state, 15.0)->hash(state));
    TS_ASSERT(Float::create(state, 15.0)->hash(state) > 0);

    Object* obj = util_new_object();
    TS_ASSERT_EQUALS(obj->hash(state), obj->hash(state));
    TS_ASSERT(obj->hash(state) > 0);

    TS_ASSERT_EQUALS(String::create(state, "blah")->hash(state), String::create(state, "blah")->hash(state));
    TS_ASSERT(String::create(state, "blah")->hash(state) > 0);
  }

  void test_singleton_class() {
    TS_ASSERT(kind_of<SingletonClass>(G(object)->singleton_class(state)));
    TS_ASSERT_EQUALS(cNil->singleton_class(state), G(nil_class));
    TS_ASSERT_EQUALS(cTrue->singleton_class(state), G(true_class));
    TS_ASSERT_EQUALS(cFalse->singleton_class(state), G(false_class));

    Tuple *tup = Tuple::create(state, 1);
    TS_ASSERT(!kind_of<SingletonClass>(tup->klass()));

    TS_ASSERT(kind_of<SingletonClass>(tup->singleton_class(state)));
    TS_ASSERT(kind_of<SingletonClass>(tup->klass()));
  }

  void test_equal() {
    String* s1 = String::create(state, "whatever");
    String* s2 = String::create(state, "whatever");

    TS_ASSERT_EQUALS(as<Object>(s1)->equal(state, as<Object>(s2)), cFalse);
    TS_ASSERT_EQUALS(as<Object>(Fixnum::from(0))->equal(state, as<Object>(Fixnum::from(0))), cTrue);
  }

  void test_set_ivar() {
    size_t size = COMPACTLOOKUPTABLE_SIZE / 2 + 2;
    Object* obj = util_new_object();
    Symbol* sym;

    for(size_t i = 0; i < size; i++) {
      std::ostringstream name;
      name << "@test" << i;
      sym = state->symbol(name.str().c_str());
      obj->set_ivar(state, sym, Fixnum::from(i));
    }

    sym = state->symbol("@test5");
    TS_ASSERT_EQUALS(obj->get_ivar(state, sym), Fixnum::from(5));
  }

  void test_set_ivar_on_immediate() {
    size_t size = COMPACTLOOKUPTABLE_SIZE / 2 + 2;
    Object* obj = Fixnum::from(-10);
    Symbol* sym;

    for(size_t i = 0; i < size; i++) {
      std::ostringstream name;
      name << "@test" << i;
      sym = state->symbol(name.str().c_str());
      obj->set_ivar(state, sym, Fixnum::from(i));
    }

    sym = state->symbol("@test5");
    TS_ASSERT_EQUALS(obj->get_ivar(state, sym), Fixnum::from(5));
  }

  void test_get_ivar() {
    Symbol* sym = state->symbol("@test");
    Object* val = Fixnum::from(33);
    Object* obj = util_new_object();

    TS_ASSERT_EQUALS(cNil, obj->get_ivar(state, state->symbol("@non_existent")));
    TS_ASSERT_EQUALS(cNil, obj->get_ivar(state, sym));

    obj->set_ivar(state, sym, val);

    TS_ASSERT_EQUALS(val, obj->get_ivar(state, sym));
  }

  void test_get_ivar_on_immediate() {
    Symbol* sym = state->symbol("@test");
    Object* val = Fixnum::from(33);
    Object* obj = Fixnum::from(-10);

    TS_ASSERT_EQUALS(cNil, obj->get_ivar(state, state->symbol("@non_existent")));
    TS_ASSERT_EQUALS(cNil, obj->get_ivar(state, sym));

    obj->set_ivar(state, sym, val);

    TS_ASSERT_EQUALS(val, obj->get_ivar(state, sym));
  }

  void test_ivars() {
    Object* obj = util_new_object();

    Symbol* sym1 = state->symbol("@test1");
    Symbol* sym2 = state->symbol("@test2");
    Fixnum* one = Fixnum::from(1);
    Fixnum* two = Fixnum::from(2);
    obj->set_ivar(state, sym1, one);
    obj->set_ivar(state, sym2, two);

    CompactLookupTable* ivars = as<CompactLookupTable>(obj->ivars());

    TS_ASSERT_EQUALS(ivars->fetch(state, sym1), one);
    TS_ASSERT_EQUALS(ivars->fetch(state, sym2), two);
  }

  void test_id() {
    Tuple* t1 = Tuple::create(state, 2);
    Tuple* t2 = Tuple::create(state, 2);

    Integer* id1 = t1->id(state);
    Integer* id2 = t2->id(state);

    TS_ASSERT(id1->to_native() > 0);
    TS_ASSERT(id2->to_native() > 0);

    TS_ASSERT_DIFFERS(id1, id2)

    TS_ASSERT_EQUALS(id1, t1->id(state));

    Integer* id3 = Fixnum::from(33)->id(state);
    TS_ASSERT_DIFFERS(id3, id1);

    Integer* id4 = Fixnum::from(33)->id(state);
    TS_ASSERT_EQUALS(id3, id4);
    TS_ASSERT(id4->to_native() % 2 == 1);

    Integer* id5 = Fixnum::from(34)->id(state);
    TS_ASSERT_DIFFERS(id4, id5);
    TS_ASSERT(id5->to_native() % 2 == 1);
  }

  void test_infect() {
    Object* obj1 = util_new_object();
    Object* obj2 = util_new_object();

    TS_ASSERT_EQUALS(obj1->tainted_p(state), cFalse);
    TS_ASSERT_EQUALS(obj2->tainted_p(state), cFalse);

    obj1->infect(state, obj2);

    TS_ASSERT_EQUALS(obj2->tainted_p(state), cFalse);

    obj1->taint(state);
    obj1->infect(state, obj2);

    TS_ASSERT_EQUALS(obj2->tainted_p(state), cTrue);
  }

  void test_infect_non_reference() {
    Object* obj1 = util_new_object();
    Object* obj2 = Integer::from(state, 5);

    obj1->infect(state, obj2);

    TS_ASSERT_EQUALS(obj2->tainted_p(state), cFalse);

    obj1->taint(state);
    obj1->infect(state, obj2);

    TS_ASSERT_EQUALS(obj2->tainted_p(state), cFalse);
  }

  void test_tainted_p() {
    Object* obj = util_new_object();

    TS_ASSERT_EQUALS(obj->tainted_p(state), cFalse);
    obj->taint(state);
    TS_ASSERT_EQUALS(obj->tainted_p(state), cTrue);
  }

  void test_tainted_p_non_reference() {
    Object* obj = Integer::from(state, 5);

    TS_ASSERT_EQUALS(obj->tainted_p(state), cFalse);
    obj->taint(state);
    TS_ASSERT_EQUALS(obj->tainted_p(state), cFalse);
  }

  void test_taint() {
    Object* obj = util_new_object();

    TS_ASSERT_EQUALS(obj->tainted_p(state), cFalse);
    obj->taint(state);
    TS_ASSERT_EQUALS(obj->tainted_p(state), cTrue);
  }

  void test_untaint() {
    Object* obj = util_new_object();

    obj->taint(state);
    TS_ASSERT_EQUALS(obj->tainted_p(state), cTrue);
    obj->untaint(state);
    TS_ASSERT_EQUALS(obj->tainted_p(state), cFalse);
  }

  void test_frozen_p() {
    Object* obj = util_new_object();

    TS_ASSERT_EQUALS(obj->frozen_p(state), cFalse);
    obj->freeze(state);
    TS_ASSERT_EQUALS(obj->frozen_p(state), cTrue);
  }

  void test_freeze() {
    Object* obj = util_new_object();

    TS_ASSERT_EQUALS(obj->frozen_p(state), cFalse);
    obj->freeze(state);
    TS_ASSERT(obj->frozen_p(state));
  }

  void test_nil_class() {
    TS_ASSERT_EQUALS(cNil->class_object(state), G(nil_class));
  }

  void test_true_class() {
    TS_ASSERT_EQUALS(cTrue->class_object(state), G(true_class));
  }

  void test_false_class() {
    TS_ASSERT_EQUALS(cFalse->class_object(state), G(false_class));
  }

  void test_fixnum_class() {
    for(size_t i = 0; i < SPECIAL_CLASS_MASK; i++) {
      TS_ASSERT_EQUALS(Fixnum::from(i)->class_object(state), G(fixnum_class));
    }
  }

  void test_object_class() {
    Array* ary = Array::create(state, 1);

    TS_ASSERT_EQUALS(G(array), ary->class_object(state));
  }

  void test_object_class_with_superclass_chain() {
    Module* mod = Module::create(state);
    Class* cls = Class::create(state, G(object));
    Object* obj = state->new_object<Object>(cls);

    /* This should be functionally correct but not actually the
     * way a superclass chain is implemented. However, it doesn't
     * require that we create a root for IncludedModule.
     */
    Module* m = cls->superclass();
    cls->superclass(state, mod);
    mod->superclass(state, m);

    TS_ASSERT_EQUALS(cls, obj->class_object(state));
  }

  void test_symbol_class() {
    TS_ASSERT_EQUALS(state->symbol("blah")->class_object(state), G(symbol));
  }

  CompiledCode* create_compiled_code() {
    CompiledCode* code = CompiledCode::create(state);
    code->iseq(state, InstructionSequence::create(state, 1));
    code->iseq()->opcodes()->put(state, 0, Fixnum::from(InstructionSequence::insn_ret));
    code->stack_size(state, Fixnum::from(10));
    code->total_args(state, Fixnum::from(0));
    code->required_args(state, code->total_args());

    // code->formalize(state);

    return code;
  }

  void test_nil_p() {
    TS_ASSERT(cNil->nil_p());
    TS_ASSERT(!cUndef->nil_p());
    TS_ASSERT(!cTrue->nil_p());
    TS_ASSERT(!cFalse->nil_p());
  }

  void test_true_p() {
    TS_ASSERT(!cNil->true_p());
    TS_ASSERT(!cUndef->true_p());
    TS_ASSERT(cTrue->true_p());
    TS_ASSERT(!cFalse->true_p());
  }

  void test_false_p() {
    TS_ASSERT(!cNil->false_p());
    TS_ASSERT(!cUndef->false_p());
    TS_ASSERT(!cTrue->false_p());
    TS_ASSERT(cFalse->false_p());
  }

  void test_get_type() {
    TS_ASSERT_EQUALS(cNil->get_type(), NilType);
    TS_ASSERT_EQUALS(cTrue->get_type(), TrueType);
    TS_ASSERT_EQUALS(cFalse->get_type(), FalseType);
    TS_ASSERT_EQUALS(state->symbol("blah")->get_type(), SymbolType);
    Object* obj = util_new_object();
    Bignum* big = Bignum::from(state, (native_int)13);
    TS_ASSERT_EQUALS(obj->get_type(), ObjectType);
    TS_ASSERT_EQUALS(big->get_type(), BignumType);
  }

  Object* util_new_object() {
    return state->new_object<Object>(G(object));
  }
};

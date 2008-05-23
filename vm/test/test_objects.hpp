#include "objects.hpp"
#include "vm.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestObjects : public CxxTest::TestSuite {
  public:

#define check_const(obj, name) TS_ASSERT_EQUALS(G(object)->get_const(state,name), G(obj))

  VM *state;

  void setUp() {
    state = new VM(1024);
  }

  void tearDown() {
    delete state;
  }

  void test_object() {
    TS_ASSERT_EQUALS(G(object)->class_object(state), G(klass));
    TS_ASSERT_EQUALS(G(object)->superclass, Qnil);
    check_const(object, "Object");
  }

  void test_class() {
    Class *cls;

    cls = G(klass);
    Class* o = cls->class_object(state);
    TS_ASSERT_EQUALS(cls, o);
    TS_ASSERT_EQUALS(cls->superclass, G(module));
    check_const(klass, "Class");
  }

  void test_metaclass_is_completely_setup() {
    Class *cls;
    MetaClass *meta;

    cls = (Class*)G(klass);
    meta = (MetaClass*)cls->klass;
    TS_ASSERT(kind_of<MetaClass>(G(object)->klass));
    TS_ASSERT(kind_of<LookupTable>(meta->method_table));
    TS_ASSERT(kind_of<LookupTable>(meta->constants));
  }

  void test_module() {
    Class *mod;

    mod = (Class*)G(module);
    TS_ASSERT_EQUALS(mod->class_object(state), G(klass));
    TS_ASSERT_EQUALS(mod->superclass, G(object));
    check_const(module, "Module");
  }

  void test_metaclass() {
    Class *meta;

    meta = G(metaclass);

    TS_ASSERT_EQUALS(meta->class_object(state), G(klass));
    TS_ASSERT_EQUALS(meta->superclass, G(klass));
    TS_ASSERT_EQUALS(meta->instance_type, Object::i2n(MetaclassType));
    check_const(metaclass, "MetaClass");
  }

  void test_tuple() {
    Class *tup;

    tup = G(tuple);

    TS_ASSERT_EQUALS(tup->class_object(state), G(klass));
    TS_ASSERT_EQUALS(tup->superclass, G(object));
    TS_ASSERT_EQUALS(tup->instance_type, Object::i2n(TupleType));
    check_const(tuple, "Tuple");
  }

  void test_lookuptable() {
    Class *lt;

    lt = G(lookuptable);

    TS_ASSERT_EQUALS(lt->class_object(state), G(klass));
    TS_ASSERT_EQUALS(lt->superclass, G(object));
    TS_ASSERT_EQUALS(lt->instance_type, Object::i2n(LookupTableType));
    check_const(lookuptable, "LookupTable");
  }

  void test_methtbl() {
    Class *cls;

    cls = G(methtbl);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass, G(lookuptable));

    TS_ASSERT_EQUALS((object_type)cls->instance_type->n2i(), MTType);
    check_const(methtbl, "MethodTable");
  }

  void test_symbol() {
    Class *cls;

    cls = G(symbol);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass, G(object));
    check_const(symbol, "Symbol");
  }

  void test_array() {
    Class *cls;

    cls = G(array);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass, G(object));
    check_const(array, "Array");
  }

  void test_bytearray() {
    Class *cls;

    cls = G(bytearray);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass, G(object));
    TS_ASSERT_EQUALS((object_type)cls->instance_type->n2i(), ByteArrayType);
    check_const(bytearray, "ByteArray");
  }

  void test_string() {
    Class *cls;

    cls = G(string);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass, G(object));
    TS_ASSERT_EQUALS((object_type)cls->instance_type->n2i(), StringType);
    check_const(string, "String");
  }

  void test_symtbl() {
    Class *cls;

    cls = G(symtbl);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass, G(object));
    check_const(symtbl, "SymbolTable");
  }

  void test_cmethod() {
    Class *cls;

    cls = G(cmethod);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass, G(executable));
    check_const(cmethod, "CompiledMethod");
  }
};

#include "vm/test/test.hpp"
#include "version.h"

class TestObjects : public CxxTest::TestSuite, public VMTest {
public:

#define check_const(obj, name) TS_ASSERT_EQUALS(G(object)->get_const(state,name), G(obj))

  void setUp() {
    create();
  }

  void tearDown() {
    destroy();
  }

  void test_object() {
    TS_ASSERT_EQUALS(G(object)->class_object(state), G(klass));
    if(!LANGUAGE_18_ENABLED) {
      TS_ASSERT_EQUALS(G(object)->superclass(), G(basicobject));
    } else {
      TS_ASSERT_EQUALS(G(object)->superclass(), cNil);
    }
    check_const(object, "Object");
  }

  void test_class() {
    Class *cls;

    cls = G(klass);
    Class* o = cls->class_object(state);
    TS_ASSERT_EQUALS(cls, o);
    TS_ASSERT_EQUALS(cls->superclass(), G(module));
    check_const(klass, "Class");
  }

  void test_singleton_class_is_completely_setup() {
    Class *cls;
    SingletonClass *sc;

    cls = G(klass);
    sc = as<SingletonClass>(cls->klass());
    TS_ASSERT(kind_of<SingletonClass>(G(object)->klass()));
    TS_ASSERT(kind_of<MethodTable>(sc->method_table()));
    TS_ASSERT(kind_of<ConstantTable>(sc->constant_table()));
  }

  void test_module() {
    Class *mod;

    mod = G(module);
    TS_ASSERT_EQUALS(mod->class_object(state), G(klass));
    TS_ASSERT_EQUALS(mod->superclass(), G(object));
    check_const(module, "Module");
  }

  void test_tuple() {
    Class *tup;

    tup = G(tuple);

    TS_ASSERT_EQUALS(tup->class_object(state), G(klass));
    TS_ASSERT_EQUALS(tup->superclass(), G(object));
    TS_ASSERT_EQUALS(tup->instance_type(), Fixnum::from(TupleType));
    TS_ASSERT_EQUALS(G(rubinius)->get_const(state, "Tuple"), G(tuple));
  }

  void test_lookuptable() {
    Class *lt;

    lt = G(lookuptable);

    TS_ASSERT_EQUALS(lt->class_object(state), G(klass));
    TS_ASSERT_EQUALS(lt->superclass(), G(object));
    TS_ASSERT_EQUALS(lt->instance_type(), Fixnum::from(LookupTableType));
    TS_ASSERT_EQUALS(G(rubinius)->get_const(state, "LookupTable"), G(lookuptable));
  }

  void test_methtbl() {
    Class *cls;

    cls = G(methtbl);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(object));

    TS_ASSERT_EQUALS((object_type)cls->instance_type()->to_native(), MethodTableType);
    TS_ASSERT_EQUALS(G(rubinius)->get_const(state, "MethodTable"), G(methtbl));
  }

  void test_symbol() {
    Class *cls;

    cls = G(symbol);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(object));
    check_const(symbol, "Symbol");
  }

  void test_array() {
    Class *cls;

    cls = G(array);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(object));
    check_const(array, "Array");
  }

  void test_bytearray() {
    Class *cls;

    cls = G(bytearray);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(object));
    TS_ASSERT_EQUALS((object_type)cls->instance_type()->to_native(), ByteArrayType);
    TS_ASSERT_EQUALS(G(rubinius)->get_const(state, "ByteArray"), G(bytearray));
  }

  void test_string() {
    Class *cls;

    cls = G(string);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(object));
    TS_ASSERT_EQUALS((object_type)cls->instance_type()->to_native(), StringType);
    check_const(string, "String");
  }

  void test_compiled_code() {
    Class *cls;

    cls = G(compiled_code);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(executable));
    TS_ASSERT_EQUALS(G(rubinius)->get_const(state, "CompiledCode"), G(compiled_code));
  }

  void test_dir() {
    Class *cls;

    cls = G(dir);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(object));
    TS_ASSERT_EQUALS(cls->instance_type(), Fixnum::from(DirType));
    check_const(dir, "Dir");
  }

  void test_compactlookuptable() {
    Class *cls;

    cls = G(compactlookuptable);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(tuple));
    TS_ASSERT_EQUALS(cls->instance_type(), Fixnum::from(CompactLookupTableType));
    TS_ASSERT_EQUALS(G(rubinius)->get_const(state, "CompactLookupTable"),
        G(compactlookuptable));
  }

  void test_time_class() {
    Class *cls;

    cls = G(time_class);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(object));
    TS_ASSERT_EQUALS(cls->instance_type(), Fixnum::from(TimeType));
    check_const(time_class, "Time");
  }

  void test_integer_class() {
    check_const(integer, "Integer");
  }

  void test_memory_pointer() {
    Class *cls;

    cls = G(ffi_pointer);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(object));

    Module* ffi = as<Module>(G(rubinius)->get_const(state, "FFI"));
    TS_ASSERT_EQUALS(cls, ffi->get_const(state, "Pointer"));
  }

  void test_exception() {
    Class *cls;

    cls = G(exception);

    TS_ASSERT_EQUALS(cls->class_object(state), G(klass));
    TS_ASSERT_EQUALS(cls->superclass(), G(object));
    check_const(exception, "Exception");
  }
};

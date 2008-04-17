#include "objects.hpp"
#include "vm.hpp"

#include <cxxtest/TestSuite.h>

using namespace rubinius;

class TestObjects : public CxxTest::TestSuite {
  public:

#define check_const(obj, name) TS_ASSERT_EQUALS(vm.globals.object->get_const(&vm,name), vm.globals.obj)

#define state &vm

  void test_object() {
    VM vm(1024);
    TS_ASSERT_EQUALS(vm.globals.object->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(vm.globals.object->superclass, Qnil);
    check_const(object, "Object");
  }

  void test_class() {
    VM vm(1024);
    Class *cls;

    cls = (Class*)vm.globals.klass;
    TS_ASSERT_EQUALS(cls->class_object(state), cls);
    TS_ASSERT_EQUALS(cls->superclass, vm.globals.module);
    check_const(klass, "Class");
  }

  void test_metaclass_is_completely_setup() {
    VM vm(1024);
    Class *cls;
    MetaClass *meta;

    cls = (Class*)vm.globals.klass;
    meta = (MetaClass*)cls->klass;
    TS_ASSERT(MetaClass::is_a(vm.globals.object->klass));
    TS_ASSERT(LookupTable::is_a(meta->method_table));
    TS_ASSERT(LookupTable::is_a(meta->constants));
  }

  void test_module() {
    VM vm(1024);
    Class *mod;

    mod = (Class*)vm.globals.module;
    TS_ASSERT_EQUALS(mod->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(mod->superclass, vm.globals.object);
    check_const(module, "Module");
  }

  void test_metaclass() {
    VM vm(1024);
    Class *meta;

    meta = vm.globals.metaclass;

    TS_ASSERT_EQUALS(meta->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(meta->superclass, vm.globals.klass);
    TS_ASSERT_EQUALS(meta->instance_type, Object::i2n(MetaclassType));
    check_const(metaclass, "MetaClass");
  }

  void test_tuple() {
    VM vm(1024);
    Class *tup;

    tup = vm.globals.tuple;

    TS_ASSERT_EQUALS(tup->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(tup->superclass, vm.globals.object);
    TS_ASSERT_EQUALS(tup->instance_type, Object::i2n(TupleType));
    check_const(tuple, "Tuple");
  }

  void test_lookuptable() {
    VM vm(1024);
    Class *lt;

    lt = vm.globals.lookuptable;

    TS_ASSERT_EQUALS(lt->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(lt->superclass, vm.globals.object);
    TS_ASSERT_EQUALS(lt->instance_type, Object::i2n(LookupTableType));
    check_const(lookuptable, "LookupTable");
  }

  void test_methtbl() {
    VM vm(1024);
    Class *cls;

    cls = vm.globals.methtbl;

    TS_ASSERT_EQUALS(cls->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(cls->superclass, vm.globals.lookuptable);

    TS_ASSERT_EQUALS((object_type)cls->instance_type->n2i(), MTType);
    check_const(methtbl, "MethodTable");
  }

  void test_symbol() {
    VM vm(1024);
    Class *cls;

    cls = vm.globals.symbol;

    TS_ASSERT_EQUALS(cls->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(cls->superclass, vm.globals.object);
    check_const(symbol, "Symbol");
  }

  void test_array() {
    VM vm(1024);
    Class *cls;

    cls = vm.globals.array;

    TS_ASSERT_EQUALS(cls->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(cls->superclass, vm.globals.object);
    check_const(array, "Array");
  }

  void test_bytearray() {
    VM vm(1024);
    Class *cls;

    cls = vm.globals.bytearray;

    TS_ASSERT_EQUALS(cls->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(cls->superclass, vm.globals.object);
    TS_ASSERT_EQUALS((object_type)cls->instance_type->n2i(), ByteArrayType);
    check_const(bytearray, "ByteArray");
  }

  void test_string() {
    VM vm(1024);
    Class *cls;

    cls = vm.globals.string;

    TS_ASSERT_EQUALS(cls->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(cls->superclass, vm.globals.object);
    TS_ASSERT_EQUALS((object_type)cls->instance_type->n2i(), StringType);
    check_const(string, "String");
  }

  void test_symtbl() {
    VM vm(1024);
    Class *cls;

    cls = vm.globals.symtbl;

    TS_ASSERT_EQUALS(cls->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(cls->superclass, vm.globals.object);
    check_const(symtbl, "SymbolTable");
  }

  void test_cmethod() {
    VM vm(1024);
    Class *cls;

    cls = vm.globals.cmethod;

    TS_ASSERT_EQUALS(cls->class_object(state), vm.globals.klass);
    TS_ASSERT_EQUALS(cls->superclass, vm.globals.executable);
    check_const(cmethod, "CompiledMethod");
  }
};

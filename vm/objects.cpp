#include "objects.hpp"
#include "objectmemory.hpp"
#include "vm.hpp"

namespace rubinius {

  /* State is a VM* so, we can just use this in here */
  #define state this

  Class *VM::new_basic_class(OBJECT sup, size_t fields) {
    Class *cls = (Class*)om->new_object(globals.klass, Class::fields);
    cls->instance_fields = Object::i2n(fields);
    cls->has_ivars = Qtrue;
    cls->object_type = Object::i2n(ObjectType);
    SET(cls, superclass, sup);

    return cls;
  }

  Class *VM::new_class(OBJECT sup, size_t fields) {
    Class *cls = new_basic_class(sup, fields);
    MetaClass::attach(this, cls);
    return cls;
  }

  /* Creates the rubinius object universe from scratch. */
  void VM::bootstrap_ontology() {
    /* Class is created first by hand, and twittle to setup the internal
       recursion. */

    Class *cls = (Class*)om->new_object(Qnil, Class::fields);
    cls->klass = cls;

    cls->instance_fields = Object::i2n(Class::fields);
    cls->has_ivars = Qtrue;
    cls->object_type = Object::i2n(ClassType);
    cls->obj_type = ClassType;

    globals.klass = cls;

    Class *object = new_basic_class(Qnil, NormalObject::fields);
    globals.object = object;

    globals.module = new_basic_class(object, Module::fields);
    cls->superclass = globals.module;

    globals.metaclass = new_basic_class(cls, MetaClass::fields);
    globals.metaclass->object_type = Object::i2n(MetaclassType);

    globals.tuple = new_basic_class(object, Tuple::fields);
    globals.tuple->object_type = Object::i2n(TupleType);

    globals.lookuptable = new_basic_class(object, LookupTable::fields);
    globals.lookuptable->object_type = Object::i2n(LookupTableType);

    globals.methtbl = new_basic_class(globals.lookuptable, MethodTable::fields);
    globals.methtbl->object_type = Object::i2n(MTType);

    OBJECT mc = MetaClass::attach(this, object, cls);
    mc = MetaClass::attach(this, globals.module, mc);
    MetaClass::attach(this, cls, mc);

    MetaClass::attach(this, globals.metaclass);
    MetaClass::attach(this, globals.tuple);
    MetaClass::attach(this, globals.lookuptable);
    MetaClass::attach(this, globals.methtbl);

    globals.symbol    = new_class(object, 0);
    globals.array     = new_class(object, Array::fields);
    globals.bytearray = new_class(object, 0);
    globals.bytearray->object_type = Object::i2n(ByteArrayType);
    globals.string    = new_class(object, String::fields);
    globals.string->object_type = Object::i2n(StringType);
    globals.symtbl    = new_class(object, SymbolTable::fields);

    globals.cmethod   = new_class(object, CompiledMethod::fields);
    globals.cmethod->object_type = Object::i2n(CMethodType);

    globals.hash      = new_class(object, Hash::fields);
    globals.hash->object_type = Object::i2n(HashType);

    globals.symbols = SymbolTable::create(this);
  }
};

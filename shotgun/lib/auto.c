#include "rubinius.h"
OBJECT cmethod_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(cmethod), 12 + extra);
}

OBJECT tuple_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(tuple), 0 + extra);
}

OBJECT array_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(array), 2 + extra);
}

OBJECT blank_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(blank), 0 + extra);
}

OBJECT symbol_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(symbol), 0 + extra);
}

OBJECT methtbl_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(methtbl), 6 + extra);
}

OBJECT bytearray_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(bytearray), 0 + extra);
}

OBJECT metaclass_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(metaclass), 9 + extra);
}

OBJECT methctx_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(methctx), 13 + extra);
}

OBJECT class_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(class), 8 + extra);
}

OBJECT io_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(io), 1 + extra);
}

OBJECT hash_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(hash), 6 + extra);
}

OBJECT string_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(string), 4 + extra);
}

OBJECT symtbl_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(symtbl), 3 + extra);
}

OBJECT blokctx_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(blokctx), 9 + extra);
}

OBJECT module_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(module), 5 + extra);
}

OBJECT object_allocate_with_extra(STATE, int extra) {
  return NEW_OBJECT(BASIC_CLASS(object), 1 + extra);
}


/* Code to create the types into classes */

OBJECT _cmethod_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(12));
   class_set_instance_flags(cls, I2N(0x02));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _cmethod_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _cmethod_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _tuple_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(0));
   class_set_instance_flags(cls, I2N(0));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _tuple_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _tuple_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _array_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(2));
   class_set_instance_flags(cls, I2N(0));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _array_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _array_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _blank_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(0));
   class_set_instance_flags(cls, I2N(0));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _blank_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _blank_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _symbol_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(0));
   class_set_instance_flags(cls, I2N(0));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _symbol_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _symbol_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _methtbl_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(6));
   class_set_instance_flags(cls, I2N(0x02));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _methtbl_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _methtbl_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _bytearray_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(0));
   class_set_instance_flags(cls, I2N(0));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _bytearray_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _bytearray_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _metaclass_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(9));
   class_set_instance_flags(cls, I2N(0x02));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _metaclass_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _metaclass_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _methctx_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(13));
   class_set_instance_flags(cls, I2N(0x02));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _methctx_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _methctx_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _class_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(8));
   class_set_instance_flags(cls, I2N(0x02));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _class_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _class_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _io_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(1));
   class_set_instance_flags(cls, I2N(0));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _io_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _io_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _hash_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(6));
   class_set_instance_flags(cls, I2N(0x02));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _hash_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _hash_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _string_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(4));
   class_set_instance_flags(cls, I2N(0));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _string_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _string_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _symtbl_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(3));
   class_set_instance_flags(cls, I2N(0x02));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _symtbl_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _symtbl_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _blokctx_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(9));
   class_set_instance_flags(cls, I2N(0x02));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _blokctx_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _blokctx_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _module_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(5));
   class_set_instance_flags(cls, I2N(0x02));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _module_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _module_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}

OBJECT _object_basic_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = class_allocate_with_extra(state, 0);
   class_set_instance_fields(cls, I2N(1));
   class_set_instance_flags(cls, I2N(0x02));
   class_set_superclass(cls, sup);
   return cls;
}

OBJECT _object_class(STATE, OBJECT sup) {
   OBJECT cls;
   cls = _object_basic_class(state, sup);
   object_create_metaclass(state, cls, (OBJECT)0);
   return cls;
}


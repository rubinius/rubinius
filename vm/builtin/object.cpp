#include "builtin/object.hpp"
#include "builtin/bignum.hpp"
#include "builtin/class.hpp"
#include "builtin/compactlookuptable.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/symbol.hpp"
#include "builtin/string.hpp"
#include "builtin/tuple.hpp"
#include "builtin/array.hpp"
#include "builtin/selector.hpp"
#include "objectmemory.hpp"
#include "global_cache.hpp"
#include "config.hpp"

#include <cstring>

namespace rubinius {

  bool Object::fixnum_p() {
    return FIXNUM_P(this);
  }

  bool Object::symbol_p() {
    return SYMBOL_P(this);
  }

  /* Initialize the objects data with the most basic info. This is done
   * right after an object is created. */
  void Object::init(gc_zone loc, size_t fields) {
    all_flags = 0;
    zone = loc;
    field_count = fields;
  }

  /* Clear the body of the object, by setting each field to Qnil */
  void Object::clear_fields() {
    ivars = Qnil;
    for(size_t i = 0; i < field_count; i++) {
      field[i] = Qnil;
    }
  }

  /* Initialize the object as storing bytes, by setting the flag then clearing the
   * body of the object, by setting the entire body as bytes to 0 */
  void Object::init_bytes() {
    this->StoresBytes = 1;
    std::memset((void*)(this->field), field_count * sizeof(OBJECT), 0);
  }

  size_t Object::size_in_bytes() {
    return SIZE_IN_BYTES(this);
  }

  size_t Object::body_in_bytes() {
    return field_count * sizeof(OBJECT);
  }

  bool Object::reference_p() {
    return REFERENCE_P(this);
  }

  bool Object::stores_bytes_p() {
    return StoresBytes;
  }

  bool Object::stores_references_p() {
    return !StoresBytes;
  }

  bool Object::young_object_p() {
    return zone == YoungObjectZone;
  }

  bool Object::mature_object_p() {
    return zone == MatureObjectZone;
  }

  bool Object::forwarded_p() {
    return Forwarded == 1;
  }

  void Object::set_forward(OBJECT fwd) {
    assert(zone == YoungObjectZone);
    Forwarded = 1;
    klass = (Class*)fwd;
  }

  OBJECT Object::forward() {
    return (OBJECT)klass;
  }

  bool Object::marked_p() {
    return Marked == 1;
  }

  void Object::mark() {
    Marked = 1;
  }

  void Object::clear_mark() {
    Marked = 0;
  }

  bool Object::nil_p() {
    return this == Qnil;
  }

  bool Object::undef_p() {
    return this == Qundef;
  }

  bool Object::true_p() {
    return this == Qtrue;
  }

  bool Object::false_p() {
    return this == Qfalse;
  }

  bool Object::has_ivars_p() {
    return TRUE;
  }

  bool Object::check_type(object_type type) {
    return reference_p() && obj_type == type;
  }

  // Safely return the object type, even if the receiver is an immediate
  object_type Object::type() {
    if(reference_p()) return obj_type;
    if(fixnum_p()) return FixnumType;
    if(symbol_p()) return SymbolType;
    if(nil_p()) return NilType;
    if(true_p()) return TrueType;
    if(false_p()) return FalseType;
    return ObjectType;
  }

  OBJECT Object::tainted_p() {
    if(reference_p() && this->IsTainted) {
      return Qtrue;
    } else {
      return Qfalse;
    }
  }

  OBJECT Object::taint() {
    if(reference_p()) {
      this->IsTainted = TRUE;
    }
    return this;
  }

  OBJECT Object::untaint() {
    if(reference_p()) {
      this->IsTainted = FALSE;
    }
    return this;
  }

  OBJECT Object::freeze() {
    if(reference_p()) {
      this->IsFrozen = TRUE;
    }
    return this;
  }

  OBJECT Object::frozen_p() {
    if(reference_p() && this->IsFrozen) {
      return Qtrue;
    } else {
      return Qfalse;
    }
  }

  const char* Object::type_to_name(object_type t) {
    const char* type;

    switch(t) {
    case ObjectType:
      type = "Object";
      break;
    case MContextType:
      type = "MethodContext";
      break;
    case BContextType:
      type = "BlockContext";
      break;
    case ClassType:
      type = "Class";
      break;
    case MetaclassType:
      type = "Metaclass";
      break;
    case MTType:
      type = "MethodTable";
      break;
    case WrapsStructType:
      type = "SubtendCStructure";
      break;
    case IncModType:
      type = "included Module";
      break;
    case TaskType:
      type = "Task";
      break;
    case FixnumType:
      type = "Fixnum";
      break;
    case BignumType:
      type = "Bignum";
      break;
    case FloatType:
      type = "Float";
      break;
    case MemPtrType:
      type = "MemoryPointer";
      break;
    case StringType:
      type = "String";
      break;
    case SymbolType:
      type = "Symbol";
      break;
    case CMethodType:
      type = "CompiledMethod";
      break;
    case NMethodType:
      type = "NativeMethod";
      break;
    case NilType:
      type = "nil";
      break;
    case LookupTableType:
      type = "LookupTable";
      break;
    case DirType:
      type = "Dir";
      break;
    case CompactLookupTableType:
      type = "CompactLookupTable";
      break;
    case TimeType:
      type = "Time";
      break;
    default:
      type = "unknown";
      break;
    }
    return type;
  }

  Class* Object::class_object(STATE) {
    if(reference_p()) {
      Class* cls = klass;
      while(!cls->nil_p() && !instance_of<Class>(cls)) {
        cls = as<Class>(cls->superclass);
      }

      return cls;
    }

    return state->globals.special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
  }

  Class* Object::lookup_begin(STATE) {
    if(reference_p()) return klass;
    return class_object(state);
  }

  OBJECT Object::dup(STATE) {
    OBJECT dup;

    // We will use lookup_begin here instead of class_object in order
    // to preserve any IncludedModule instances; they will be shared
    // between all duplicates made from this object
    // TODO - Verify this statement
    dup = state->om->new_object(lookup_begin(state), field_count);
    dup->all_flags = all_flags;

    if(stores_bytes_p()) {
      std::memcpy(dup->bytes, bytes, field_count * sizeof(OBJECT));
    } else {
      for(size_t i = 0; i < field_count; i++) {
        state->om->store_object(dup, i, field[i]);
      }
    }
    // TODO - Duplicate ('make independent') ivars here

    return dup;
  }

  OBJECT Object::clone(STATE) {
    LookupTable* source_methods = this->metaclass(state)->method_table->dup(state);
    LookupTable* source_constants = this->metaclass(state)->constants->dup(state);
    OBJECT new_object = this->dup(state);

    // Clone gets a new MetaClass
    SET(new_object, klass, (MetaClass*)state->new_object(G(metaclass)));
    // Set the clone's method and constants tables to those
    // of the receiver's metaclass
    SET(new_object->klass, method_table, source_methods);
    SET(new_object->klass, constants, source_constants);

    return new_object;
  }

  bool Object::kind_of_p(STATE, OBJECT cls) {
    Class* found = class_object(state);
    if(found == cls) return true;

    while(!found->nil_p()) {
      found = (Class*)found->superclass;
      if(found == cls) return true;

      if(found->reference_p() && found->obj_type == IncModType) {
        if(((IncludedModule*)found)->module == cls) return true;
      }
    }

    return false;
  }

  OBJECT Object::kind_of_prim(STATE, Module* klass) {
    return kind_of_p(state, klass) ? Qtrue : Qfalse;
  }

  hashval Object::hash(STATE) {
    hashval hsh;
    hsh = (hashval)(uintptr_t)this;

    if(!reference_p()) {
      /* Get rid of the tag part (i.e. the part that indicate nature of self */
      if(fixnum_p()) {
        native_int val = as<Integer>(this)->to_native();
        /* We do this so the 2's complement will fit into 29 bits properly. */
        if(val < 0) {
          hsh = hsh >> 1;
        }
      }
      hsh = hsh >> 2;
    } else {
      if(kind_of_p(state, G(string))) {
        hsh = ((String*)this)->hash_string(state);
      } else if(kind_of_p(state, G(bignum))) {
        hsh = ((Bignum*)this)->hash_bignum(state);
      } else if(kind_of_p(state, G(floatpoint))) {
        hsh = String::hash_str((unsigned char *)(this->bytes), sizeof(double));
      } else {
        hsh = id(state)->to_native();
      }
    }

    return hsh;
  }

  FIXNUM Object::hash_prim(STATE) {
    return Fixnum::from(hash(state));
  }

  INTEGER Object::id(STATE) {
    if(reference_p()) {
      OBJECT id;

      Class* meta = metaclass(state);
      id =   meta->get_ivar(state, G(sym_object_id));

      /* Lazy allocate object's ids, since most don't need them. */
      if(id->nil_p()) {
        /* All references have an even object_id. last_object_id starts out at 0
         * but we don't want to use 0 as an object_id, so we just add before using */
        id = Fixnum::from(state->om->last_object_id += 2);
        meta->set_ivar(state, G(sym_object_id), id);
      }

      return as<Integer>(id);
    } else {
      /* All non-references have an odd object_id */
      return Fixnum::from(((uintptr_t)this << 1) | 1);
    }
  }

  OBJECT Object::equal(STATE, OBJECT other) {
    return this == other ? Qtrue : Qfalse;
  }

  Class* Object::metaclass(STATE) {
    if(reference_p()) {
      if(kind_of<MetaClass>(klass)) {
        return as<MetaClass>(klass);
      }
      return MetaClass::attach(state, this);
    }

    return class_object(state);
  }

  OBJECT Object::get_ivars(STATE) {
    return ivars;
  }

  OBJECT Object::get_ivar(STATE, OBJECT sym) {
    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(tbl) return tbl->fetch(state, sym);
      return Qnil;
    }

    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars)) {
      return tbl->fetch(state, sym);
    } else if(LookupTable* tbl = try_as<LookupTable>(ivars)) {
      return tbl->fetch(state, sym);
    }

    return Qnil;
  }

  OBJECT Object::set_ivar(STATE, OBJECT sym, OBJECT val) {
    LookupTable* tbl;

    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(!tbl) {
        tbl = LookupTable::create(state);
        G(external_ivars)->store(state, this, tbl);
      }
      tbl->store(state, sym, val);
      return val;
    }

    /* Lazy creation of a lookuptable to store instance variables. */
    if(ivars->nil_p()) {
      CompactLookupTable* tbl = CompactLookupTable::create(state);
      SET(this, ivars, tbl);
      tbl->store(state, sym, val);
      return val;
    }

    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars)) {
      if(tbl->store(state, sym, val) == Qtrue) {
        return val;
      }

      /* No more room in the CompactLookupTable. */
      ivars = tbl->to_lookuptable(state);
      SET(this, ivars, ivars);
    }

    try_as<LookupTable>(ivars)->store(state, sym, val);
    return val;
  }

  void inspect(STATE, OBJECT obj) {
    String* name = obj->class_object(state)->name->to_str(state);
    std::cout << "#<" << (char*)*name << ":" << (void*)obj << ">\n";
  }

  void inspect(STATE, SYMBOL sym) {
    String* name = sym->to_str(state);
    std::cout << ":" << (char*)*name << "\n";
  }

  void Object::cleanup(STATE) {
    state->om->find_type_info(this)->cleanup(this);
  }

  void Object::copy_flags(STATE, OBJECT source) {
    this->obj_type        = source->obj_type;
    this->StoresBytes     = source->StoresBytes;
    this->RequiresCleanup = source->RequiresCleanup;
    this->IsBlockContext  = source->IsBlockContext;
    this->IsMeta          = source->IsMeta;
  }

  // 'virtual' methods. They dispatch to the object's TypeInfo
  // object to perform the work.
  OBJECT Object::get_field(STATE, size_t index) {
    return state->om->type_info[type()]->get_field(state, this, index);
  }

  void Object::set_field(STATE, size_t index, OBJECT val) {
    state->om->type_info[type()]->set_field(state, this, index, val);
  }

  OBJECT Object::show(STATE) {
    state->om->type_info[type()]->show(state, this);
    return Qnil;
  }

  /* VM level primitives. This is a silly place, I know. */
  OBJECT Object::vm_get_config_item(STATE, String* var) {
    ConfigParser::Entry* ent = state->user_config->find(var->byte_address());
    if(!ent) return Qnil;

    if(ent->is_number()) {
      return Fixnum::from(atoi(ent->value.c_str()));
    }
    return String::create(state, ent->value.c_str());
  }

  OBJECT Object::vm_get_config_section(STATE, String* section) {
    ConfigParser::EntryList* list;

    list = state->user_config->get_section(section->byte_address());

    Array* ary = Array::create(state, list->size());
    for(size_t i = 0; i < list->size(); i++) {
      String* var = String::create(state, list->at(i)->variable.c_str());
      String* val = String::create(state, list->at(i)->value.c_str());

      ary->set(state, i, Tuple::from(state, 2, var, val));
    }

    return ary;
  }

  OBJECT Object::vm_write_error(STATE, String* str) {
    std::cerr << str->byte_address() << std::endl;
    return Qnil;
  }

  // Clears the global cache for all methods named +name+,
  // also clears all sendsite caches matching that name.
  OBJECT Object::vm_reset_method_cache(STATE, SYMBOL name) {
    // 1. clear the global cache
    state->global_cache->clear(name);
    // 2. clear the send site caches
    Selector::clear_by_name(state, name);
    return name;
  }

  Object* Object::yield_gdb(STATE, Object* obj) {
    obj->show(state);
    Assertion::raise("yield_gdb called and not caught");
    return obj;
  }
}

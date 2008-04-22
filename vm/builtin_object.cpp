#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cstring>

namespace rubinius {

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
    default:
      type = "unknown";
      break;
    }
    return type;
  }

  OBJECT Object::ui2n(STATE, unsigned int num) {
    /* No need to check what 'num' is if it will always fit into a Fixnum */
#if (CONFIG_WORDSIZE != 64)
    if(num > FIXNUM_MAX) {
      /* Number is too big for Fixnum. Use Bignum. */
      return Bignum::new_unsigned(state, num);
    }
#endif
    return APPLY_TAG((native_int)num, TAG_FIXNUM);
  }

  OBJECT Object::i2n(STATE, native_int num) {
    if(num > FIXNUM_MAX || num < FIXNUM_MIN) {
      return Bignum::create(state, num);
    } else {
      return APPLY_TAG(num, TAG_FIXNUM);
    }
  }

  OBJECT Object::ll2n(STATE, long long num) {
    if(num > FIXNUM_MAX || num < FIXNUM_MIN) {
      return Bignum::from_ll(state, num);
    } else {
      return APPLY_TAG(num, TAG_FIXNUM);
    }
  }

  OBJECT Object::ull2n(STATE, unsigned long long num) {
    if(num > FIXNUM_MAX) {
      return Bignum::from_ull(state, num);
    } else {
      return APPLY_TAG(num, TAG_FIXNUM);
    }
  }

  Class* Object::class_object(STATE) {
    if(reference_p()) {
      Class* cls = klass;
      while(!cls->nil_p() && !kind_of<Class>(cls)) {
        cls = (Class*)cls->superclass;
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

    dup = state->om->new_object(class_object(state), field_count);
    dup->all_flags = all_flags;

    if(stores_bytes_p()) {
      std::memcpy(dup->bytes, bytes, field_count * sizeof(OBJECT));
    } else {
      for(size_t i = 0; i < field_count; i++) {
        state->om->store_object(dup, i, field[i]);
      }
    }

    return dup;
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

  hashval Object::hash(STATE) {
    hashval hsh;
    hsh = (hashval)(uintptr_t)this;

    if(!reference_p()) {
      /* Get rid of the tag part (i.e. the part that indicate nature of self */
      if(fixnum_p()) {
        native_int val = n2i();
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
        hsh = id(state);
      }
    }

    return hsh;
  }

  uintptr_t Object::id(STATE) {
    if(reference_p()) {
      OBJECT id;

      Class* meta = metaclass(state);
      id =   meta->get_ivar(state, G(sym_object_id));

      /* Lazy allocate object's ids, since most don't need them. */
      if(id->nil_p()) {
        /* All references have an even object_id. last_object_id starts out at 0
         * but we don't want to use 0 as an object_id, so we just add before using */
        id = Object::i2n(state->om->last_object_id += 2);
        meta->set_ivar(state, G(sym_object_id), id);
      }

      return (uintptr_t)id;
    } else {
      /* All non-references have an odd object_id */
      return (((uintptr_t)this << 1) | 1);
    }
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

  OBJECT Object::get_ivar(STATE, OBJECT sym) {
    OBJECT val;
    LookupTable* tbl;

    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      LookupTable* tbl = (LookupTable*)G(external_ivars)->fetch(state, this);
      if(!tbl->nil_p()) {
        return tbl->fetch(state, sym);
      }
      return Qnil;
    } else if(!has_ivars_p()) {
      MetaClass* meta = (MetaClass*)metaclass(state);
      LookupTable* tbl = (LookupTable*)meta->has_ivars;

      if(tbl->nil_p()) return Qnil;

      return tbl->fetch(state, sym);
    }

    tbl = (LookupTable*)((NormalObject*)this)->instance_variables;

    /* No table, no ivar! */
    if(tbl->nil_p()) return Qnil;

    /* It's a tuple, use csm */
    if(kind_of<Tuple>(tbl)) {
      return Hash::csm_find(state, (Tuple*)tbl, sym);
    }

    /* It's a normal hash, no problem. */
    val = tbl->fetch(state, sym);
    return val;
  }

  OBJECT Object::set_ivar(STATE, OBJECT sym, OBJECT val) {
    LookupTable *tbl;

    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      tbl = (LookupTable*)G(external_ivars)->fetch(state, this);

      if(tbl->nil_p()) {
        tbl = (LookupTable*)LookupTable::create(state);
        G(external_ivars)->store(state, this, tbl);
      }
      tbl->store(state, sym, val);
      return val;
    } else if(!has_ivars_p()) {
      MetaClass* meta = (MetaClass*)metaclass(state);
      LookupTable* tbl = (LookupTable*)meta->has_ivars;

      if(tbl->nil_p()) {
        tbl = (LookupTable*)LookupTable::create(state);
        SET(meta, has_ivars, tbl);
      }

      tbl->store(state, sym, val);
      return val;
    }

    tbl = (LookupTable*)((NormalObject*)this)->instance_variables;

    /* Lazy creation of hash to store instance variables. */
    if(tbl->nil_p()) {
      Tuple* tup = (Tuple*)Hash::csm_new(state);
      SET((NormalObject*)this, instance_variables, tup);
      Hash::csm_add(state, tup, sym, val);
      return val;
    }

    if(kind_of<Tuple>(tbl)) {
      if(Hash::csm_add(state, (Tuple*)tbl, sym, val) == Qtrue) {
        return val;
      }
      /* csm_add said false, meaning there is no room. We convert
         the csm into a normal hash and use it from now on. */
      tbl = (LookupTable*)Hash::csm_into_lookuptable(state, (Tuple*)tbl);
      SET((NormalObject*)this, instance_variables, tbl);
    }
    tbl->store(state, sym, val);
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
}

#include "prelude.hpp"
#include "object.hpp"
#include "objects.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"

#include <cstring>

namespace rubinius {
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

  OBJECT Object::class_object() {
    OBJECT cls = klass;
    while(cls->reference_p() && (MetaClass::is_a(cls) || !Class::is_a(cls))) {
      cls = ((Class*)cls)->superclass;
    }

    return cls;
  }

  OBJECT Object::dup(STATE) {
    OBJECT dup;

    dup = state->om->new_object(class_object(), field_count);
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

  bool Object::kind_of_p(OBJECT cls) {
    Class* found = (Class*)class_object();
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
      if(kind_of_p(state->globals.string)) {
        hsh = ((String*)this)->hash_string(state);
      } else if(kind_of_p(state->globals.bignum)) {
        hsh = ((Bignum*)this)->hash_bignum(state);
      } else if(kind_of_p(state->globals.floatpoint)) {
        hsh = String::hash_str((unsigned char *)BYTES_OF(this), sizeof(double));
      } else {
        hsh = id(state);
      }
    }

    return hsh;
  }

  uintptr_t Object::id(STATE) {
    if(reference_p()) {
      OBJECT meta, id;

      meta = metaclass(state);
      id =   meta->get_ivar(state, state->globals.sym_object_id);

      /* Lazy allocate object's ids, since most don't need them. */
      if(id->nil_p()) {
        /* All references have an even object_id. last_object_id starts out at 0
         * but we don't want to use 0 as an object_id, so we just add before using */
        id = Object::i2n(state->om->last_object_id += 2);
        meta->set_ivar(state, state->globals.sym_object_id, id);
      }

      return (uintptr_t)id;
    } else {
      /* All non-references have an odd object_id */
      return (((uintptr_t)this << 1) | 1);
    }
  }

  OBJECT Object::metaclass(STATE) {
    if(reference_p()) {
      if(MetaClass::is_a(klass)) {
        return klass;
      }
      return MetaClass::attach(state, this);
    }

    if(nil_p()) {
      return state->globals.nil_class;
    } else if(this == Qtrue) {
      return state->globals.true_class;
    } else if(this == Qfalse) { 
      return state->globals.false_class;
    }

    return Qnil;
  }

  OBJECT Object::get_ivar(STATE, OBJECT sym) {
    OBJECT val;
    LookupTable* tbl;

    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      LookupTable* tbl = (LookupTable*)state->globals.external_ivars->fetch(state, this);
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
    if(Tuple::is_a(tbl)) {
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
      tbl = (LookupTable*)state->globals.external_ivars->fetch(state, this);

      if(tbl->nil_p()) {
        tbl = (LookupTable*)LookupTable::create(state);
        state->globals.external_ivars->store(state, this, tbl);
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

    if(Tuple::is_a(tbl)) {
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
}

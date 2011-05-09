#include <iostream>
#include <sstream>

#include <stdarg.h>

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
#include "builtin/float.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/system.hpp"
#include "builtin/methodtable.hpp"
#include "builtin/packed_object.hpp"
#include "builtin/location.hpp"

#include "objectmemory.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "lookup_data.hpp"
#include "primitives.hpp"
#include "global_cache.hpp"

#include "vm/object_utils.hpp"

namespace rubinius {

  Class* Object::class_object(STATE) const {
    if(reference_p()) {
      Module* mod = klass_;
      while(!mod->nil_p() && !instance_of<Class>(mod)) {
        mod = as<Module>(mod->superclass());
      }

      if(mod->nil_p()) {
        Exception::assertion_error(state, "Object::class_object() failed to find a class");
      }
      return as<Class>(mod);
    }

    return state->globals().special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
  }

  Object* Object::duplicate(STATE) {
    if(!reference_p()) return this;

    Object* other = state->new_object_typed(
        class_object(state), this->total_size(state), type_id());
    return other->copy_object(state, this);
  }

  Object* Object::copy_singleton_class(STATE, Object* other) {
    if(SingletonClass* sc = try_as<SingletonClass>(other->klass())) {
      MethodTable* source_methods = sc->method_table()->duplicate(state);
      LookupTable* source_constants = sc->constant_table()->duplicate(state);

      singleton_class(state)->method_table(state, source_methods);
      singleton_class(state)->constant_table(state, source_constants);
      // TODO inc the global serial here?

      // This allows us to preserve included modules
      singleton_class(state)->superclass(state, sc->superclass());
    }

    return this;
  }

  Object* Object::copy_object_prim(STATE, Object* other, CallFrame* call_frame) {
    if(!reference_p() || !other->reference_p() || type_id() != other->type_id() ||
        class_object(state) != other->class_object(state)) {
      Exception* exc =
        Exception::make_type_error(state, type_id(), other);
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    return copy_object(state, other);
  }

  Object* Object::copy_object(STATE, Object* other) {
    initialize_copy(other, age());

    write_barrier(state, klass());
    write_barrier(state, ivars());

#ifdef RBX_OBJECT_ID_IN_HEADER
    // Don't inherit the object_id from the original.
    set_object_id(0);
#endif

    /* C extensions use Data objects for various purposes. The object
     * usually is made an instance of some extension class. So, we
     * have to check the object type to ensure we don't clobber the
     * data caried in the new instance.
     */
    if(type_id() != DataType) {
      copy_body(state, other);
    }

    // Ensure that the singleton class is not shared
    klass(state, other->class_object(state));

    // HACK: If other is mature, remember it.
    // We could inspect inspect the references we just copied to see
    // if there are any young ones if other is mature, then and only
    // then remember other. The up side to just remembering it like
    // this is that other is rarely mature, and the remember_set is
    // flushed on each collection anyway.
    if(zone() == MatureObjectZone) {
      state->om->remember_object(this);
    }

    // Copy ivars.
    if(other->ivars()->reference_p()) {
      // NOTE Don't combine these 2 branches even though they both just call
      // ::copy. There is a special LookupTable::copy that can only be seen
      // when the receiver is of LookupTable* type. Without the explicit cast
      // and call, the wrong one will be called.
      if(LookupTable* lt = try_as<LookupTable>(other->ivars())) {
        ivars(state, lt->duplicate(state));

        // We store the object_id in the ivar table, so nuke it.
#ifndef RBX_OBJECT_ID_IN_HEADER
        LookupTable* ld = as<LookupTable>(ivars());
        ld->remove(state, G(sym_object_id));
#endif
      } else {
        // Use as<> so that we throw a TypeError if there is something else
        // here.
        CompactLookupTable* clt = as<CompactLookupTable>(other->ivars());
        ivars(state, clt->duplicate(state));

        // We store the object_id in the ivar table, so nuke it.
#ifndef RBX_OBJECT_ID_IN_HEADER
        CompactLookupTable* ld = as<CompactLookupTable>(ivars());
        ld->remove(state, G(sym_object_id));
#endif
      };
    }

    return this;
  }

  Object* Object::equal(STATE, Object* other) {
    return this == other ? Qtrue : Qfalse;
  }

  Object* Object::freeze(STATE) {
    if(reference_p()) set_frozen();
    return this;
  }

  Object* Object::frozen_p(STATE) {
    if(reference_p() && is_frozen_p()) return Qtrue;
    return Qfalse;
  }

  Object* Object::get_field(STATE, size_t index) {
    return type_info(state)->get_field(state, this, index);
  }

  Object* Object::get_table_ivar(STATE, Symbol* sym) {
    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars())) {
      return tbl->fetch(state, sym);
    } else if(LookupTable* tbl = try_as<LookupTable>(ivars())) {
      return tbl->fetch(state, sym);
    }

    return Qnil;
  }

  Object* Object::table_ivar_defined(STATE, Symbol* sym) {
    bool found = false;
    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars())) {
      tbl->fetch(state, sym, &found);
    } else if(LookupTable* tbl = try_as<LookupTable>(ivars())) {
      tbl->fetch(state, sym, &found);
    }

    if(found) return Qtrue;
    return Qfalse;
  }

  Object* Object::get_ivar_prim(STATE, Symbol* sym) {
    if(sym->is_ivar_p(state)->false_p()) {
      return reinterpret_cast<Object*>(kPrimitiveFailed);
    }

    return get_ivar(state, sym);
  }

  Object* Object::get_ivar(STATE, Symbol* sym) {
    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(tbl) return tbl->fetch(state, sym);
      return Qnil;
    }

    switch(type_id()) {
    case Object::type:
      return get_table_ivar(state, sym);
    case PackedObject::type:
      {
        LookupTable* tbl = this->reference_class()->packed_ivar_info();

        LookupTableBucket* entry = tbl->find_entry(state, sym);
        if(entry->nil_p()) {
          return get_table_ivar(state, sym);
        }

        Fixnum* which = try_as<Fixnum>(entry->value());

        Object** baa = reinterpret_cast<Object**>(pointer_to_body());
        Object* obj = baa[which->to_native()];
        if(obj == Qundef) return Qnil;
        return obj;
      }
    default:
      break;
    }

    /*
    // Handle packed objects in a unique way.
    if(PackedObject* po = try_as<PackedObject>(this)) {
      return po->get_packed_ivar(state, sym);
    }
    */

    // We might be trying to access a slot, so try that first.

    TypeInfo* ti = state->om->find_type_info(this);
    if(ti) {
      TypeInfo::Slots::iterator it = ti->slots.find(sym->index());
      if(it != ti->slots.end()) {
        return ti->get_field(state, this, it->second);
      }
    }

    return get_table_ivar(state, sym);
  }

  Object* Object::ivar_defined_prim(STATE, Symbol* sym) {
    if(!sym->is_ivar_p(state)->true_p()) {
      return reinterpret_cast<Object*>(kPrimitiveFailed);
    }

    return ivar_defined(state, sym);
  }

  Object* Object::ivar_defined(STATE, Symbol* sym) {
    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(tbl) {
        bool found = false;
        tbl->fetch(state, sym, &found);
        if(found) return Qtrue;
      }

      return Qfalse;
    }

    // Handle packed objects in a unique way.
    if(PackedObject* po = try_as<PackedObject>(this)) {
      return po->packed_ivar_defined(state, sym);
    }

    // We don't check slots, because we don't advertise them
    // as normal ivars.

    return table_ivar_defined(state, sym);
  }

  Array* Object::ivar_names(STATE) {
    return ivar_names(state, Array::create(state, 3));
  }

  Array* Object::ivar_names(STATE, Array* ary) {
    // We don't check slots, because we don't advertise them
    // as normal ivars.
    class ivar_match : public ObjectMatcher {
    public:
      virtual bool match_p(STATE, Object* match) {
        if(Symbol* sym = try_as<Symbol>(match)) {
          if(sym->is_ivar_p(state)->true_p()) return true;
        }

        return false;
      }
    } match;

    if(!reference_p()) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));
      if(tbl) {
        tbl->filtered_keys(state, match, ary);
      }
      return ary;
    }

    // Handle packed objects in a unique way.
    if(PackedObject* po = try_as<PackedObject>(this)) {
      po->add_packed_ivars(state, ary);
    }

    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars())) {
      tbl->filtered_keys(state, match, ary);
    } else if(LookupTable* tbl = try_as<LookupTable>(ivars())) {
      tbl->filtered_keys(state, match, ary);
    }

    return ary;
  }

  object_type Object::get_type() const {
    if(reference_p()) return type_id();
    if(fixnum_p()) return FixnumType;
    if(symbol_p()) return SymbolType;
    if(nil_p()) return NilType;
    if(true_p()) return TrueType;
    if(false_p()) return FalseType;
    return ObjectType;
  }

  hashval Object::hash(STATE) {
    if(!reference_p()) {
      return reinterpret_cast<uintptr_t>(this) & FIXNUM_MAX;

#ifdef _LP64
      uintptr_t key = reinterpret_cast<uintptr_t>(this);
      key = (~key) + (key << 21); // key = (key << 21) - key - 1;
      key = key ^ (key >> 24);
      key = (key + (key << 3)) + (key << 8); // key * 265
      key = key ^ (key >> 14);
      key = (key + (key << 2)) + (key << 4); // key * 21
      key = key ^ (key >> 28);
      key = key + (key << 31);
      return key & FIXNUM_MAX;
#else
      // See http://burtleburtle.net/bob/hash/integer.html
      uint32_t a = (uint32_t)this;
      a = (a+0x7ed55d16) + (a<<12);
      a = (a^0xc761c23c) ^ (a>>19);
      a = (a+0x165667b1) + (a<<5);
      a = (a+0xd3a2646c) ^ (a<<9);
      a = (a+0xfd7046c5) + (a<<3);
      a = (a^0xb55a4f09) ^ (a>>16);
      return a & FIXNUM_MAX;
#endif
    } else {
      if(String* string = try_as<String>(this)) {
        return string->hash_string(state);
      } else if(Bignum* bignum = try_as<Bignum>(this)) {
        return bignum->hash_bignum(state);
      } else if(Float* flt = try_as<Float>(this)) {
        return String::hash_str((unsigned char *)(&(flt->val)), sizeof(double));
      } else {
        return id(state)->to_native();
      }
    }
  }

  Integer* Object::hash_prim(STATE) {
    return Integer::from(state, hash(state));
  }

  Integer* Object::id(STATE) {
    if(reference_p()) {
#ifdef RBX_OBJECT_ID_IN_HEADER
      if(object_id() == 0) {
        set_object_id(++state->om->last_object_id);
      }

      // Shift it up so we don't waste the numeric range in the actual
      // storage, but still present the id as always modulo 4, so it doesn't
      // collide with the immediates, since immediates never have a tag
      // ending in 00.
      return Integer::from(state, object_id() << TAG_REF_WIDTH);
#else
      Object* id = get_ivar(state, G(sym_object_id));

      /* Lazy allocate object's ids, since most don't need them. */
      if(id->nil_p()) {
        /* All references have an even object_id. last_object_id starts out at 0
         * but we don't want to use 0 as an object_id, so we just add before using */
        id = Integer::from(state, ++state->om->last_object_id << TAG_REF_WIDTH);
        set_ivar(state, G(sym_object_id), id);
      }

      return as<Integer>(id);
#endif
    } else {
      /* All non-references have the pointer directly as the object id */
      return Integer::from(state, (uintptr_t)this);
    }
  }

  bool Object::has_id(STATE) {
    if(!reference_p()) return true;

#ifdef RBX_OBJECT_ID_IN_HEADER
    return object_id() > 0;
#else
    Object* id = get_ivar(state, G(sym_object_id));
    return !id->nil_p();
#endif
  }

  void Object::infect(STATE, Object* other) {
    if(this->tainted_p(state) == Qtrue) {
      other->taint(state);
    }
  }

  bool Object::kind_of_p(STATE, Object* module) {
    Module* found = NULL;

    if(!reference_p()) {
      found = state->globals().special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
    } else {
      found = try_as<Module>(klass_);
    }

    while(found) {
      if(found == module) return true;

      if(IncludedModule* im = try_as<IncludedModule>(found)) {
        if(im->module() == module) return true;
      }

      found = try_as<Module>(found->superclass());
    }

    return false;
  }

  Object* Object::kind_of_prim(STATE, Module* klass) {
    return kind_of_p(state, klass) ? Qtrue : Qfalse;
  }

  Object* Object::instance_of_prim(STATE, Module* klass) {
    return class_object(state) == klass ? Qtrue : Qfalse;
  }

  Class* Object::singleton_class(STATE) {
    if(reference_p()) {
      if(SingletonClass* sc = try_as<SingletonClass>(klass())) {
        /* This test is very important! SingletonClasses can get their klass()
         * hooked up to the SingletonClass of a parent class, so that the MOP
         * works properly. BUT we should not return that parent singleton
         * class, we need to only return a SingletonClass that is for this!
         */
        if(sc->attached_instance() == this) return sc;
      }
      return SingletonClass::attach(state, this);
    }

    return class_object(state);
  }

  Object* Object::send(STATE, CallFrame* caller, Symbol* name, Array* ary,
      Object* block, bool allow_private) {
    LookupData lookup(this, this->lookup_begin(state), allow_private);
    Dispatch dis(name);

    Arguments args(ary);
    args.set_block(block);
    args.set_recv(this);

    return dis.send(state, caller, lookup, args);
  }

  Object* Object::send(STATE, CallFrame* caller, Symbol* name, bool allow_private) {
    LookupData lookup(this, this->lookup_begin(state), allow_private);
    Dispatch dis(name);

    Arguments args;
    args.set_block(Qnil);
    args.set_recv(this);

    return dis.send(state, caller, lookup, args);
  }

  Object* Object::send_prim(STATE, Executable* exec, CallFrame* call_frame, Dispatch& msg,
                            Arguments& args) {
    if(args.total() < 1) return Primitives::failure();

    // Don't shift the argument because we might fail and we need Arguments
    // to be pristine in the fallback code.
    Object* meth = args.get_argument(0);
    Symbol* sym = try_as<Symbol>(meth);

    if(!sym) {
      if(String* str = try_as<String>(meth)) {
        sym = str->to_sym(state);
      } else {
        return Primitives::failure();
      }
    }

    // Discard the 1st argument.
    args.shift(state);

    Dispatch dis(sym);
    LookupData lookup(this, this->lookup_begin(state), true);

    return dis.send(state, call_frame, lookup, args);
  }

  void Object::set_field(STATE, size_t index, Object* val) {
    type_info(state)->set_field(state, this, index, val);
  }

  Object* Object::set_table_ivar(STATE, Symbol* sym, Object* val) {
    /* Lazy creation of a lookuptable to store instance variables. */
    if(ivars()->nil_p()) {
      CompactLookupTable* tbl = CompactLookupTable::create(state);
      ivars(state, tbl);
      tbl->store(state, sym, val);
      return val;
    }

    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars())) {
      if(tbl->store(state, sym, val) == Qtrue) {
        return val;
      }

      /* No more room in the CompactLookupTable. */
      ivars(state, tbl->to_lookuptable(state));
    }

    if(LookupTable* tbl = try_as<LookupTable>(ivars())) {
      tbl->store(state, sym, val);
    }
    /* else.. what? */

    return val;
  }

  Object* Object::set_ivar_prim(STATE, Symbol* sym, Object* val) {
    if(sym->is_ivar_p(state)->false_p()) {
      return reinterpret_cast<Object*>(kPrimitiveFailed);
    }

    return set_ivar(state, sym, val);
  }

  Object* Object::set_ivar(STATE, Symbol* sym, Object* val) {
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

    if(type_id() == Object::type) return set_table_ivar(state, sym, val);

    // Handle packed objects in a unique way.
    if(PackedObject* po = try_as<PackedObject>(this)) {
      return po->set_packed_ivar(state, sym, val);
    }

    /* We might be trying to access a field, so check there first. */
    TypeInfo* ti = state->om->find_type_info(this);
    if(ti) {
      TypeInfo::Slots::iterator it = ti->slots.find(sym->index());
      if(it != ti->slots.end()) {
        ti->set_field(state, this, it->second, val);
        return val;
      }
    }

    return set_table_ivar(state, sym, val);
  }

  Object* Object::del_ivar(STATE, Symbol* sym) {
    LookupTable* tbl;
    bool removed = false;

    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(tbl) {
        Object* val = tbl->remove(state, sym, &removed);
        if(removed) return val;
      }

      return Primitives::failure();
    }

    // Handle packed objects in a unique way.
    if(PackedObject* po = try_as<PackedObject>(this)) {
      Object* val = po->packed_ivar_delete(state, sym, &removed);
      if(removed) return val;
      return Primitives::failure();
    }

    /* We might be trying to access a field, so check there first. */
    TypeInfo* ti = state->om->find_type_info(this);
    if(ti) {
      TypeInfo::Slots::iterator it = ti->slots.find(sym->index());
      // Can't remove a slot, so just bail.
      if(it != ti->slots.end()) return Qnil;
    }

    Object* val = del_table_ivar(state, sym, &removed);
    if(removed) return val;
    return Primitives::failure();
  }

  Object* Object::del_table_ivar(STATE, Symbol* sym, bool* removed) {
    /* No ivars, we're done! */
    if(ivars()->nil_p()) return Qnil;

    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars())) {
      return tbl->remove(state, sym, removed);
    } else if(LookupTable* tbl = try_as<LookupTable>(ivars())) {
      return tbl->remove(state, sym, removed);
    }
    return Qnil;
  }

  String* Object::to_s(STATE, bool address) {
    std::stringstream name;

    if(!reference_p()) {
      if(nil_p()) return String::create(state, "nil");
      if(true_p()) return String::create(state, "true");
      if(false_p()) return String::create(state, "false");

      if(Fixnum* fix = try_as<Fixnum>(this)) {
        name << fix->to_native();
        return String::create(state, name.str().c_str());
      } else if(Symbol* sym = try_as<Symbol>(this)) {
        name << ":\"" << sym->c_str(state) << "\"";
        return String::create(state, name.str().c_str());
      }
    }

    if(String* str = try_as<String>(this)) {
      return str;
    } else {
      name << "#<";
      if(Module* mod = try_as<Module>(this)) {
        if(mod->name()->nil_p()) {
          name << "Class";
        } else {
          name << mod->name()->c_str(state);
        }
        name << "(" << this->class_object(state)->name()->c_str(state) << ")";
      } else {
        if(this->class_object(state)->name()->nil_p()) {
          name << "Object";
        } else {
          name << this->class_object(state)->name()->c_str(state);
        }
      }
    }

    name << ":";
    if(address) {
      name << reinterpret_cast<void*>(this);
    } else {
      name << "0x" << std::hex << this->id(state)->to_native();
    }
    name << ">";

    return String::create(state, name.str().c_str());
  }

  Object* Object::show(STATE) {
    return show(state, 0);
  }

  Object* Object::show(STATE, int level) {
    type_info(state)->show(state, this, level);
    return Qnil;
  }

  Object* Object::show_simple(STATE) {
    return show_simple(state, 0);
  }

  Object* Object::show_simple(STATE, int level) {
    type_info(state)->show_simple(state, this, level);
    return Qnil;
  }

  Object* Object::taint(STATE) {
    if(reference_p()) set_tainted();
    return this;
  }

  Object* Object::tainted_p(STATE) {
    if(reference_p() && is_tainted_p()) return Qtrue;
    return Qfalse;
  }

  TypeInfo* Object::type_info(STATE) const {
    return state->om->type_info[get_type()];
  }

  Object* Object::untaint(STATE) {
    if(reference_p()) set_tainted(0);
    return this;
  }

  Object* Object::respond_to(STATE, Symbol* name, Object* priv) {
    LookupData lookup(this, lookup_begin(state));
    lookup.priv = RTEST(priv);

    Dispatch dis(name);

    if(!GlobalCache::resolve(state, name, dis, lookup)) {
      return Qfalse;
    }

    return Qtrue;
  }

  Object* Object::respond_to_public(STATE, Object* obj) {
    Symbol* name;

    if(Symbol* sym = try_as<Symbol>(obj)) {
      name = sym;
    } else if(String* str = try_as<String>(obj)) {
      name = str->to_sym(state);
    } else {
      return Primitives::failure();
    }

    LookupData lookup(this, lookup_begin(state));
    lookup.priv = false;

    Dispatch dis(name);

    if(!GlobalCache::resolve(state, name, dis, lookup)) {
      return Qfalse;
    }

    return Qtrue;
  }

  /**
   *  We use void* as the type for obj to work around C++'s type system
   *  that requires full definitions of classes to be present for it
   *  figure out if you can properly pass an object (the superclass
   *  has to be known).
   *
   *  If we have Object* obj here, then we either have to cast to call
   *  write_barrier (which means we lose the ability to have type specific
   *  write_barrier versions, which we do), or we have to include
   *  every header up front. We opt for the former.
   */
  void Object::inline_write_barrier_passed(STATE, void* obj) {
    if(!remembered_p()) {
      state->om->remember_object(this);
    }
  }

  void Object::write_barrier(gc::WriteBarrier* wb, void* obj) {
    wb->write_barrier(this, reinterpret_cast<Object*>(obj));
  }

}

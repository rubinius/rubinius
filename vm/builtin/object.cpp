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
#include "builtin/constantscope.hpp"
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
#include "on_stack.hpp"

#include "configuration.hpp"

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

    Object* other = state->vm()->new_object_typed(
        class_object(state), this->total_size(state->vm()), type_id());
    return other->copy_object(state, this);
  }

  Object* Object::copy_singleton_class(STATE, GCToken gct, Object* other) {
    if(SingletonClass* sc = try_as<SingletonClass>(other->klass())) {
      MethodTable* source_methods = 0;
      LookupTable* source_constants = 0;
      Object* self = this;

      OnStack<4> os(state, self, sc, source_methods, source_constants);

      source_methods = sc->method_table()->duplicate(state, gct);
      source_constants = sc->constant_table()->duplicate(state);

      self->singleton_class(state)->method_table(state, source_methods);
      self->singleton_class(state)->constant_table(state, source_constants);
      // TODO inc the global serial here?

      // This allows us to preserve included modules
      self->singleton_class(state)->superclass(state, sc->superclass());
      return self;
    }

    return this;
  }

  Object* Object::copy_object_prim(STATE, Object* other, CallFrame* call_frame) {
    if(!reference_p() || !other->reference_p() || type_id() != other->type_id() ||
        class_object(state) != other->class_object(state)) {
      Exception* exc =
        Exception::make_type_error(state, type_id(), other);
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->raise_exception(exc);
      return NULL;
    }

    return copy_object(state, other);
  }

  Object* Object::copy_object(STATE, Object* other) {
    initialize_copy(state->memory(), other, age());

    write_barrier(state, klass());
    write_barrier(state, ivars());

    // Don't inherit the object_id from the original.
    reset_id(state);

    /* C extensions use Data objects for various purposes. The object
     * usually is made an instance of some extension class. So, we
     * have to check the object type to ensure we don't clobber the
     * data caried in the new instance.
     */
    if(type_id() != DataType) {
      copy_body(state->vm(), other);
    }

    // Ensure that the singleton class is not shared
    klass(state, other->class_object(state));

    // HACK: If other is mature, remember it.
    // We could inspect inspect the references we just copied to see
    // if there are any young ones if other is mature, then and only
    // then remember other. The up side to just remembering it like
    // this is that other is rarely mature, and the remember_set is
    // flushed on each collection anyway.
    if(mature_object_p()) {
      state->memory()->remember_object(this);
    }

    // Copy ivars.
    if(other->ivars()->reference_p()) {
      // NOTE Don't combine these 2 branches even though they both just call
      // ::copy. There is a special LookupTable::copy that can only be seen
      // when the receiver is of LookupTable* type. Without the explicit cast
      // and call, the wrong one will be called.
      if(LookupTable* lt = try_as<LookupTable>(other->ivars())) {
        ivars(state, lt->duplicate(state));

      } else {
        // Use as<> so that we throw a TypeError if there is something else
        // here.
        CompactLookupTable* clt = as<CompactLookupTable>(other->ivars());
        ivars(state, clt->duplicate(state));

      };
    }

    other->infect(state, this);
    return this;
  }

  Object* Object::equal(STATE, Object* other) {
    return this == other ? cTrue : cFalse;
  }

  Object* Object::freeze(STATE) {
    if(reference_p()) {
      set_frozen();
    } else if(!LANGUAGE_18_ENABLED(state)) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(!tbl) {
        tbl = LookupTable::create(state);
        G(external_ivars)->store(state, this, tbl);
      }
      tbl->set_frozen();
    }

    return this;
  }

  Object* Object::frozen_p(STATE) {
    if(reference_p()) {
      if(is_frozen_p()) return cTrue;
    } else if(!LANGUAGE_18_ENABLED(state)) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));
      if(tbl && tbl->is_frozen_p()) return cTrue;
    }
    return cFalse;
  }

  void Object::check_frozen(STATE) {
    if(frozen_p(state) == cTrue) {
      const char* reason = "can't modify frozen object";
      if(LANGUAGE_18_ENABLED(state)) {
        Exception::type_error(state, reason);
      } else {
        Exception::runtime_error(state, reason);
      }
    }
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

    return cNil;
  }

  Object* Object::table_ivar_defined(STATE, Symbol* sym) {
    bool found = false;
    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars())) {
      tbl->fetch(state, sym, &found);
    } else if(LookupTable* tbl = try_as<LookupTable>(ivars())) {
      tbl->fetch(state, sym, &found);
    }

    if(found) return cTrue;
    return cFalse;
  }

  Object* Object::get_ivar_prim(STATE, Symbol* sym) {
    if(sym->is_ivar_p(state)->false_p()) {
      return Primitives::failure();
    }

    return get_ivar(state, sym);
  }

  Object* Object::get_ivar(STATE, Symbol* sym) {
    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(tbl) return tbl->fetch(state, sym);
      return cNil;
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
        if(obj == cUndef) return cNil;
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

    TypeInfo* ti = state->memory()->find_type_info(this);
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
      return Primitives::failure();
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
        if(found) return cTrue;
      }

      return cFalse;
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

#ifdef IS_X8664
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
        return String::hash_str(state, (unsigned char *)(&(flt->val)), sizeof(double));
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
      if(object_id() == 0) {
        state->memory()->assign_object_id(state, this);
      }

      // Shift it up so we don't waste the numeric range in the actual
      // storage, but still present the id as always modulo 4, so it doesn't
      // collide with the immediates, since immediates never have a tag
      // ending in 00.
      return Integer::from(state, object_id() << TAG_REF_WIDTH);
    } else {
      /* All non-references have the pointer directly as the object id */
      return Integer::from(state, (uintptr_t)this);
    }
  }

  bool Object::has_id(STATE) {
    if(!reference_p()) return true;
    return object_id() > 0;
  }

  void Object::reset_id(STATE) {
    if(reference_p()) {
      set_object_id(state, state->memory(), 0);
    }
  }

  void Object::infect(STATE, Object* other) {
    if(is_tainted_p()) {
      other->taint(state);
    }

    if(!LANGUAGE_18_ENABLED(state)) {
      if(is_untrusted_p()) {
        other->untrust(state);
      }
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
    return kind_of_p(state, klass) ? cTrue : cFalse;
  }

  Object* Object::instance_of_prim(STATE, Module* klass) {
    return class_object(state) == klass ? cTrue : cFalse;
  }

  Class* Object::singleton_class(STATE) {
    if(reference_p()) {
      SingletonClass* sc = try_as<SingletonClass>(klass());

      /* This test is very important! SingletonClasses can get their klass()
       * hooked up to the SingletonClass of a parent class, so that the MOP
       * works properly. BUT we should not return that parent singleton
       * class, we need to only return a SingletonClass that is for this!
       */
      if(!sc || sc->attached_instance() != this) {
        sc = SingletonClass::attach(state, this);
      }

      /* We might have to fixup the chain further here. If we have inherited
       * from another class with a singleton class, this might be incorrect.
       * We have to correct this until we either find the correctly attached
       * class or when we have hit the cycle of the class being the singleton
       * class itself.
       */
      if(SingletonClass* sc_klass = try_as<SingletonClass>(sc->klass())) {
        if(sc != sc_klass->attached_instance() && sc_klass != sc_klass->klass()) {
          SingletonClass::attach(state, sc);
        }
      }

      infect(state, sc);
      sc->set_frozen(is_frozen_p());

      return sc;
    }

    return class_object(state);
  }

  Object* Object::send(STATE, CallFrame* caller, Symbol* name, Array* ary,
      Object* block, bool allow_private) {
    LookupData lookup(this, this->lookup_begin(state), allow_private ? G(sym_private) : G(sym_protected));
    Dispatch dis(name);

    Arguments args(name, ary);
    args.set_block(block);
    args.set_recv(this);

    return dis.send(state, caller, lookup, args);
  }

  Object* Object::send(STATE, CallFrame* caller, Symbol* name, bool allow_private) {
    LookupData lookup(this, this->lookup_begin(state), allow_private ? G(sym_private) : G(sym_protected));
    Dispatch dis(name);

    Arguments args(name);
    args.set_block(cNil);
    args.set_recv(this);

    return dis.send(state, caller, lookup, args);
  }

  Object* Object::send_prim(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                            Arguments& args, Symbol* min_visibility) {
    if(args.total() < 1) return Primitives::failure();

    // Don't shift the argument because we might fail and we need Arguments
    // to be pristine in the fallback code.
    Object* meth = args.get_argument(0);
    Symbol* sym = try_as<Symbol>(meth);

    // All coercion must be done here. Coercing Ruby-side and then
    // re-calling #send/#__send__ would produce incorrect results when
    // sending messages that are sensitive to the call stack like
    // send("__callee__") and the regex globals following send("gsub").
    //
    // MRI checks for Fixnum explicitly and raises ArgumentError
    // instead of TypeError. Seems silly, so we don't bother.
    if(!sym) {
      if(String* str = try_as<String>(meth)) {
        sym = str->to_sym(state);
      } else {
        TypeError::raise(Symbol::type, meth);
      }
    }

    // Discard the 1st argument.
    args.shift(state);
    args.set_name(sym);

    Dispatch dis(sym);
    LookupData lookup(this, this->lookup_begin(state), min_visibility);

    return dis.send(state, call_frame, lookup, args);
  }

  Object* Object::private_send_prim(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args) {
    return send_prim(state, call_frame, exec, mod, args, G(sym_private));
  }

  Object* Object::public_send_prim(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args) {
    return send_prim(state, call_frame, exec, mod, args, G(sym_public));
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
      if(tbl->store(state, sym, val) == cTrue) {
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
      return Primitives::failure();
    }

    check_frozen(state);
    return set_ivar(state, sym, val);
  }

  Object* Object::set_ivar(STATE, Symbol* sym, Object* val) {
    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

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
    TypeInfo* ti = state->memory()->find_type_info(this);
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
    TypeInfo* ti = state->memory()->find_type_info(this);
    if(ti) {
      TypeInfo::Slots::iterator it = ti->slots.find(sym->index());
      // Can't remove a slot, so just bail.
      if(it != ti->slots.end()) return cNil;
    }

    Object* val = del_table_ivar(state, sym, &removed);
    if(removed) return val;
    return Primitives::failure();
  }

  Object* Object::del_table_ivar(STATE, Symbol* sym, bool* removed) {
    /* No ivars, we're done! */
    if(ivars()->nil_p()) return cNil;

    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars())) {
      return tbl->remove(state, sym, removed);
    } else if(LookupTable* tbl = try_as<LookupTable>(ivars())) {
      return tbl->remove(state, sym, removed);
    }
    return cNil;
  }

  String* Object::to_s(STATE, bool address) {
    std::ostringstream name;

    if(!reference_p()) {
      if(nil_p()) return String::create(state, "nil");
      if(true_p()) return String::create(state, "true");
      if(false_p()) return String::create(state, "false");

      if(Fixnum* fix = try_as<Fixnum>(this)) {
        name << fix->to_native();
        return String::create(state, name.str().c_str(), name.str().size());
      } else if(Symbol* sym = try_as<Symbol>(this)) {
        name << ":\"" << sym->debug_str(state) << "\"";
        return String::create(state, name.str().c_str(), name.str().size());
      }
    }

    if(String* str = try_as<String>(this)) {
      return str;
    } else {
      name << "#<";
      if(Module* mod = try_as<Module>(this)) {
        if(mod->module_name()->nil_p()) {
          name << "Class";
        } else {
          name << mod->debug_str(state);
        }
        name << "(";
        if(SingletonClass* sc = try_as<SingletonClass>(mod)) {
          name << sc->true_superclass(state)->debug_str(state);
        } else {
          name << class_object(state)->debug_str(state);
        }
        name << ")";
      } else {
        if(this->class_object(state)->module_name()->nil_p()) {
          name << "Object";
        } else {
          name << class_object(state)->debug_str(state);
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

    return String::create(state, name.str().c_str(), name.str().size());
  }

  Object* Object::show(STATE) {
    return show(state, 0);
  }

  Object* Object::show(STATE, int level) {
    if(reference_p() && !state->memory()->valid_object_p(this)) rubinius::warn("bad object in show");
    type_info(state)->show(state, this, level);
    return cNil;
  }

  Object* Object::show_simple(STATE) {
    return show_simple(state, 0);
  }

  Object* Object::show_simple(STATE, int level) {
    type_info(state)->show_simple(state, this, level);
    return cNil;
  }

  Object* Object::taint(STATE) {
    if(!is_tainted_p()) {
      check_frozen(state);
      if (reference_p()) set_tainted();
    }
    return this;
  }

  Object* Object::tainted_p(STATE) {
    if(is_tainted_p()) return cTrue;
    return cFalse;
  }

  Object* Object::trust(STATE) {
    if(is_untrusted_p()) {
      check_frozen(state);
      set_untrusted(0);
    }
    return this;
  }

  Object* Object::untrust(STATE) {
    if(!is_untrusted_p()) {
      check_frozen(state);
      if(reference_p()) set_untrusted();
    }
    return this;
  }

  Object* Object::untrusted_p(STATE) {
    if(is_untrusted_p()) return cTrue;
    return cFalse;
  }

  TypeInfo* Object::type_info(STATE) const {
    return state->memory()->type_info[get_type()];
  }

  Object* Object::untaint(STATE) {
    if(is_tainted_p()) {
      check_frozen(state);
      if(reference_p()) set_tainted(0);
    }
    return this;
  }

  Object* Object::respond_to(STATE, Symbol* name, Object* priv) {
    LookupData lookup(this, lookup_begin(state), CBOOL(priv) ? G(sym_private) : G(sym_protected));

    Dispatch dis(name);

    if(!GlobalCache::resolve(state, name, dis, lookup)) {
      return cFalse;
    }

    return cTrue;
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

    LookupData lookup(this, lookup_begin(state), G(sym_protected));
    Dispatch dis(name);

    if(!GlobalCache::resolve(state, name, dis, lookup)) {
      return cFalse;
    }

    return cTrue;
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
      state->memory()->remember_object(this);
    }
  }

  void Object::inline_write_barrier_passed(VM* vm, void* obj) {
    if(!remembered_p()) {
      vm->om->remember_object(this);
    }
  }

  void Object::write_barrier(gc::WriteBarrier* wb, void* obj) {
    wb->write_barrier(this, reinterpret_cast<Object*>(obj));
  }

  void Object::setup_allocation_site(STATE, CallFrame* call_frame) {
    this->set_ivar(state, G(sym_allocation_site),
                   Location::create(state, call_frame));
  }

}

extern "C" long __id__(rubinius::Object* obj) {
  rubinius::State state(rubinius::VM::current());
  long id = obj->id(&state)->to_native();
  printf("Object: %p, id: %ld\n", obj, id);
  return id;
}

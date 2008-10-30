#include <iostream>

#include <cstdarg>

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
#include "builtin/task.hpp"
#include "builtin/float.hpp"
#include "objectmemory.hpp"
#include "message.hpp"

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

    return state->globals.special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
  }

  void Object::cleanup(STATE) {
    type_info(state)->cleanup(this);
  }

  Object* Object::clone(STATE) {
    Object* other = dup(state);

    other->copy_internal_state_from(state, this);

    return other;
  }


  void Object::copy_flags(STATE, Object* source) {
    this->obj_type        = source->obj_type;
    this->StoresBytes     = source->StoresBytes;
    this->RequiresCleanup = source->RequiresCleanup;
    this->IsBlockContext  = source->IsBlockContext;
    this->IsMeta          = source->IsMeta;
  }


  void Object::copy_internal_state_from(STATE, Object* original) {
    if(MetaClass* mc = try_as<MetaClass>(original->klass())) {
      LookupTable* source_methods = mc->method_table()->dup(state);
      LookupTable* source_constants = mc->constants()->dup(state);

      this->metaclass(state)->method_table(state, source_methods);
      this->metaclass(state)->constants(state, source_constants);

      // This allows us to preserve included modules
      this->metaclass(state)->superclass(state, mc->superclass());
    }
  }

  Object* Object::dup(STATE) {
    Object* other = state->om->allocate_object(this->num_fields());

    other->initialize_copy(this, age);
    other->copy_body(this);

    // Set the dup's class this's class
    other->klass(state, class_object(state));

    // HACK: If other is mature, remember it.
    // We could inspect inspect the references we just copied to see
    // if there are any young ones if other is mature, then and only
    // then remember other. The up side to just remembering it like
    // this is that other is rarely mature, and the remember_set is
    // flushed on each collection anyway.
    if(other->zone == MatureObjectZone) {
      state->om->remember_object(other);
    }

    // Copy ivars.
    if(ivars_->reference_p()) {
      // NOTE Don't combine these 2 branches even though they both just call
      // ::dup. There is a special LookupTable::dup that can only be seen
      // when the receiver is of LookupTable* type. Without the explicit cast
      // and call, the wrong one will be called.
      if(LookupTable* lt = try_as<LookupTable>(ivars_)) {
        other->ivars_ = lt->dup(state);
      } else {
        // Use as<> so that we throw a TypeError if there is something else
        // here.
        CompactLookupTable* clt = as<CompactLookupTable>(ivars_);
        other->ivars_ = clt->dup(state);
      };
    }

    return other;
  }

  Object* Object::equal(STATE, Object* other) {
    return this == other ? Qtrue : Qfalse;
  }

  Object* Object::freeze() {
    if(reference_p()) {
      this->IsFrozen = TRUE;
    }
    return this;
  }

  Object* Object::frozen_p() {
    if(reference_p() && this->IsFrozen) {
      return Qtrue;
    } else {
      return Qfalse;
    }
  }

  Object* Object::get_field(STATE, size_t index) {
    return type_info(state)->get_field(state, this, index);
  }

  Object* Object::get_ivar(STATE, Symbol* sym) {
    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(tbl) return tbl->fetch(state, sym);
      return Qnil;
    }

    // We might be trying to access a slot, so try that first.

    TypeInfo* ti = state->om->find_type_info(this);
    if(ti) {
      TypeInfo::Slots::iterator it = ti->slots.find(sym->index());
      if(it != ti->slots.end()) {
        return ti->get_field(state, this, it->second);
      }
    }

    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars_)) {
      return tbl->fetch(state, sym);
    } else if(LookupTable* tbl = try_as<LookupTable>(ivars_)) {
      return tbl->fetch(state, sym);
    }

    return Qnil;
  }

  /*
   * Returns a LookupTable or a CompactLookupTable.  Below a certain number of
   * instance variables a CompactTable is used to save memory.  See
   * Object::get_ivar for how to fetch an item out of get_ivars depending upon
   * storage type.
   */
  Object* Object::get_ivars(STATE) {
    if(!reference_p()) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(tbl) return tbl;
      return Qnil;
    }

    return ivars_;
  }

  object_type Object::get_type() const {
    if(reference_p()) return obj_type;
    if(fixnum_p()) return FixnumType;
    if(symbol_p()) return SymbolType;
    if(nil_p()) return NilType;
    if(true_p()) return TrueType;
    if(false_p()) return FalseType;
    return ObjectType;
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
      if(String* string = try_as<String>(this)) {
        hsh = string->hash_string(state);
      } else if(Bignum* bignum = try_as<Bignum>(this)) {
        hsh = bignum->hash_bignum(state);
      } else if(Float* flt = try_as<Float>(this)) {
        hsh = String::hash_str((unsigned char *)(&(flt->val)), sizeof(double));
      } else {
        hsh = id(state)->to_native();
      }
    }

    return hsh;
  }

  Integer* Object::hash_prim(STATE) {
    return Integer::from(state, hash(state));
  }

  Integer* Object::id(STATE) {
    if(reference_p()) {
      Object* id;

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

  void Object::infect(Object* other) {
    if(this->tainted_p() == Qtrue) {
      other->taint();
    }
  }

  /* Initialize the object as storing bytes, by setting the flag then clearing the
   * body of the object, by setting the entire body as bytes to 0 */
  void Object::init_bytes() {
    this->StoresBytes = 1;
    clear_body_to_null();
  }

  bool Object::kind_of_p(STATE, Object* module) {
    Module* found = NULL;

    if(!reference_p()) {
      found = state->globals.special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
    }
    else {
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

  Class* Object::metaclass(STATE) {
    if(reference_p()) {
      if(kind_of<MetaClass>(klass_)) {
        return as<MetaClass>(klass_);
      }
      return MetaClass::attach(state, this);
    }

    return class_object(state);
  }

  bool Object::send(STATE, Symbol* name, size_t count_args, ...) {
    va_list va;
    Array* args = Array::create(state, count_args);

    // Use the va_* macros to iterate over the variable number of
    // arguments passed in.
    va_start(va, count_args);
    for(size_t i = 0; i < count_args; i++) {
      args->set(state, i, va_arg(va, Object*));
    }
    va_end(va);

    return send_on_task(state, G(current_task), name, args);
  }

  bool Object::send_on_task(STATE, Task* task, Symbol* name, Array* args) {
    Message msg(state);
    msg.name = name;
    msg.recv = this;
    msg.lookup_from = this->lookup_begin(state);
    msg.stack = 0;
    msg.set_caller(task->active());

    msg.set_arguments(state, args);

    return task->send_message_slowly(msg);
  }

  ExecuteStatus Object::send_prim(STATE, Executable* exec, Task* task, Message& msg) {
    Object* meth = msg.shift_argument(state);
    Symbol* sym = try_as<Symbol>(meth);

    if(!sym) {
      sym = as<String>(meth)->to_sym(state);
    }

    msg.name = sym;
    msg.priv = true;
    return task->send_message_slowly(msg);
  }

  void Object::set_field(STATE, size_t index, Object* val) {
    type_info(state)->set_field(state, this, index, val);
  }

  void Object::set_forward(STATE, Object* fwd) {
    assert(zone == YoungObjectZone);
    Forwarded = 1;
    // DO NOT USE klass() because we need to get around the
    // write barrier!
    klass_ = (Class*)fwd;
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

    /* We might be trying to access a field, so check there first. */
    TypeInfo* ti = state->om->find_type_info(this);
    if(ti) {
      TypeInfo::Slots::iterator it = ti->slots.find(sym->index());
      if(it != ti->slots.end()) {
        ti->set_field(state, this, it->second, val);
        return val;
      }
    }

    /* Lazy creation of a lookuptable to store instance variables. */
    if(ivars_->nil_p()) {
      CompactLookupTable* tbl = CompactLookupTable::create(state);
      ivars(state, tbl);
      tbl->store(state, sym, val);
      return val;
    }

    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars_)) {
      if(tbl->store(state, sym, val) == Qtrue) {
        return val;
      }

      /* No more room in the CompactLookupTable. */
      ivars(state, tbl->to_lookuptable(state));
    }

    try_as<LookupTable>(ivars_)->store(state, sym, val);
    return val;
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

  Object* Object::taint() {
    if(reference_p()) {
      this->IsTainted = TRUE;
    }
    return this;
  }

  Object* Object::tainted_p() {
    if(reference_p() && this->IsTainted) {
      return Qtrue;
    } else {
      return Qfalse;
    }
  }

  TypeInfo* Object::type_info(STATE) const {
    return state->om->type_info[get_type()];
  }

  Object* Object::untaint() {
    if(reference_p()) {
      this->IsTainted = FALSE;
    }
    return this;
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
  void Object::write_barrier(STATE, void* obj) {
    state->om->write_barrier(this, reinterpret_cast<Object*>(obj));
  }

}

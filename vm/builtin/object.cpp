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
#include "builtin/taskprobe.hpp"
#include "builtin/float.hpp"
#include "objectmemory.hpp"
#include "global_cache.hpp"
#include "config.hpp"
#include "compiled_file.hpp"
#include "message.hpp"

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <cstring>
#include <sstream>

namespace rubinius {

  Class* Object::class_object(STATE) {
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

  // HACK: remove this when performance is better and compiled_file.rb
  // unmarshal_data method works.
  OBJECT Object::compiledfile_load(STATE, String* path, OBJECT version) {
    if(!state->probe->nil_p()) {
      state->probe->load_runtime(state, std::string(path->c_str()));
    }

    std::ifstream stream(path->c_str());
    if(!stream) {
      std::ostringstream msg;
      msg << "unable to open file to run: " << path->c_str();
      Exception::io_error(state, msg.str().c_str());
    }

    CompiledFile* cf = CompiledFile::load(stream);
    if(cf->magic != "!RBIX") {
      std::ostringstream msg;
      msg << "Invalid file: " << path->c_str();
      Exception::io_error(state, msg.str().c_str());
    }

    return cf->body(state);
  }

  void Object::copy_flags(STATE, OBJECT source) {
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

    /* TODO: dup instance vars. */
    return other;
  }

  OBJECT Object::equal(STATE, OBJECT other) {
    return this == other ? Qtrue : Qfalse;
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

  OBJECT Object::get_field(STATE, size_t index) {
    return type_info(state)->get_field(state, this, index);
  }

  OBJECT Object::get_ivar(STATE, SYMBOL sym) {
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

  OBJECT Object::get_ivars(STATE) {
    return ivars_;
  }

  object_type Object::get_type() {
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

  INTEGER Object::hash_prim(STATE) {
    return Integer::from(state, hash(state));
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

  /* Initialize the object as storing bytes, by setting the flag then clearing the
   * body of the object, by setting the entire body as bytes to 0 */
  void Object::init_bytes() {
    this->StoresBytes = 1;
    clear_body_to_null();
  }

  bool Object::kind_of_p(STATE, OBJECT module) {
    Module* found = class_object(state);
    if(found == cls) return true;

    for(;;) {
      found = try_as<Module>(found->superclass());
      if(!found) return false;
      if(found == cls) return true;

      if(IncludedModule* im = try_as<IncludedModule>(found)) {
        if(im->module() == cls) return true;
      }
    }

    return false;
  }

  OBJECT Object::kind_of_prim(STATE, Module* klass) {
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

  bool Object::send(STATE, SYMBOL name, size_t count_args, ...) {
    va_list va;
    Array* args = Array::create(state, count_args);

    // Use the va_* macros to iterate over the variable number of
    // arguments passed in.
    va_start(va, count_args);
    for(size_t i = 0; i < count_args; i++) {
      args->set(state, i, va_arg(va, OBJECT));
    }
    va_end(va);

    return send_on_task(state, G(current_task), name, args);
  }

  bool Object::send_on_task(STATE, Task* task, SYMBOL name, Array* args) {
    Message msg(state);
    msg.name = name;
    msg.recv = this;
    msg.lookup_from = this->lookup_begin(state);
    msg.stack = 0;

    msg.set_arguments(state, args);

    return task->send_message_slowly(msg);
  }

  ExecuteStatus Object::send_prim(STATE, Executable* exec, Task* task, Message& msg) {
    SYMBOL meth = as<Symbol>(msg.shift_argument(state));
    msg.name = meth;
    msg.priv = true;
    return task->send_message_slowly(msg);
  }

  void Object::set_field(STATE, size_t index, OBJECT val) {
    type_info(state)->set_field(state, this, index, val);
  }

  void Object::set_forward(STATE, OBJECT fwd) {
    assert(zone == YoungObjectZone);
    Forwarded = 1;
    // DO NOT USE klass() because we need to get around the
    // write barrier!
    klass_ = (Class*)fwd;
  }

  OBJECT Object::set_ivar(STATE, SYMBOL sym, OBJECT val) {
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

  OBJECT Object::show(STATE) {
    return this->show(state, 0);
  }

  OBJECT Object::show(STATE, int level) {
    type_info(state)->show(state, this, level);
    return Qnil;
  }

  OBJECT Object::show_simple(STATE) {
    return this->show_simple(state, 0);
  }

  OBJECT Object::show_simple(STATE, int level) {
    type_info(state)->show_simple(state, this, level);
    return Qnil;
  }

  OBJECT Object::taint() {
    if(reference_p()) {
      this->IsTainted = TRUE;
    }
    return this;
  }

  OBJECT Object::tainted_p() {
    if(reference_p() && this->IsTainted) {
      return Qtrue;
    } else {
      return Qfalse;
    }
  }

  TypeInfo* Object::type_info(STATE) {
    return state->om->type_info[get_type()];
  }

  OBJECT Object::untaint() {
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
   *  If we have OBJECT obj here, then we either have to cast to call
   *  write_barrier (which means we lose the ability to have type specific
   *  write_barrier versions, which we do), or we have to include
   *  every header up front. We opt for the former.
   */
  void Object::write_barrier(STATE, void* obj) {
    state->om->write_barrier(this, reinterpret_cast<Object*>(obj));
  }

}

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

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <cstring>

namespace rubinius {

  bool Object::fixnum_p() {
    return FIXNUM_P(this);
  }

  bool Object::symbol_p() {
    return SYMBOL_P(this);
  }

  /* Initialize the object as storing bytes, by setting the flag then clearing the
   * body of the object, by setting the entire body as bytes to 0 */
  void Object::init_bytes() {
    this->StoresBytes = 1;
    clear_body_to_null();
  }

  void Object::set_forward(STATE, OBJECT fwd) {
    assert(zone == YoungObjectZone);
    Forwarded = 1;
    klass(state, (Class*)fwd);
  }

  void Object::write_barrier(STATE, OBJECT obj) {
    state->om->write_barrier(this, obj);
  }

  // Safely return the object type, even if the receiver is an immediate
  object_type Object::get_type() {
    if(reference_p()) return obj_type;
    if(fixnum_p()) return FixnumType;
    if(symbol_p()) return SymbolType;
    if(nil_p()) return NilType;
    if(true_p()) return TrueType;
    if(false_p()) return FalseType;
    return ObjectType;
  }

  TypeInfo* Object::type_info(STATE) {
    return state->om->type_info[get_type()];
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

  Class* Object::class_object(STATE) {
    if(reference_p()) {
      Class* cls = klass_;
      while(!cls->nil_p() && !instance_of<Class>(cls)) {
        cls = as<Class>(cls->superclass());
      }

      if(cls->nil_p()) {
        Assertion::raise("Object::class_object() failed to find a class");
      }
      return cls;
    }

    return state->globals.special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
  }

  Class* Object::lookup_begin(STATE) {
    if(reference_p()) return klass_;
    return class_object(state);
  }

  OBJECT Object::dup(STATE) {
    OBJECT dup;

    // We will use lookup_begin here instead of class_object in order
    // to preserve any IncludedModule instances; they will be shared
    // between all duplicates made from this object
    // TODO - Verify this statement
    dup = state->om->new_object(lookup_begin(state), num_fields());
    gc_zone zone = dup->zone;
    dup->all_flags = all_flags;
    dup->zone = zone;

    dup->copy_body(this);

    // HACK: If dup is mature, remember it.
    // We could inspect inspect the references we just copied to see
    // if there are any young ones if dup is mature, then and only
    // then remember dup. The up side to just remembering it like
    // this is that dup is rarely mature, and the remember_set is
    // flushed on each collection anyway.
    if(dup->zone == MatureObjectZone) {
      state->om->remember_object(dup);
    }

    // TODO - Duplicate ('make independent') ivars here

    return dup;
  }

  OBJECT Object::clone(STATE) {
    LookupTable* source_methods = this->metaclass(state)->method_table()->dup(state);
    LookupTable* source_constants = this->metaclass(state)->constants()->dup(state);
    OBJECT new_object = this->dup(state);

    // TODO why can't we use new_object->metaclass(state) here? Why are
    // we creating a metaclass by hand?
    // Clone gets a new MetaClass
    new_object->klass(state, (MetaClass*)state->new_object(G(metaclass)));
    // Set the clone's method and constants tables to those
    // of the receiver's metaclass
    new_object->metaclass(state)->method_table(state, source_methods);
    new_object->klass()->constants(state, source_constants);

    return new_object;
  }

  bool Object::kind_of_p(STATE, OBJECT cls) {
    Class* found = class_object(state);
    if(found == cls) return true;

    while(!found->nil_p()) {
      found = (Class*)found->superclass();
      if(found == cls) return true;

      if(found->reference_p() && found->obj_type == IncludedModuleType) {
        if(((IncludedModule*)found)->module() == cls) return true;
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
        Float* flt = as<Float>(this);
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

  OBJECT Object::equal(STATE, OBJECT other) {
    return this == other ? Qtrue : Qfalse;
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

  OBJECT Object::get_ivars(STATE) {
    return ivars_;
  }

  OBJECT Object::get_ivar(STATE, OBJECT sym) {
    /* Implements the external ivars table for objects that don't
       have their own space for ivars. */
    if(!reference_p()) {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(tbl) return tbl->fetch(state, sym);
      return Qnil;
    }

    if(CompactLookupTable* tbl = try_as<CompactLookupTable>(ivars_)) {
      return tbl->fetch(state, sym);
    } else if(LookupTable* tbl = try_as<LookupTable>(ivars_)) {
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

  bool Object::send_prim(STATE, Executable* exec, Task* task, Message& msg) {
    SYMBOL meth = as<Symbol>(msg.shift_argument(state));
    msg.name = meth;
    msg.priv = true;
    return task->send_message_slowly(msg);
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

    msg.set_arguments(state, args);

    return task->send_message_slowly(msg);
  }

  void Object::cleanup(STATE) {
    type_info(state)->cleanup(this);
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
    return type_info(state)->get_field(state, this, index);
  }

  void Object::set_field(STATE, size_t index, OBJECT val) {
    type_info(state)->set_field(state, this, index, val);
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

  /* VM level primitives. This is a silly place, I know. */
  OBJECT Object::vm_get_config_item(STATE, String* var) {
    ConfigParser::Entry* ent = state->user_config->find(var->c_str());
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

  OBJECT Object::vm_exit(STATE, FIXNUM code) {
    ::exit(code->to_native());
  }

  OBJECT Object::vm_show_backtrace(STATE, MethodContext* ctx) {
    G(current_task)->print_backtrace(ctx);
    return Qnil;
  }

  OBJECT Object::vm_start_profiler(STATE) {
    G(current_task)->enable_profiler();
    return Qtrue;
  }

  OBJECT Object::vm_stop_profiler(STATE, String* path) {
    G(current_task)->disable_profiler(path->c_str());
    return path;
  }

  Object* Object::yield_gdb(STATE, Object* obj) {
    obj->show(state);
    Assertion::raise("yield_gdb called and not caught");
    return obj;
  }

  // HACK: remove this when performance is better and compiled_file.rb
  // unmarshal_data method works.
  OBJECT Object::compiledfile_load(STATE, String* path, OBJECT version) {
    if(!state->probe->nil_p()) {
      state->probe->load_runtime(state, std::string(path->c_str()));
    }

    std::ifstream stream(path->c_str());
    if(!stream) throw std::runtime_error("Unable to open file to run");

    CompiledFile* cf = CompiledFile::load(stream);
    if(cf->magic != "!RBIX") throw std::runtime_error("Invalid file");

    return cf->body(state);
  }
}

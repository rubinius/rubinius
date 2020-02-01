#include "arguments.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "dispatch.hpp"
#include "logger.hpp"
#include "lookup_data.hpp"
#include "memory.hpp"
#include "object_utils.hpp"
#include "on_stack.hpp"
#include "primitives.hpp"
#include "thread_phase.hpp"

#include "class/array.hpp"
#include "class/basic_object.hpp"
#include "class/bignum.hpp"
#include "class/call_site.hpp"
#include "class/class.hpp"
#include "class/compact_lookup_table.hpp"
#include "class/constant_table.hpp"
#include "class/encoding.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/float.hpp"
#include "class/location.hpp"
#include "class/lookup_table.hpp"
#include "class/method_table.hpp"
#include "class/object.hpp"
#include "class/packed_object.hpp"
#include "class/symbol.hpp"
#include "class/string.hpp"
#include "class/tuple.hpp"

#include "memory/collector.hpp"
#include "memory/visitor.hpp"

#include <sstream>

namespace rubinius {
  void Object::bootstrap(STATE) {
    GO(object).set(Class::bootstrap_class(state, G(basicobject), ObjectType));
  }

  Class* Object::class_object(STATE) const {
    if(reference_p()) {
      Module* mod = klass();
      while(!mod->nil_p() && !instance_of<Class>(mod)) {
        mod = as<Module>(mod->superclass());
      }

      if(mod->nil_p()) {
        Exception::raise_assertion_error(state, "Object::class_object() failed to find a class");
      }
      return as<Class>(mod);
    }

    return state->globals().special_classes[((uintptr_t)this) & SPECIAL_CLASS_MASK].get();
  }

  Object* Object::duplicate(STATE) {
    if(!reference_p()) return this;

    Object* other = state->memory()->new_object(
        state, class_object(state), this->size_in_bytes(state), type_id());

    return other->copy_object(state, this);
  }

  Object* Object::copy_singleton_class(STATE, Object* other) {
    if(SingletonClass* sc = try_as<SingletonClass>(other->klass())) {
      MethodTable* source_methods = 0;
      ConstantTable* source_constants = 0;
      Object* self = this;

      OnStack<4> os(state, self, sc, source_methods, source_constants);

      source_methods = sc->method_table()->duplicate(state);
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

  Object* Object::copy_object_prim(STATE, Object* other) {
    if(!reference_p() || !other->reference_p() || type_id() != other->type_id() ||
        class_object(state) != other->class_object(state)) {
      Exception* exc =
        Exception::make_type_error(state, type_id(), other);
      exc->locations(state, Location::from_call_stack(state));
      state->raise_exception(exc);
      return NULL;
    }

    return copy_object(state, other);
  }

  Object* Object::copy_object(STATE, Object* other) {
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

    // Copy ivars.
    if(other->ivars()->reference_p()) {
      // NOTE Don't combine these 2 branches even though they both just call
      // ::copy. There is a special LookupTable::copy that can only be seen
      // when the receiver is of LookupTable* type. Without the explicit cast
      // and call, the wrong one will be called.
      if(LookupTable* lt = try_as<LookupTable>(other->ivars())) {
        ivars(state, lt->duplicate(state));

      } else if(CompactLookupTable* clt = try_as<CompactLookupTable>(other->ivars())) {
        ivars(state, clt->duplicate(state));
      } else {
        logger::warn(
          "Object::copy_object: invalid ivars_ reference for %s",
          other->class_object(state)->to_string(state, true).c_str()
        );
      };
    }

    other->infect(state, this);

    return this;
  }

  Object* Object::equal(STATE, Object* other) {
    return RBOOL(equal_p(other));
  }

  Object* Object::freeze(STATE) {
    if(reference_p()) {
      set_frozen(state);
    } else {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));

      if(!tbl) {
        tbl = LookupTable::create(state);
        G(external_ivars)->store(state, this, tbl);
      }
      tbl->set_frozen(state);
    }

    return this;
  }

  Object* Object::object_frozen_p(STATE) {
    if(reference_p()) {
      return RBOOL(frozen_p());
    } else if(try_as<Symbol>(this) || try_as<Fixnum>(this)) {
      return cTrue;
    } else if(this->nil_p() || this->true_p() || this->false_p()) {
      return cTrue;
    } else {
      LookupTable* tbl = try_as<LookupTable>(G(external_ivars)->fetch(state, this));
      return RBOOL(tbl && tbl->frozen_p());
    }
    return cFalse;
  }

  void Object::check_frozen(STATE) {
    if(CBOOL(object_frozen_p(state)) && frozen_mod_disallowed()) {
      Exception::raise_frozen_error(state, this);
    }
  }

  bool Object::frozen_mod_disallowed() {
	  if(this->nil_p() || this->true_p() || this->false_p()) {
	    return false;
	  }
	  return true;
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

    return RBOOL(found);
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
        if(obj->undef_p()) return cNil;
        return obj;
      }
    default:
      break;
    }

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
        return RBOOL(found);
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

  Array* Object::instance_fields(STATE) {
    if(!reference_p()) return Array::create(state, 0);

    TypeInfo* ti = state->memory()->type_info[type_id()];

    Array* fields = Array::create(state, ti->slots.size());
    int index = 0;

    for(TypeInfo::Slots::iterator i = ti->slots.begin();
        i != ti->slots.end();
        ++i) {
      Symbol* sym = Symbol::from_index(i->first);
      if(sym->is_ivar_p(state)->true_p()) {
        fields->set(state, index++, sym);
      }
    }

    return fields;
  }

  Array* Object::ivar_names(STATE) {
    return ivar_names(state, Array::create(state, 3));
  }

  Array* Object::ivar_names(STATE, Array* ary) {
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
#ifdef IS_64BIT_ARCH
      uintptr_t key;

      if(nil_p()) {
        key = reinterpret_cast<uintptr_t>(cNil);
      } else {
        key = reinterpret_cast<uintptr_t>(this);
      }

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
      uint32_t a;

      if(nil_p()) {
        a = reinterpret_cast<uint32_t>(cNil);
      } else {
        a = reinterpret_cast<uint32_t>(this);
      }

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
        double value = flt->value();
        return String::hash_str(state, (unsigned char *)(&value), sizeof(double));
      } else {
        return object_id(state)->to_native();
      }
    }
  }

  Integer* Object::hash_prim(STATE) {
    return Integer::from(state, hash(state));
  }

  Integer* Object::object_id(STATE) {
    if(reference_p()) {
      uintptr_t id = ObjectHeader::object_id();
      if(id == 0) id = ObjectHeader::assign_object_id(state);

      // Shift it so it doesn't collide with object_id for immediates.
      return Integer::from(state, id << TAG_REF_WIDTH);
    } else if(nil_p()) {
      return Integer::from(state, reinterpret_cast<uintptr_t>(cNil));
    } else {
      // All non-references have the pointer directly as the object id
      return Integer::from(state, reinterpret_cast<uintptr_t>(this));
    }
  }

  bool Object::object_id_p(STATE) {
    return !reference_p() || ObjectHeader::object_id() > 0;
  }

  void Object::infect(STATE, Object* other) {
    if(reference_p() && tainted_p()) {
      other->taint(state);
    }
  }

  bool Object::kind_of_p(STATE, Object* module) {
    Module* found = lookup_begin(state);

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
    return RBOOL(kind_of_p(state, klass));
  }

  Object* Object::instance_of_prim(STATE, Module* klass) {
    return RBOOL(class_object(state) == klass);
  }

  Class* Object::singleton_class(STATE) {

    Class* sc = singleton_class_instance(state);

    /* We might have to fixup the chain further here. If we have inherited
     * from another class with a singleton class, this might be incorrect.
     * We have to correct this until we either find the correctly attached
     * class or when we have hit the cycle of the class being the singleton
     * class itself.
     */
    if(SingletonClass* sc_klass = try_as<SingletonClass>(sc->klass())) {
      if(sc != sc_klass->singleton()) {
        SingletonClass::attach(state, sc, sc->klass());
      }
    }

    return sc;
  }

  Class* Object::singleton_class_instance(STATE) {
    if(reference_p()) {
      SingletonClass* sc = try_as<SingletonClass>(klass());

      /* This test is very important! SingletonClasses can get their klass()
       * hooked up to the SingletonClass of a parent class, so that the MOP
       * works properly. BUT we should not return that parent singleton
       * class, we need to only return a SingletonClass that is for this!
       */
      if(!sc || sc->singleton() != this) {
        sc = SingletonClass::attach(state, this, klass());
      }

      infect(state, sc);
      if(frozen_p()) {
        sc->set_frozen(state);
      }

      return sc;
    }

    return class_object(state);
  }

  Object* Object::send(STATE, Symbol* name, Array* ary, Object* block, bool allow_private) {
    LookupData lookup(this, this->lookup_begin(state),
        allow_private ? G(sym_private) : G(sym_protected));
    Dispatch dispatch(name);

    Arguments args(name, this, block, ary);

    return dispatch.send(state, lookup, args);
  }

  Object* Object::send(STATE, Symbol* name, bool allow_private) {
    LookupData lookup(this, this->lookup_begin(state),
        allow_private ? G(sym_private) : G(sym_protected));
    Dispatch dispatch(name);

    Arguments args(name, this);

    return dispatch.send(state, lookup, args);
  }

  Object* Object::send(STATE, Executable* exec,
      Module* mod, Arguments& args, Symbol* min_visibility)
  {
    if(args.total() < 1) return Primitives::failure();

    /* Don't shift the argument because we might fail and we need Arguments to
     * be pristine in the fallback code.
     */
    Object* meth = args.get_argument(0);
    Symbol* sym = try_as<Symbol>(meth);

    /* All coercion must be done here. Coercing Ruby-side and then re-calling
     * #send/#__send__ would produce incorrect results when sending messages
     * that are sensitive to the call stack like send("__callee__") and the
     * regex globals following send("gsub").
     *
     * MRI checks for Fixnum explicitly and raises ArgumentError instead of
     * TypeError. Seems silly, so we don't bother.
     */
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

    /* We have to send it with self from the current frame as the source, not
     * this object to have correct visibility checks for protected.
     */
    Dispatch dispatch(sym);
    Object* scope = this;
    CallFrame* call_frame = state->call_frame();
    if(call_frame && !call_frame->native_method_p()) {
      scope = call_frame->self();
    }
    LookupData lookup(scope, lookup_begin(state), min_visibility);

    return dispatch.send(state, lookup, args);
  }

  Object* Object::private_send_prim(STATE,
      Executable* exec, Module* mod, Arguments& args)
  {
    return send(state, exec, mod, args, G(sym_private));
  }

  Object* Object::public_send_prim(STATE,
      Executable* exec, Module* mod, Arguments& args)
  {
    return send(state, exec, mod, args, G(sym_public));
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
      if(CBOOL(tbl->store(state, sym, val))) {
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

    switch(type_id()) {
    case Object::type:
      return set_table_ivar(state, sym, val);
    case PackedObject::type:
      {
        LookupTable* tbl = this->reference_class()->packed_ivar_info();
        bool found = false;

        Fixnum* which = try_as<Fixnum>(tbl->fetch(state, sym, &found));
        if(!found) {
          return set_table_ivar(state, sym, val);
        }

        Object** baa = reinterpret_cast<Object**>(pointer_to_body());
        baa[which->to_native()] = val;
        write_barrier(state, val);
        return val;
      }
    default:
      break;
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
    if(String* str = try_as<String>(this)) {
      return str;
    }
    std::string name = to_string(state, address);
    return String::create(state, name.c_str(), name.size());
  }

  std::string Object::to_string(STATE, bool address) {
    std::ostringstream name;

    if(!reference_p()) {
      if(nil_p()) return "nil";
      if(true_p()) return "true";
      if(false_p()) return "false";

      if(Fixnum* fix = try_as<Fixnum>(this)) {
        name << fix->to_native();
        return name.str();
      } else if(Symbol* sym = try_as<Symbol>(this)) {
        name << ":\"" << sym->debug_str(state) << "\"";
        return name.str();
      }
    }

    if(String* str = try_as<String>(this)) {
      return std::string(str->c_str(state), str->byte_size());
    } else if(Encoding* enc = try_as<Encoding>(this)) {
      name << "#<Encoding::";
      name << enc->name()->c_str(state) << ">";
      return name.str();
    } else {
      name << "#<";
      if(Module* mod = try_as<Module>(this)) {
        if(IncludedModule* im = try_as<IncludedModule>(mod)) {
          name << im->module()->module_name()->debug_str(state);
        } else if(mod->module_name()->nil_p()) {
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
      name << "0x" << std::hex << this->object_id(state)->to_native();
    }
    name << ">";

    return name.str();
  }

  Object* Object::show(STATE) {
    return show(state, 0);
  }

  Object* Object::show(STATE, int level) {
    /* TODO: GC
    if(reference_p() && !state->memory()->valid_object_p(this)) {
      std::ostringstream msg;
      msg << "bad object in show(): " << this;

      rubinius::warn(msg.str().c_str());
    }
    */

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
    if(reference_p()) {
      if(!try_as<Bignum>(this) && !try_as<Float>(this)) {
        if(!tainted_p()) {
          check_frozen(state);
          set_tainted(state);
        }
      }
    }
    return this;
  }

  Object* Object::object_tainted_p(STATE) {
    return RBOOL(reference_p() && tainted_p());
  }

  TypeInfo* Object::type_info(STATE) const {
    return state->memory()->type_info[get_type()];
  }

  Object* Object::untaint(STATE) {
    if(reference_p() && tainted_p()) {
      check_frozen(state);
      if(reference_p()) unset_tainted(state);
    }
    return this;
  }

  Object* Object::respond_to_prim(STATE, Symbol* name, Object* priv)
  {
    Object* self = this;
    OnStack<1> os(state, self);

    return respond_to(state, name, priv);
  }

  Object* Object::respond_to(STATE, Symbol* name, Object* priv) {
    LookupData lookup(cUndef, lookup_begin(state),
        CBOOL(priv) ? G(sym_private) : G(sym_public));

    Dispatch dispatch(name);

    if(dispatch.resolve(state, name, lookup)) {
      return cTrue;
    } else {
      LookupData lookup(this, lookup_begin(state), G(sym_private));
      Symbol* missing = G(sym_respond_to_missing);
      Dispatch dispatch(missing);

      if(dispatch.resolve(state, missing, lookup)) {
        Object* buf[2];
        buf[0] = name;
        buf[1] = priv;

        Arguments args(missing, this, 2, buf);

        if(Object* responds = dispatch.send(state, lookup, args)) {
          return RBOOL(CBOOL(responds));
        }

        return NULL;
      } else {
        return cFalse;
      }
    }
  }

  Object* Object::become(STATE, Object* other) {
    memory::Collector::Inhibit inhibitor(state);
    StopPhase locked(state);

    memory::MemoryVisitor visitor(state);

    visitor.visit_heap(state, [&](STATE, Object** obj) mutable {
        if(*obj == this) {
          *obj = other;
        } else if(*obj == other) {
          *obj = this;
        }
      });

    return other;
  }

  Object* Object::nil_code_id(STATE) {
    if(!nil_p()) return cNil;

    return Fixnum::from(NIL_TAG_ID(this));
  }

  Object* Object::nil_ip(STATE) {
    if(!nil_p()) return cNil;

    return Fixnum::from(NIL_TAG_IP(this));
  }

  void Object::setup_allocation_site(STATE) {
    this->set_ivar(state, G(sym_allocation_site),
                   Location::create(state, state->call_frame()));
  }
}

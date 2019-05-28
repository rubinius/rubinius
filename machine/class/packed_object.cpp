#include "class/array.hpp"
#include "class/class.hpp"
#include "class/fixnum.hpp"
#include "class/lookup_table.hpp"
#include "class/object.hpp"
#include "class/packed_object.hpp"
#include "class/symbol.hpp"
#include "memory.hpp"
#include "object_utils.hpp"

namespace rubinius {

  Object* PackedObject::packed_ivar_defined(STATE, Symbol* sym) {
    LookupTable* tbl = this->reference_class()->packed_ivar_info();
    bool found = false;

    Fixnum* which = try_as<Fixnum>(tbl->fetch(state, sym, &found));
    if(!found) {
      return table_ivar_defined(state, sym);
    }

    Object* obj = body_as_array()[which->to_native()];
    if(obj->undef_p()) return cFalse;
    return cTrue;
  }

  Object* PackedObject::packed_ivar_delete(STATE, Symbol* sym, bool* removed) {
    LookupTable* tbl = this->reference_class()->packed_ivar_info();
    bool found = false;

    Fixnum* which = try_as<Fixnum>(tbl->fetch(state, sym, &found));
    if(!found) {
      return del_table_ivar(state, sym, removed);
    }

    if(removed) *removed = true;

    Object* val = body_as_array()[which->to_native()];
    body_as_array()[which->to_native()] = cUndef;

    return val;
  }

  void PackedObject::add_packed_ivars(STATE, Array* ary) {
    LookupTable* tbl = this->reference_class()->packed_ivar_info();

    LookupTable::iterator i(tbl);

    while(i.advance()) {
      Object* key = i.key();
      if(Fixnum* which = try_as<Fixnum>(tbl->fetch(state, key))) {
        if(!body_as_array()[which->to_native()]->undef_p()) {
          ary->append(state, key);
        }
      }
    }
  }

  size_t PackedObject::Info::object_size(const ObjectHeader* obj) {
    return const_cast<ObjectHeader*>(obj)->reference_class()->packed_size();
  }

  namespace {
    inline size_t to_fields(size_t total) {
      return (total - sizeof(ObjectHeader)) / sizeof(Object*);
    }
  }

  void PackedObject::Info::mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    PackedObject* po = static_cast<PackedObject*>(obj);

    size_t fields = to_fields(object_size(obj));
    Object** body = po->body_as_array();

    for(size_t i = 0; i < fields; i++) {
      f(state, &body[i]);
    }
  }

  void PackedObject::Info::show(STATE, Object* self, int level) {
    class_info(state, self, true);
    return;
  }

  void PackedObject::Info::show_simple(STATE, Object* self, int level) {
    class_info(state, self, true);
    return;
  }
}

// This is in a separate file, even though it would normally be in system.cpp,
// because it uses a whole bunch of local classes and it's cleaner to have
// all that be in it's own file.

#include "vm/vm.hpp"

#include "gc/gc.hpp"
#include "gc/walker.hpp"
#include "objectmemory.hpp"

#include "builtin/object.hpp"
#include "builtin/array.hpp"
#include "builtin/symbol.hpp"
#include "builtin/module.hpp"
#include "builtin/class.hpp"
#include "builtin/tuple.hpp"
#include "builtin/variable_scope.hpp"

#include "builtin/system.hpp"

#include "object_utils.hpp"

namespace rubinius {
  class QueryCondition {
  public:
    virtual ~QueryCondition() { }
    virtual bool perform(STATE, Object* obj) = 0;
  };

  class AndCondition : public QueryCondition {
    QueryCondition* left_;
    QueryCondition* right_;

  public:
    AndCondition(QueryCondition* left, QueryCondition* right)
      : left_(left)
      , right_(right)
    {}

    virtual ~AndCondition() {
      delete left_;
      delete right_;
    }

    virtual bool perform(STATE, Object* obj) {
      return left_->perform(state, obj) && right_->perform(state, obj);
    }
  };

  class OrCondition : public QueryCondition {
    QueryCondition* left_;
    QueryCondition* right_;

  public:
    OrCondition(QueryCondition* left, QueryCondition* right)
      : left_(left)
      , right_(right)
    {}

    virtual ~OrCondition() {
      delete left_;
      delete right_;
    }

    virtual bool perform(STATE, Object* obj) {
      return left_->perform(state, obj) || right_->perform(state, obj);
    }
  };

  class KindofCondition : public QueryCondition {
    Module* mod_;

  public:
    KindofCondition(Module* mod)
      : mod_(mod)
    {}

    virtual bool perform(STATE, Object* obj) {
      // A really crappy restriction, but MRI does this and people
      // depend on it.
      if(kind_of<SingletonClass>(obj)) return false;
      return obj->kind_of_p(state, mod_);
    }
  };

  class ObjectIdCondition : public QueryCondition {
    Fixnum* id_;

  public:
    ObjectIdCondition(Fixnum* id)
      : id_(id)
    {}

    Fixnum* id() {
      return id_;
    }

    Object* immediate() {
      native_int id = id_->to_native();

      // immediates have an odd object id, references even
      if((id & 1) == 1) {
        Object* obj = reinterpret_cast<Object*>(id >> 1);

        // Be sure to not leak a bad reference leak out here.
        if(obj->reference_p()) return Qnil;
        return obj;
      }

      return 0;
    }

    virtual bool perform(STATE, Object* obj) {
      return obj->has_id(state) && obj->id(state) == id_;
    }
  };

  class HasIvarCondition : public QueryCondition {
    Symbol* ivar_;

  public:
    HasIvarCondition(Symbol* ivar)
      : ivar_(ivar)
    {}

    virtual bool perform(STATE, Object* obj) {
      return !obj->get_ivar(state, ivar_)->nil_p();
    }
  };

  class HasMethodCondition : public QueryCondition {
    Symbol* name_;

  public:
    HasMethodCondition(Symbol* name)
      : name_(name)
    {}

    virtual bool perform(STATE, Object* obj) {
      return obj->respond_to(state, name_, Qtrue)->true_p();
    }
  };

  class ReferencesCondition : public QueryCondition {
    Object* target_;
    Array* tmp_;

  public:
    ReferencesCondition(Object* obj)
      : target_(obj)
      , tmp_(0)
    {}

    virtual bool perform(STATE, Object* obj) {
      // Check class and ivars.
      if(obj->reference_class() == target_) return true;
      if(obj->ivars() == target_) return true;

      // Check slots.
      TypeInfo* ti = state->om->type_info[obj->type_id()];
      for(TypeInfo::Slots::iterator i = ti->slots.begin();
          i != ti->slots.end();
          i++) {
        Symbol* sym = Symbol::from_index(i->first);
        if(obj->get_ivar(state, sym) == target_) return true;
      }

      // Check normal (which include packed) ivars.
      if(!tmp_) tmp_ = Array::create(state, 20);

      // Reset the array. We use the same array each time to keep from
      // allocating an array and tuple per object.
      tmp_->total(state, Fixnum::from(0));

      obj->ivar_names(state, tmp_);

      for(size_t i = 0; i < tmp_->size(); i++) {
        if(Symbol* sym = try_as<Symbol>(tmp_->get(state, i))) {
          if(obj->get_ivar(state, sym) == target_) return true;
        }
      }

      // Check a tuples body.
      if(Tuple* tup = try_as<Tuple>(obj)) {
        for(native_int i = 0; i < tup->num_fields(); i++) {
          if(tup->at(i) == target_) return true;
        }
      }

      // Check a VariableScope's locals
      if(VariableScope* vs = try_as<VariableScope>(obj)) {
        for(int i = 0; i < vs->number_of_locals(); i++) {
          if(vs->get_local(state, i) == target_) return true;
        }
      }

      return false;
    }
  };

  static QueryCondition* create_condition(STATE, Array* ary) {
    if(ary->size() < 2) return 0;

    Symbol* what = try_as<Symbol>(ary->get(state, 0));
    Object* arg = ary->get(state, 1);

    if(what == state->symbol("kind_of")) {
      if(Module* mod = try_as<Module>(arg)) {
        return new KindofCondition(mod);
      }
    } else if(what == state->symbol("object_id")) {
      if(Fixnum* id = try_as<Fixnum>(arg)) {
        return new ObjectIdCondition(id);
      }
    } else if(what == state->symbol("ivar")) {
      if(Symbol* ivar = try_as<Symbol>(arg)) {
        return new HasIvarCondition(ivar);
      }
    } else if(what == state->symbol("method")) {
      if(Symbol* method = try_as<Symbol>(arg)) {
        return new HasMethodCondition(method);
      }
    } else if(what == state->symbol("references")) {
      return new ReferencesCondition(arg);
    } else if(what == state->symbol("and") ||
              what == state->symbol("or")) {
      if(ary->size() != 3) return 0;

      QueryCondition* left = 0;
      QueryCondition* right = 0;

      if(Array* sub = try_as<Array>(arg)) {
        left = create_condition(state, sub);
      }

      if(!left) return 0;

      if(Array* sub = try_as<Array>(ary->get(state, 2))) {
        right = create_condition(state, sub);
      }

      if(!right) {
        delete left;
        return 0;
      }

      if(what == state->symbol("and")) {
        return new AndCondition(left, right);
      } else {
        return new OrCondition(left, right);
      }
    }

    return 0;
  }

  Object* System::vm_find_object(STATE, Array* arg, Object* callable,
                                 CallFrame* calling_environment)
  {
    ObjectMemory::GCInhibit inhibitor(state->om);

    // Support an aux mode, where callable is an array and we just append
    // objects to it rather than #call it.
    Array* ary = try_as<Array>(callable);

    Array* args = Array::create(state, 1);

    int total = 0;

    QueryCondition* condition = create_condition(state, arg);
    if(!condition) return Fixnum::from(0);

    Object* ret = Qnil;

    // Special case for looking for an object_id of an immediate
    if(ObjectIdCondition* oic = dynamic_cast<ObjectIdCondition*>(condition)) {
      if(Object* obj = oic->immediate()) {
        if(ary) {
          ary->append(state, obj);
        } else {
          args->set(state, 0, obj);
          ret = callable->send(state, calling_environment, G(sym_call),
                               args, Qnil, false);
        }

        delete condition;
        if(!ret) return 0;
        return Fixnum::from(1);
      }
    }

    state->set_call_frame(calling_environment);
    ObjectWalker walker(state->om);
    GCData gc_data(state);

    // Seed it with the root objects.
    walker.seed(gc_data);

    Object* obj = walker.next();

    while(obj) {
      if(condition->perform(state, obj)) {
        total++;

        if(ary) {
          ary->append(state, obj);
        } else {
          args->set(state, 0, obj);
          ret = callable->send(state, calling_environment, G(sym_call),
                               args, Qnil, false);
          if(!ret) break;
        }
      }

      obj = walker.next();
    }

    delete condition;

    if(!ret) return 0;

    return Integer::from(state, total);
  }
}

#ifndef RBX_VARIABLE_SCOPE_HPP
#define RBX_VARIABLE_SCOPE_HPP

#include "call_frame.hpp"
#include "object_utils.hpp"
#include "spinlock.hpp"

#include "class/compiled_code.hpp"
#include "class/fiber.hpp"
#include "class/lookup_table.hpp"
#include "class/module.hpp"
#include "class/object.hpp"
#include "class/variable_scope.hpp"

namespace rubinius {

  class CompiledCode;
  class Module;
  class Tuple;

  /**
   *  Variable information.
   */
  class VariableScope : public Object {
  public:
    const static object_type type = VariableScopeType;

    attr_accessor(block, Object);
    attr_accessor(name, Symbol);
    attr_accessor(method, CompiledCode);
    attr_accessor(module, Module);
    attr_accessor(parent, VariableScope);
    attr_accessor(heap_locals, Tuple);
    attr_accessor(dynamic_locals, LookupTable);
    attr_accessor(last_match, Object);
    attr_accessor(fiber, Fiber);
    attr_accessor(self, Object);

    void set_local_internal(STATE, int pos, Object* val);
    Object* get_local_internal(STATE, int pos);
    void flush_to_heap_internal(STATE);

    attr_field(locals, Object**);
    attr_field(number_of_locals, int);
    attr_field(isolated, int);
    attr_field(flags, int);

    locks::spinlock_mutex _lock_;

    static void bootstrap(STATE);
    static void bootstrap_methods(STATE);
    static void initialize(STATE, VariableScope* obj) {
      obj->block(nil<Object>());
      obj->name(nil<Symbol>());
      obj->method(nil<CompiledCode>());
      obj->module(nil<Module>());
      obj->parent(nil<VariableScope>());
      obj->heap_locals(nil<Tuple>());
      obj->dynamic_locals(nil<LookupTable>());
      obj->last_match(nil<Object>());
      obj->fiber(nil<Fiber>());
      obj->self(nil<Object>());

      obj->locals(0);
      obj->number_of_locals(0);
      obj->isolated(1);
      obj->flags(0);

      new(&obj->_lock_) locks::spinlock_mutex;
    }

    bool isolated_p() const {
      return isolated() == 1;
    }

    bool block_as_method_p() const {
      return flags() & CallFrame::cBlockAsMethod;
    }

    bool top_level_visibility_p() const {
      return flags() & CallFrame::cTopLevelVisibility;
    }

    bool script_p() const {
      return flags() & CallFrame::cScript;
    }

    bool locked_p() const {
      return flags() & CallFrame::cScopeLocked;
    }

    void set_local(int pos, Object* val);
    void set_local(STATE, int pos, Object* val);

    Object* get_local(int pos);
    Object* get_local(STATE, int pos);

    void flush_to_heap(STATE);

    VariableScope* promote(STATE);

    // Rubinius.primitive+ :variable_scope_of_sender
    static VariableScope* of_sender(STATE);

    // Rubinius.primitive+ :variable_scope_current
    static VariableScope* current(STATE);

    // Rubinius.primitive :variable_scope_allocate
    static VariableScope* allocate(STATE);

    // Rubinius.primitive :variable_scope_synthesize
    static VariableScope* synthesize(STATE, CompiledCode* method, Module* module, Object* parent, Object* self, Object* block, Tuple* locals);

    // Rubinius.primitive :variable_scope_locals
    Tuple* local_variables(STATE);

    // Rubinius.primitive :variable_scope_set_local
    Object* set_local_prim(STATE, Fixnum* number, Object* object);

    // Rubinius.primitive+ :variable_scope_method_visibility
    Object* method_visibility(STATE);

    // Rubinius.primitive+ :variable_scope_top_level_visibility
    Object* top_level_visibility(STATE);

    // Rubinius.primitive+ :variable_scope_script
    Object* script(STATE);

    // Rubinius.primitive+ :variable_scope_locked
    Object* locked(STATE);

    // Rubinius.primitive+ :variable_scope_set_locked
    Object* set_locked(STATE);

  public: // Rubinius Type stuff
    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
      virtual void auto_mark(STATE, Object* obj, std::function<void (STATE, Object**)> f);
      virtual void set_field(STATE, Object*, size_t, Object*);
      virtual Object* get_field(STATE, Object*, size_t);
      virtual void populate_slot_locations();
      virtual void before_visit(STATE, Object* o, std::function<void (STATE, Object**)> f);
      virtual void after_visit(STATE, Object* o, std::function<void (STATE, Object**)> f) {}
      virtual void visit(STATE, Object* o, std::function<void (STATE, Object**)> f);
    };
  };
}

#endif

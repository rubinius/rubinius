#ifndef RBX_VARIABLE_SCOPE_HPP
#define RBX_VARIABLE_SCOPE_HPP

#include "call_frame.hpp"
#include "builtin/object.hpp"
#include "util/thread.hpp"

namespace rubinius {

  class CompiledCode;
  class Module;
  class Fiber;
  class Tuple;

  /**
   *  Variable information.
   */
  class VariableScope : public Object {
  public:
    const static object_type type = VariableScopeType;

  private:    /* Instance variables */
    /** Block given to method */
    Object*         block_;   // slot
    /** Method this scope is for. */
    CompiledCode* method_;  // slot
    Module*         module_;  // slot
    VariableScope*  parent_;  // slot
    Tuple*          heap_locals_; // slot
    LookupTable*    dynamic_locals_; // slot
    Object*         last_match_; // slot

    // The Fiber that the scope was created on
    Fiber*          fiber_; // slot

    void set_local_internal(STATE, int pos, Object* val);
    Object* get_local_internal(STATE, int pos);
    void flush_to_heap_internal(STATE);

  public:
    Object* self_;    // slot

    Object** locals_;
    int number_of_locals_;
    int isolated_;
    int flags_;
    utilities::thread::SpinLock lock_;

  public: /* Accessors */
    attr_accessor(block, Object);
    attr_accessor(method, CompiledCode);
    attr_accessor(module, Module);
    attr_accessor(parent, VariableScope);
    attr_accessor(self, Object);
    attr_accessor(heap_locals, Tuple);
    attr_accessor(last_match, Object);
    attr_accessor(dynamic_locals, LookupTable);
    attr_accessor(fiber, Fiber);

    static void init(STATE);
    static void bootstrap_methods(STATE);

    bool isolated() const {
      return isolated_ == 1;
    }

    bool block_as_method_p() const {
      return flags_ & CallFrame::cBlockAsMethod;
    }

    bool top_level_visibility_p() const {
      return flags_ & CallFrame::cTopLevelVisibility;
    }

    bool script_p() const {
      return flags_ & CallFrame::cScript;
    }

    bool locked_p() const {
      return flags_ & CallFrame::cScopeLocked;
    }

    void set_local(int pos, Object* val);
    void set_local(STATE, int pos, Object* val);

    Object* get_local(int pos);
    Object* get_local(STATE, int pos);

    int number_of_locals() const {
      return number_of_locals_;
    }

    void flush_to_heap(STATE);

    VariableScope* promote(STATE);

    // Rubinius.primitive+ :variable_scope_of_sender
    static VariableScope* of_sender(STATE, CallFrame* calling_environment);

    // Rubinius.primitive+ :variable_scope_current
    static VariableScope* current(STATE, CallFrame* calling_environment);

    // Rubinius.primitive :variable_scope_synthesize
    static VariableScope* synthesize(STATE, CompiledCode* method, Module* module, Object* parent, Object* self, Object* block, Tuple* locals);

    // Rubinius.primitive :variable_scope_locals
    Tuple* locals(STATE);

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
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void set_field(STATE, Object*, size_t, Object*);
      virtual Object* get_field(STATE, Object*, size_t);
      virtual void auto_mark(Object*, ObjectMark&);
      virtual void populate_slot_locations();
    };
  };
}

#endif

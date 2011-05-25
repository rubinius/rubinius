#ifndef RBX_VARIABLE_SCOPE_HPP
#define RBX_VARIABLE_SCOPE_HPP

#include "vm/object_utils.hpp"

#include "builtin/object.hpp"
#include "builtin/tuple.hpp"

namespace rubinius {

  class CompiledMethod;
  class Module;
  struct CallFrame;

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
    CompiledMethod* method_;  // slot
    Module*         module_;  // slot
    VariableScope*  parent_;  // slot
    Tuple*          heap_locals_; // slot
    Object*         last_match_; // slot

  public:
    Object* self_;    // slot

    int number_of_locals_;
    bool isolated_;
    Object** locals_;
    int block_as_method_;

  public: /* Accessors */
    attr_accessor(block, Object);
    attr_accessor(method, CompiledMethod);
    attr_accessor(module, Module);
    attr_accessor(parent, VariableScope);
    attr_accessor(self, Object);
    attr_accessor(heap_locals, Tuple);
    attr_accessor(last_match, Object);

    static void init(STATE);
    static void bootstrap_methods(STATE);

    void fixup() { }

    bool isolated() {
      return isolated_;
    }

    Object** stack_locals() {
      return locals_;
    }

    bool block_as_method_p() {
      return block_as_method_ == 1;
    }

    void set_block_as_method(bool val) {
      block_as_method_ = (val ? 1 : 0);
    }

    void update_parent(VariableScope* vs) {
      parent_ = vs;
    }

    void set_local(STATE, int pos, Object* val) {
      if(isolated_) {
        heap_locals_->put(state, pos, val);
      } else {
        locals_[pos] = val;
      }
    }

    void set_local(int pos, Object* val) {
      assert(isolated_ == false);
      locals_[pos] = val;
    }

    Object* get_local(STATE, int pos) {
      if(isolated_) {
        return heap_locals_->at(state, pos);
      }
      return locals_[pos];
    }

    Object* get_local(int pos) {
      if(isolated_) {
        return heap_locals_->at(pos);
      }
      return locals_[pos];
    }

    int number_of_locals() {
      return number_of_locals_;
    }

    VariableScope* promote(STATE);

    // Ruby.primitive :variable_scope_of_sender
    static VariableScope* of_sender(STATE, CallFrame* calling_environment);

    // Ruby.primitive :variable_scope_current
    static VariableScope* current(STATE, CallFrame* calling_environment);

    // Ruby.primitive :variable_scope_locals
    Tuple* locals(STATE);

    // Ruby.primitive :variable_scope_method_visibility
    Object* method_visibility(STATE);

  public: // Rubinius Type stuff
    class Info : public TypeInfo {
    public:
      Info(object_type type) : TypeInfo(type) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void visit(Object*, ObjectVisitor&);
      virtual void set_field(STATE, Object*, size_t, Object*);
      virtual Object* get_field(STATE, Object*, size_t);
      virtual void auto_mark(Object*, ObjectMark&);
      virtual void auto_visit(Object*, ObjectVisitor&);
      virtual void populate_slot_locations();
    };
  };
}

#endif

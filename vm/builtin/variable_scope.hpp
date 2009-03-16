#ifndef RBX_VARIABLE_SCOPE_HPP
#define RBX_VARIABLE_SCOPE_HPP

#include "builtin/object.hpp"

namespace rubinius {
  class Module;

  class VariableScope : public Object {
  public:
    const static object_type type = VariableScopeType;

  private:
    VariableScope* parent_; // slot
    Object* self_;   // slot
    Module* module_; // slot
    Object* block_;  // slot

    // Indicates if this scope is for a scope that has
    // already exitted
    Object* exitted_; // slot

    int number_of_locals_;
    Object* locals_[];

  public:
    attr_accessor(parent, VariableScope);
    attr_accessor(self, Object);
    attr_accessor(module, Module);
    attr_accessor(block, Object);
    attr_accessor(exitted, Object);

    static void init(STATE);

    /**
     *  Initialize and default scope.
     */
    void prepare(Object* self, Module* mod, Object* block, int num) {
      obj_type = InvalidType;
      parent_ = (VariableScope*)Qnil;
      self_ = self;
      module_ = mod;
      block_ = block;
      number_of_locals_ = num;
      exitted_ = Qnil;

      for(int i = 0; i < num; i++) {
        locals_[i] = Qnil;
      }
    }

    void exit() {
      exitted_ = Qtrue;
    }

    bool exitted_p() {
      return exitted_ == Qtrue;
    }

    void update(Object* self, Object* mod, Object* block) {
      self_ = self;
      module_ = (Module*)mod; // safe? unsafe?
      block_ = block;
    }

    void update_parent(VariableScope* vs) {
      parent_ = vs;
    }

    void setup_as_block(VariableScope* top, VariableScope* parent, int num, Object* self=0);

    bool stack_allocated_p() {
      return obj_type == 0;
    }

    void set_local(STATE, int pos, Object* val) {
      locals_[pos] = val;

      if(!stack_allocated_p()) {
        write_barrier(state, val);
      }
    }

    void set_local(int pos, Object* val) {
      locals_[pos] = val;
    }

    Object* get_local(STATE, int pos) {
      return locals_[pos];
    }

    Object* get_local(int pos) {
      return locals_[pos];
    }

    int number_of_locals() {
      return number_of_locals_;
    }

    VariableScope* promote(STATE);

    // Ruby.primitive? :variable_scope_of_sender
    static VariableScope* of_sender(STATE, Executable* exec, CallFrame* call_frame, Message& msg);

    // Ruby.primitive? :variable_scope_current
    static VariableScope* current(STATE, Executable* exec, CallFrame* call_frame, Message& msg);

  public: // Rubinius Type stuff
    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void visit(Object*, ObjectVisitor&);
      virtual void set_field(STATE, Object*, size_t, Object*);
      virtual Object* get_field(STATE, Object*, size_t);
      virtual void auto_mark(Object*, ObjectMark&);
      virtual void auto_visit(Object*, ObjectVisitor&);
    };
  };
}

#endif

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

    int number_of_locals_;
    Object* locals_[];

  public:
    attr_accessor(parent, VariableScope);
    attr_accessor(self, Object);
    attr_accessor(module, Module);
    attr_accessor(block, Object);

    static void init(STATE);

    void setup(Object* self, Module* mod, Object* block, int num) {
      obj_type = InvalidType;
      parent_ = (VariableScope*)Qnil;
      self_ = self;
      module_ = mod;
      block_ = block;
      number_of_locals_ = num;

      for(int i = 0; i < num; i++) {
        locals_[i] = Qnil;
      }
    }

    void setup_as_block(VariableScope* top, VariableScope* parent, int num);

    bool stack_allocated_p() {
      return obj_type == 0;
    }

    void set_local(STATE, int pos, Object* val) {
      locals_[pos] = val;
      write_barrier(state, val);
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
    static VariableScope* of_sender(STATE, Executable* exec, CallFrame* call_frame, Task* task, Message& msg);

  public: // Rubinius Type stuff
    class Info : public TypeInfo {
    public:
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void set_field(STATE, Object*, size_t, Object*);
      virtual Object* get_field(STATE, Object*, size_t);
      virtual void auto_mark(Object*, ObjectMark&);
    };
  };
}

#endif

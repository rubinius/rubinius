#ifndef RBX_VARIABLE_SCOPE_HPP
#define RBX_VARIABLE_SCOPE_HPP

#include "vm/object_utils.hpp"

#include "builtin/object.hpp"

namespace rubinius {

  class CompiledMethod;
  class Module;
  class CallFrame;

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

  public:
    Object* self_;    // slot

    int number_of_locals_;
    bool isolated_;
    Object** locals_;
    int block_as_method_;

    // MUST BE LAST
    Object* heap_locals_[];

  public: /* Accessors */
    attr_accessor(block, Object);
    attr_accessor(method, CompiledMethod);
    attr_accessor(module, Module);
    attr_accessor(parent, VariableScope);
    attr_accessor(self, Object);

    static void init(STATE);
    static void bootstrap_methods(STATE);

    void point_locals_to(Object** locals) {
      locals_ = locals;
    }

    void fixup() {
      if(isolated_) {
        locals_ = heap_locals_;
      }
    }

    bool block_as_method_p() {
      return block_as_method_ == 1;
    }

    void set_block_as_method(bool val) {
      block_as_method_ = (val ? 1 : 0);
    }

    /**
     *  Initialize scope for methods.
     */
    void prepare(Object* self, Module* mod, Object* block, CompiledMethod* method, int num) {
      init_header(UnspecifiedZone, InvalidType);
      klass_ = 0;
      ivars_ = 0;

      parent_ = (VariableScope*)Qnil;
      self_ = self;
      method_ = method;
      module_ = mod;
      block_ = block;
      number_of_locals_ = num;

      for(int i = 0; i < num; i++) {
        locals_[i] = Qnil;
      }
    }

    /**
     *  Initialize scope for blocks.
     */
    void setup_as_block(VariableScope* top, VariableScope* parent, CompiledMethod* method, int num, Object* self = 0);

    void update(Object* self, Object* cm, Object* mod, Object* block) {
      self_ = self;
      module_ = (Module*)mod; // safe? unsafe?
      block_ = block;

      method_ = reinterpret_cast<CompiledMethod*>(cm);
    }

    void update_parent(VariableScope* vs) {
      parent_ = vs;
    }

    void set_local(STATE, int pos, Object* val) {
      locals_[pos] = val;

      if(locals_ == heap_locals_) {
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
      Info(object_type type, bool cleanup = false) : TypeInfo(type, cleanup) { }
      virtual void mark(Object* t, ObjectMark& mark);
      virtual void visit(Object*, ObjectVisitor&);
      virtual void set_field(STATE, Object*, size_t, Object*);
      virtual Object* get_field(STATE, Object*, size_t);
      virtual void auto_mark(Object*, ObjectMark&);
      virtual void auto_visit(Object*, ObjectVisitor&);
      virtual size_t object_size(const ObjectHeader* object);
      virtual void populate_slot_locations();
    };
  };
}

#endif

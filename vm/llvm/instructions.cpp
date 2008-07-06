#include "builtin.hpp"
#include "jit_state.h"

using namespace rubinius;

#define OP(name, args...) void name(Task* task, struct jit_state* const js, ## args)
#define OP2(type, name, args...) type name(Task* task, struct jit_state* const js, ## args)
#define stack_push(val) *++js->stack = val
#define stack_pop() *js->stack--
#define stack_top() *js->stack
#define stack_back(count) *(js->stack - count)

#define SHOW(obj) (((NormalObject*)(obj))->show(state))

#define state task->state

#define I2N(num) APPLY_TAG(num, TAG_FIXNUM)
#define both_fixnum_p(_p1, _p2) ((intptr_t)_p1 & (intptr_t)_p2 & TAG_FIXNUM)

#define CACHE_JS() js->stack = task->stack->field + task->sp
#define FLUSH_JS() task->sp = js->stack - task->stack->field

extern "C" {

  OP(push_int, int val) {
    stack_push(Object::i2n(val));
  }

  OP(meta_push_0) {
    stack_push(I2N(0));
  }
  
  OP(meta_push_1) {
    stack_push(I2N(1));
  }

  OP(push_nil) {
    stack_push(Qnil);
  }

  OP(push_true) {
    stack_push(Qtrue);
  }

  OP(push_false) {
    stack_push(Qfalse);
  }

  OP(push_context) {
    MethodContext* ctx = task->active;
    ctx->reference(state);
    stack_push(ctx);
  }

  OP(push_literal, int index) {
    stack_push(task->literals->field[index]);
  }

  OP(set_literal, int index) {
    task->literals->field[index] = stack_top();
  }

  OP(push_self) {
    stack_push(task->self);
  }

  OP(push_local, int index) {
    stack_push(task->stack->field[index]);
  }

  OP(set_local, int index) {
    task->stack->field[index] = stack_top();
  }

  OP(push_block) {
    stack_push(task->active->block);
  }

  OP(push_ivar, int index) {
    OBJECT sym = task->literals->field[index];
    stack_push(task->self->get_ivar(state, sym));
  }

  OP(set_ivar, int index) {
    OBJECT sym = task->literals->field[index];
    task->self->set_ivar(state, sym, stack_top());
  }

  OP(push_my_field, int index) {
    stack_push(task->self->get_field(state, index));
  }

  OP(store_my_field, int index) {
    task->self->set_field(state, index, stack_top());
  }

  OP(swap_stack) {
    OBJECT t1 = stack_pop();
    OBJECT t2 = stack_pop();
    stack_push(t1);
    stack_push(t2);
  }

  OP(dup_top) {
    OBJECT t1 = stack_top();
    stack_push(t1);
  }

  OP(pop) {
    stack_pop();
  }

  OP(make_array, int index) {
    Array* ary = Array::create(state, index);
    int j = index - 1;
    for(; j >= 0; j--) {
      ary->set(state, j, stack_pop());
    }

    stack_push(ary);
  }

  OP(push_const, int index) {
    bool found;
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    OBJECT res = task->const_get(sym, &found);
    if(!found) {
      assert("implement const_missing");
    } else {
      stack_push(res);
    }
  }

  OP(find_const, int index) {
    bool found;
    Module* under = as<Module>(stack_pop());
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    OBJECT res = task->const_get(under, sym, &found);
    if(!found) {
      assert("implement const_missing");
    } else {
      stack_push(res);
    }
  }

  OP(set_const, int index) {
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    task->const_set(sym, stack_top());
  }

  OP(set_const_at, int index) {
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    OBJECT val = stack_pop();
    Module* under = as<Module>(stack_pop());

    task->const_set(under, sym, val);
    stack_push(val);
  }

  OP(push_cpath_top) {
    stack_push(G(object));
  }

  OP(open_class_under, int index) {
    bool created;
    OBJECT super = stack_pop();
    Module* under = as<Module>(stack_pop());
    SYMBOL sym = as<Symbol>(task->literals->field[index]);

    Class* cls = task->open_class(under, super, sym, &created);
    // TODO use created? it's only for running the opened_class hook, which
    // we're eliminating anyway.

    stack_push(cls);
  }

  OP(open_class, int index) {
    bool created;
    OBJECT super = stack_pop();
    SYMBOL sym = as<Symbol>(task->literals->field[index]);

    Class* cls = task->open_class(super, sym, &created);

    stack_push(cls);
  }

  OP(open_module_under, int index) {
    bool created;
    Module* mod = as<Module>(stack_pop());
    SYMBOL sym = as<Symbol>(task->literals->field[index]);

    stack_push(task->open_module(mod, sym));
  }

  OP(open_module, int index) {
    bool created;
    SYMBOL sym = as<Symbol>(task->literals->field[index]);

    stack_push(task->open_module(sym));
  }

  OP(open_metaclass) {
    OBJECT obj = stack_pop();
    stack_push(obj->metaclass(state));
  }

  OP(attach_method, int index) {
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    OBJECT obj = stack_pop();
    OBJECT obj2 = stack_pop();
    CompiledMethod* meth = as<CompiledMethod>(obj2);

    task->attach_method(obj, sym, meth);
    stack_push(meth);
  }

  OP(add_method, int index) {
    SYMBOL sym = as<Symbol>(task->literals->field[index]);
    Module* obj = as<Module>(stack_pop());
    CompiledMethod* meth = as<CompiledMethod>(stack_pop());

    task->add_method(obj, sym, meth);
    stack_push(meth);
  }

  OP2(bool, send_method, int index) {
    Message msg(state);

    msg.send_site = as<SendSite>(task->literals->field[index]);
    msg.recv = stack_top();
    msg.block = Qnil;
    msg.splat = Qnil;
    msg.args = 0;
    msg.stack = 1;

    msg.priv = task->call_flags & 1;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name;

    task->call_flags = 0;

    FLUSH_JS();
    if(task->send_message(msg)) {
      return true;
    } else {
      CACHE_JS();
      return false;
    }
  }

  OP2(bool, send_stack, int index, int count) {
    Message msg(state);

    msg.send_site = as<SendSite>(task->literals->field[index]);
    msg.recv = stack_back(count);
    msg.block = Qnil;
    msg.splat = Qnil;
    msg.args = count;
    msg.stack = count + 1;
    msg.use_from_task(task, count);

    msg.priv = task->call_flags & 1;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.name = msg.send_site->name;

    task->call_flags = 0;

    FLUSH_JS();
    if(task->send_message(msg)) {
      return true;
    } else {
      CACHE_JS();
      return false;
    }
  }

  bool send_slowly(Task* task, struct jit_state* const js, SYMBOL name) {
    Message msg(state);
    msg.recv = stack_back(1);
    msg.import_arguments(state, task, 1);
    msg.name = name;
    msg.lookup_from = msg.recv->lookup_begin(state);
    msg.block = Qnil;

    /* pull receiver off stack */
    stack_pop();

    FLUSH_JS();
    if(task->send_message_slowly(msg)) {
      return true;
    } else {
      CACHE_JS();
      return false;
    }
  }

  OP2(bool, meta_send_op_plus) {
    OBJECT left =  stack_back(1);
    OBJECT right = stack_back(0);

    if(both_fixnum_p(left, right)) {
      stack_pop();
      stack_pop();
      OBJECT res = ((FIXNUM)(left))->add(state, (FIXNUM)(right));
      stack_push(res);
      return false;
    }

    return send_slowly(task, js, G(sym_plus));
  }

  OP2(bool, meta_send_op_lt) {
    OBJECT left =  stack_back(1);
    OBJECT right = stack_back(0);

    if(both_fixnum_p(left, right)) {
      stack_pop();
      stack_pop();
      stack_push(as<Fixnum>(left)->lt(state, as<Fixnum>(right)));
      return false;
    }

    return send_slowly(task, js, G(sym_lt));
  }

  OP(halt) {
    throw Task::Halt("Task halted");
  }

  OP(ret) {
    task->simple_return(stack_top());
  }

  OP2(bool, goto_if_true) {
    return RTEST(stack_pop());
  }

  OP2(bool, goto_if_false) {
    return !RTEST(stack_pop());
  }

  OP2(bool, goto_if_defined) {
    return stack_pop() != Qundef;
  }

  OP(set_call_flags, int flags) {
    task->call_flags = flags;
  }

  OP(test) {
    OBJECT obj = stack_pop();
    if(RTEST(obj)) {
      printf("blah");
    } else {
      printf("crap");
    }
  }
}

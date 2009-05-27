#ifdef ENABLE_LLVM

#include "vm.hpp"
#include "objectmemory.hpp"
#include "llvm/jit.hpp"
#include "call_frame.hpp"
#include "builtin/object.hpp"
#include "builtin/symbol.hpp"
#include "builtin/system.hpp"
#include "builtin/class.hpp"
#include "builtin/string.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/proc.hpp"
#include "builtin/sendsite.hpp"
#include "builtin/autoload.hpp"
#include "builtin/global_cache_entry.hpp"

#include "helpers.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "lookup_data.hpp"

#define both_fixnum_p(_p1, _p2) ((uintptr_t)(_p1) & (uintptr_t)(_p2) & TAG_FIXNUM)

using namespace rubinius;

extern "C" {
  Object* rbx_write_barrier(STATE, Object* obj, Object* val) {
    if(obj->zone == UnspecifiedZone) return val;
    state->om->write_barrier(obj, val);
    return val;
  }

  Object* rbx_inline_cache_send(STATE, CallFrame* call_frame, Symbol* name,
                          SendSite::Internal* cache, int count, Object** args) {
    Object* recv = args[0];
    Class* const klass = recv->lookup_begin(state);

    Arguments out_args(recv, Qnil, count, args+1);

    if(unlikely(cache->recv_class != klass)) {
      if(SendSite::fill(state, klass, call_frame, cache, false)) {
        out_args.unshift(state, cache->name);
      }
    }

    return cache->method->execute(state, call_frame, *cache, out_args);
  }

  Object* rbx_inline_cache_send_private(STATE, CallFrame* call_frame, Symbol* name,
                          SendSite::Internal* cache, int count, Object** args) {
    Object* recv = args[0];
    Class* const klass = recv->lookup_begin(state);

    Arguments out_args(recv, Qnil, count, args+1);

    if(unlikely(cache->recv_class != klass)) {
      if(SendSite::fill(state, klass, call_frame, cache, true)) {
        out_args.unshift(state, cache->name);
      }
    }

    return cache->method->execute(state, call_frame, *cache, out_args);
  }

  Object* rbx_simple_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, count, args+1);
    Dispatch dis(name);

    return dis.send(state, call_frame, out_args);
  }

  Object* rbx_simple_send_private(STATE, CallFrame* call_frame, Symbol* name,
                                  int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, count, args+1);
    LookupData lookup(recv, recv->lookup_begin(state), true);
    Dispatch dis(name);

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_block_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, args[count+1], count, args+1);
    Dispatch dis(name);

    return dis.send(state, call_frame, out_args);
  }

  Object* rbx_block_send_private(STATE, CallFrame* call_frame, Symbol* name,
                                  int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, args[count+1], count, args+1);
    LookupData lookup(recv, recv->lookup_begin(state), true);
    Dispatch dis(name);

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_splat_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, args[count+2], count, args+1);
    Dispatch dis(name);

    if(Array* ary = try_as<Array>(args[count+1])) {
      out_args.append(state, ary);
    }

    return dis.send(state, call_frame, out_args);
  }

  Object* rbx_splat_send_private(STATE, CallFrame* call_frame, Symbol* name,
                                  int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(recv, args[count+2], count, args+1);
    LookupData lookup(recv, recv->lookup_begin(state), true);
    Dispatch dis(name);

    if(Array* ary = try_as<Array>(args[count+1])) {
      out_args.append(state, ary);
    }

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_super_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = call_frame->self();
    Arguments out_args(recv, args[count], count, args);
    LookupData lookup(recv, call_frame->module()->superclass(), true);
    Dispatch dis(name);

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_super_splat_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = call_frame->self();
    Arguments out_args(recv, args[count+1], count, args);
    LookupData lookup(recv, call_frame->module()->superclass(), true);
    Dispatch dis(name);

    out_args.append(state, as<Array>(args[count]));

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_arg_error(STATE, CallFrame* call_frame, Dispatch& msg, Arguments& args,
                        int required) {
    Exception* exc =
        Exception::make_argument_error(state, required, args.total(), msg.name);
    exc->locations(state, System::vm_backtrace(state, Fixnum::from(1), call_frame));
    state->thread_state()->raise_exception(exc);

    return NULL;
  }

  Object* rbx_string_dup(STATE, CallFrame* call_frame, Object* obj) {
    try {
      return as<String>(obj)->string_dup(state);
    } catch(TypeError& e) {
      Exception* exc =
        Exception::make_type_error(state, e.type, e.object, e.reason);
      exc->locations(state, System::vm_backtrace(state, 0, call_frame));

      state->thread_state()->raise_exception(exc);
      return NULL;
    }
  }

  Object* rbx_create_block(STATE, CallFrame* call_frame, int index) {
    Object* _lit = call_frame->cm->literals()->at(state, index);
    CompiledMethod* cm = as<CompiledMethod>(_lit);

    call_frame->promote_scope(state);

    // TODO: We don't need to be doing this everytime.
    cm->scope(state, call_frame->static_scope);

    VMMethod* vmm = call_frame->cm->backend_method_;
    return BlockEnvironment::under_call_frame(state, cm, vmm,
                                              call_frame, index);
  }

  void rbx_setup_scope(STATE, VariableScope* scope, CallFrame* call_frame,
                       Dispatch& msg, Arguments& args) {
    CompiledMethod* cm = as<CompiledMethod>(msg.method);
    VMMethod* vmm = cm->backend_method_;

    scope->prepare(args.recv(), msg.module, args.block(), cm, vmm->number_of_locals);
  }

  Object* rbx_construct_splat(STATE, CallFrame* call_frame, Arguments& args, size_t total) {
    if(args.total() > total) {
      size_t splat_size = args.total() - total;
      Array* ary = Array::create(state, splat_size);

      for(size_t i = 0, n = total; i < splat_size; i++, n++) {
        ary->set(state, i, args.get_argument(n));
      }
      return ary;
    } else {
      return Array::create(state, 0);
    }
  }

  Object* rbx_cast_array(STATE, CallFrame* call_frame, Object* top) {
    if(Tuple* tup = try_as<Tuple>(top)) {
      return Array::from_tuple(state, tup);
    } else if(kind_of<Array>(top)) {
      return top;
    }

    // coerce
    Object* recv = G(array);
    Arguments args(recv, 1, &top);
    Dispatch dis(G(sym_coerce_into_array));

    return dis.send(state, call_frame, args);
  }

  Object* rbx_add_scope(STATE, CallFrame* call_frame, Object* top) {
    Module* mod = as<Module>(top);
    StaticScope* scope = StaticScope::create(state);
    scope->module(state, mod);
    scope->parent(state, call_frame->static_scope);
    call_frame->cm->scope(state, scope);
    call_frame->static_scope = scope;

    return Qnil;
  }

  Object* rbx_cast_for_splat_block_arg(STATE, CallFrame* call_frame, Object* top) {
    if(top->nil_p()){
      return Array::create(state, 0);
    } else if(Tuple* tup = try_as<Tuple>(top)) {
      return Array::from_tuple(state, tup);
    } else if(kind_of<Array>(top)) {
      return top;
    }

    // Wrap
    Array* ary = Array::create(state, 1);
    ary->set(state, 0, top);
    return ary;
  }

  Object* rbx_cast_for_multi_block_arg(STATE, CallFrame* call_frame, Object* top) {
    if(Tuple* tup = try_as<Tuple>(top)) {
      /* If there is only one thing in the tuple... */
      if(tup->num_fields() == 1) {
        /* and that thing is an array... */
        if(Array* ary = try_as<Array>(tup->at(state, 0))) {
          /* make a tuple out of the array contents... */
          int j = ary->size();
          Tuple* out = Tuple::create(state, j);

          for(int k = 0; k < j; k++) {
            out->put(state, k, ary->get(state, k));
          }

          return out;
        }
      }
    }

    return top;
  }

  Object* rbx_cast_for_single_block_arg(STATE, CallFrame* call_frame, Object* top) {
    Tuple* tup = as<Tuple>(top);
    int k = tup->num_fields();
    if(k == 0) {
      return Qnil;
    } else if(k == 1) {
      return tup->at(state, 0);
    }

    return Array::from_tuple(state, tup);
  }

  Object* rbx_check_serial(STATE, CallFrame* call_frame, int index, int serial, Object* top) {
    SendSite* ss = as<SendSite>(call_frame->cm->literals()->at(state, index));

    if(ss->check_serial(state, call_frame, top, serial)) {
      return Qtrue;
    }

    return Qfalse;
  }

  Object* rbx_find_const(STATE, CallFrame* call_frame, int index, Object* top) {
    bool found;
    Module* under = as<Module>(top);
    Symbol* sym = as<Symbol>(call_frame->cm->literals()->at(state, index));
    Object* res = Helpers::const_get(state, under, sym, &found);

    if(!found) {
      res = Helpers::const_missing(state, under, sym, call_frame);
    } else if(Autoload* autoload = try_as<Autoload>(res)) {
      res = autoload->resolve(state, call_frame);
    }

    return res;
  }

  Object* rbx_instance_of(STATE, CallFrame* call_frame, Object* top, Object* b1) {
    Class* cls = as<Class>(b1);
    if(top->class_object(state) == cls) return Qtrue;
    return Qfalse;
  }

  Object* rbx_in_nil(STATE, CallFrame* call_frame, Object* top) {
    return top->nil_p() ? Qtrue : Qfalse;
  }

  Object* rbx_kind_of(STATE, CallFrame* call_frame, Object* top, Object* b1) {
    return top->kind_of_p(state, b1) ? Qtrue : Qfalse;
  }

  Object* rbx_make_array(STATE, CallFrame* call_frame, int count, Object** args) {
    Array* ary = Array::create(state, count);
    for(int i = 0; i < count; i++) {
      ary->set(state, i, args[i]);
    }

    return ary;
  }

  Object* rbx_meta_send_call(STATE, CallFrame* call_frame, int count, Object** args) {
    Object* t1 = args[0];

    Arguments out_args(Qnil, count, args+1);

    if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
      return env->call(state, call_frame, out_args);
    } else if(Proc* proc = try_as<Proc>(t1)) {
      return proc->call(state, call_frame, out_args);
    }

    return rbx_simple_send(state, call_frame, G(sym_call), count, args);
  }

  Object* rbx_yield_stack(STATE, CallFrame* call_frame, int count, Object** args) {
    Object* t1 = call_frame->top_scope->block();

    Arguments out_args(t1, count, args);

    if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
      return env->call(state, call_frame, out_args);
    } else if(Proc* proc = try_as<Proc>(t1)) {
      return proc->yield(state, call_frame, out_args);
    } else if(t1->nil_p()) {
      Exception* exc = Exception::make_exception(state, G(jump_error), "no block given");
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    Dispatch dis(G(sym_call));

    return dis.send(state, call_frame, out_args);
  }

  Object* rbx_yield_splat(STATE, CallFrame* call_frame, int count, Object** stk) {
    Object* ary = stk[count];
    Object* t1 = call_frame->top_scope->block();

    Arguments args(t1, count, stk);

    if(!ary->nil_p()) {
      args.append(state, as<Array>(ary));
    }

    if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
      return env->call(state, call_frame, args);
    } else if(Proc* proc = try_as<Proc>(t1)) {
      return proc->yield(state, call_frame, args);
    } else if(t1->nil_p()) {
      Exception* exc = Exception::make_exception(state, G(jump_error), "no block given");
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    Dispatch dis(G(sym_call));
    return dis.send(state, call_frame, args);
  }

  Object* rbx_meta_send_op_gt(STATE, CallFrame* call_frame, Object** stk) {
    Object* t1 = stk[0];
    Object* t2 = stk[1];
    if(both_fixnum_p(t1, t2)) {
      native_int j = as<Integer>(t1)->to_native();
      native_int k = as<Integer>(t2)->to_native();
      return (j > k) ? Qtrue : Qfalse;
    }

    return rbx_simple_send(state, call_frame, G(sym_gt), 1, stk);
  }

  Object* rbx_meta_send_op_minus(STATE, CallFrame* call_frame, Object** stk) {
    Object* left =  stk[0];
    Object* right = stk[1];

    if(both_fixnum_p(left, right)) {
      return reinterpret_cast<Fixnum*>(left)->sub(state,
          reinterpret_cast<Fixnum*>(right));

    }

    return rbx_simple_send(state, call_frame, G(sym_minus), 1, stk);
  }

  Object* rbx_meta_send_op_nequal(STATE, CallFrame* call_frame, Object** stk) {
    Object* t1 = stk[0];
    Object* t2 = stk[1];
    /* If both are not references, compare them directly. */
    if(!t1->reference_p() && !t2->reference_p()) {
      return (t1 == t2) ? Qfalse : Qtrue;
    }

    return rbx_simple_send(state, call_frame, G(sym_nequal), 1, stk);
  }

  Object* rbx_meta_send_op_tequal(STATE, CallFrame* call_frame, Object** stk) {
    Object* t1 = stk[0];
    Object* t2 = stk[1];
    /* If both are fixnums, or both are symbols, compare the ops directly. */
    if((t1->fixnum_p() && t2->fixnum_p()) || (t1->symbol_p() && t2->symbol_p())) {
      return (t1 == t2) ? Qfalse : Qtrue;
    }

    return rbx_simple_send(state, call_frame, G(sym_tequal), 1, stk);
  }

  Object* rbx_passed_arg(STATE, CallFrame* call_frame, int index) {
    return (index < call_frame->args) ? Qtrue : Qfalse;
  }

  // TODO remove this and use passed_arg
  Object* rbx_passed_blockarg(STATE, CallFrame* call_frame, int index) {
    return (index == call_frame->args) ? Qtrue : Qfalse;
  }

  Object* rbx_push_const(STATE, CallFrame* call_frame, Symbol* sym) {
    bool found;
    Object* res = Helpers::const_get(state, call_frame, sym, &found);

    if(!found) {
      Module* under;
      StaticScope* scope = call_frame->static_scope;
      if(scope->nil_p()) {
        under = G(object);
      } else {
        under = scope->module();
      }
      res = Helpers::const_missing(state, under, sym, call_frame);
    } else if(Autoload* autoload = try_as<Autoload>(res)) {
      res = autoload->resolve(state, call_frame);
    }

    return res;
  }

  Object* rbx_push_const_fast(STATE, CallFrame* call_frame, Symbol* sym,
                              int association_index) {
    bool found;
    Object* res = 0;

    Object* val = call_frame->cm->literals()->at(state, association_index);

    // See if the cache is present, if so, validate it and use the value
    GlobalCacheEntry* cache;
    if((cache = try_as<GlobalCacheEntry>(val)) != NULL) {
      if(cache->valid_p(state)) {
        res = cache->value();
      } else {
        res = Helpers::const_get(state, call_frame, sym, &found);
        if(found) cache->update(state, res);
      }
    } else {
      res = Helpers::const_get(state, call_frame, sym, &found);
      if(found) {
        cache = GlobalCacheEntry::create(state, res);
        call_frame->cm->literals()->put(state, association_index, cache);
      } else {
        Module* under;
        StaticScope* scope = call_frame->static_scope;
        if(scope->nil_p()) {
          under = G(object);
        } else {
          under = scope->module();
        }
        res = Helpers::const_missing(state, under, sym, call_frame);
      }
    }

    if(!res) return NULL;

    if(Autoload* autoload = try_as<Autoload>(res)) {
      res = autoload->resolve(state, call_frame);
      if(cache && res) {
        cache->update(state, res);
      }
    }

    return res;
  }

  Object* rbx_set_local_depth(STATE, CallFrame* call_frame, Object* top,
                              int depth, int index) {
    VariableScope* scope = call_frame->scope;

    for(int j = 0; j < depth; j++) {
      scope = scope->parent();
    }

    scope->set_local(state, index, top);
    return top;
  }

  Object* rbx_push_local_depth(STATE, CallFrame* call_frame,
                              int depth, int index) {
    VariableScope* scope = call_frame->scope;

    for(int j = 0; j < depth; j++) {
      scope = scope->parent();
    }

    return scope->get_local(index);
  }

  Object* rbx_check_interrupts(STATE, CallFrame* call_frame) {
    if(!state->check_stack(call_frame, &state)) return NULL;

    if(unlikely(state->interrupts.check)) {
      state->interrupts.check = false;
      state->collect_maybe(call_frame);
    }

    if(unlikely(state->interrupts.timer)) {
      {
        state->interrupts.timer = false;
        state->set_call_frame(call_frame);
        // unlock..
        GlobalLock::UnlockGuard lock(state->global_lock());
        // and relock automatically!
      }
    }
    if(!state->check_async(call_frame)) return NULL;
    return Qtrue;
  }

  bool rbx_return_to_here(STATE, CallFrame* call_frame) {
    ThreadState* th = state->thread_state();
    if(th->raise_reason() != cReturn) return false;
    if(th->destination_scope() == call_frame->scope) return true;
    return false;
  }

  bool rbx_break_to_here(STATE, CallFrame* call_frame) {
    ThreadState* th = state->thread_state();
    if(th->raise_reason() != cBreak) return false;
    if(th->destination_scope() == call_frame->scope) return true;
    return false;
  }

  Object* rbx_clear_raise_value(STATE) {
    Object* val = state->thread_state()->raise_value();
    state->thread_state()->clear_exception(true);
    return val;
  }

  bool rbx_raising_exception(STATE) {
    return state->thread_state()->raise_reason() == cException;
  }

  Object* rbx_current_exception(STATE) {
    return state->thread_state()->as_object(state);
  }

  Object* rbx_clear_exception(STATE) {
    // Don't allow this code to clear non-exception raises
    if(state->thread_state()->raise_reason() == cException) {
      state->thread_state()->clear_exception();
    }

    return Qnil;
  }

  Object* rbx_pop_exception(STATE, Object* top) {
    if(top->nil_p()) {
      state->thread_state()->clear_exception();
    } else {
      state->thread_state()->set_exception(state, top);
    }

    return Qnil;
  }

  Object* rbx_push_system_object(STATE, int which) {
    switch(which) {
    case 0:
      return G(object);
    default:
      return Qnil;
    }
  }

  Object* rbx_push_ivar(STATE, CallFrame* call_frame, Symbol* name) {
    return call_frame->self()->get_ivar(state, name);
  }

  Object* rbx_set_ivar(STATE, CallFrame* call_frame, Symbol* name, Object* val) {
    return call_frame->self()->set_ivar(state, name, val);
  }

  Object* rbx_push_my_field(STATE, CallFrame* call_frame, int which) {
    return call_frame->self()->get_field(state, which);
  }

  Object* rbx_set_my_field(STATE, CallFrame* call_frame, int which, Object* val) {
    call_frame->self()->set_field(state, which, val);
    return val;
  }

  Object* rbx_set_const(STATE, CallFrame* call_frame, Symbol* name, Object* val) {
    call_frame->static_scope->module()->set_const(state, name, val);
    return val;
  }

  Object* rbx_set_const_at(STATE, CallFrame* call_frame, Symbol* name, Object* mod, Object* val) {
    // TODO if the as<> fails, the process will abort().
    as<Module>(mod)->set_const(state, name, val);
    return val;
  }

  Object* rbx_set_literal(STATE, CallFrame* call_frame, int which, Object* val) {
    call_frame->cm->literals()->put(state, which, val);
    return Qnil;
  }

  Object* rbx_shift_array(STATE, CallFrame* call_frame, Object** loc) {
    Array* array = as<Array>(*loc);
    size_t size = (size_t)array->size();

    if(size == 0) return Qnil;

    size_t j = size - 1;
    Object* shifted_value = array->get(state, 0);
    Array* smaller_array = Array::create(state, j);

    for(size_t i = 0; i < j; i++) {
      smaller_array->set(state, i, array->get(state, i+1));
    }

    *loc = smaller_array;
    return shifted_value;
  }

  Object* rbx_string_append(STATE, CallFrame* call_frame, Object* left, Object* right) {
    return as<String>(left)->append(state, as<String>(right));
  }

  Object* rbx_raise_return(STATE, CallFrame* call_frame, Object* top) {
    if(!(call_frame->flags & CallFrame::cIsLambda) &&
       call_frame->scope->parent()->exitted_p()) {
      Exception* exc = Exception::make_exception(state, G(jump_error), "unexpected return");
      exc->locations(state, System::vm_backtrace(state, Fixnum::from(0), call_frame));
      state->thread_state()->raise_exception(exc);
    } else {
      state->thread_state()->raise_return(top, call_frame->top_scope);
    }

    return Qnil;
  }

  Object* rbx_ensure_return(STATE, CallFrame* call_frame, Object* top) {
    state->thread_state()->raise_return(top, call_frame->scope);
    return Qnil;
  }

  Object* rbx_raise_break(STATE, CallFrame* call_frame, Object* top) {
    state->thread_state()->raise_break(top, call_frame->scope->parent());
    return Qnil;
  }

}

#endif

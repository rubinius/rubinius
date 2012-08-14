#ifdef ENABLE_LLVM

#include "llvm/state.hpp"
#include "llvm/method_info.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"
#include "call_frame.hpp"
#include "on_stack.hpp"

#include "builtin/object.hpp"
#include "builtin/symbol.hpp"
#include "builtin/system.hpp"
#include "builtin/class.hpp"
#include "builtin/string.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/constantscope.hpp"
#include "builtin/proc.hpp"
#include "builtin/autoload.hpp"
#include "builtin/global_cache_entry.hpp"
#include "builtin/iseq.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/integer.hpp"
#include "builtin/float.hpp"
#include "builtin/location.hpp"
#include "builtin/cache.hpp"
#include "builtin/encoding.hpp"

#include "instruments/tooling.hpp"

#include "helpers.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "lookup_data.hpp"
#include "inline_cache.hpp"
#include "vmmethod.hpp"
#include "configuration.hpp"

#include <stdarg.h>

#define both_fixnum_p(_p1, _p2) ((uintptr_t)(_p1) & (uintptr_t)(_p2) & TAG_FIXNUM)

#define CPP_TRY try {

#define CPP_CATCH \
    } catch(TypeError& e) { \
      Exception* exc = \
        Exception::make_type_error(state, e.type, e.object, e.reason); \
      exc->locations(state, Location::from_call_stack(state, call_frame)); \
      state->raise_exception(exc); \
      return NULL; \
    } catch(const RubyException& exc) { \
      exc.exception->locations(state, \
            Location::from_call_stack(state, call_frame)); \
      state->raise_exception(exc.exception); \
      return NULL; \
    }

using namespace rubinius;

extern "C" {

  void rbx_jit_debug_spot(int spot) {
    std::cout << "JIT DEBUG: hit spot: " << spot << "\n";
  }

  Object* rbx_write_barrier(STATE, Object* obj, Object* val) {
    if(obj->zone() == UnspecifiedZone) return val;
    obj->write_barrier(state, val);
    return val;
  }

  Object* rbx_check_frozen(STATE, CallFrame* call_frame, Object* obj) {
    if(obj->reference_p() && obj->is_frozen_p()) {
      Exception::frozen_error(state, call_frame);
      return NULL;
    }

    return obj;
  }

  void rbx_begin_profiling(STATE, void* data, Executable* exec, Module* mod, Arguments& args,
                           CompiledCode* cm)
  {
    // Use placement new to stick the class into data, which is on the callers
    // stack.
    new(data) tooling::MethodEntry(state, exec, mod, args, cm);
  }

  void rbx_begin_profiling_block(STATE, void* data, BlockEnvironment* env,
                                 Module* mod, CompiledCode* cm)
  {
    // Use placement new to stick the class into data, which is on the callers
    // stack.
    new(data) tooling::BlockEntry(state, env, mod);
  }

  void rbx_end_profiling(tooling::MethodEntry* entry) {
    entry->~MethodEntry();
  }

  Object* rbx_simple_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(name, recv, count, args+1);
    Dispatch dis(name);

    return dis.send(state, call_frame, out_args);
  }

  Object* rbx_simple_send_private(STATE, CallFrame* call_frame, Symbol* name,
                                  int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(name, recv, count, args+1);
    LookupData lookup(recv, recv->lookup_begin(state), G(sym_private));
    Dispatch dis(name);

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_splat_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(name, recv, args[count+2], count, args+1);
    Dispatch dis(name);

    if(Array* ary = try_as<Array>(args[count+1])) {
      out_args.append(state, ary);
    }

    return dis.send(state, call_frame, out_args);
  }

  Object* rbx_splat_send_private(STATE, CallFrame* call_frame, Symbol* name,
                                  int count, Object** args) {
    Object* recv = args[0];
    Arguments out_args(name, recv, args[count+2], count, args+1);
    LookupData lookup(recv, recv->lookup_begin(state), G(sym_private));
    Dispatch dis(name);

    if(Array* ary = try_as<Array>(args[count+1])) {
      out_args.append(state, ary);
    }

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_super_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = call_frame->self();
    Arguments out_args(name, recv, args[count], count, args);
    LookupData lookup(recv, call_frame->module()->superclass(), G(sym_private));
    Dispatch dis(name);

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_super_splat_send(STATE, CallFrame* call_frame, Symbol* name,
                          int count, Object** args) {
    Object* recv = call_frame->self();
    Arguments out_args(name, recv, args[count+1], count, args);
    LookupData lookup(recv, call_frame->module()->superclass(), G(sym_private));
    Dispatch dis(name);

    if(Array* ary = try_as<Array>(args[count])) {
      out_args.append(state, ary);
    }

    return dis.send(state, call_frame, lookup, out_args);
  }

  Object* rbx_zsuper_send(STATE, CallFrame* call_frame, Symbol* name, Object* block) {
    Object* const recv = call_frame->self();

    VariableScope* scope = call_frame->method_scope(state);
    assert(scope);

    VMMethod* v = scope->method()->backend_method();
    Object* splat_obj = 0;
    Array* splat = 0;

    size_t arg_count = v->total_args;

    if(v->splat_position >= 0) {
      splat_obj = scope->get_local(state, v->splat_position);
      splat = try_as<Array>(splat_obj);
      if(splat) {
        arg_count += splat->size();
      } else {
        arg_count++;
      }
    }

    Tuple* tup = Tuple::create(state, arg_count);
    for(int i = 0; i < v->total_args; i++) {
      tup->put(state, i, scope->get_local(state, i));
    }

    if(splat) {
      for(size_t i = 0; i < splat->size(); i++) {
        tup->put(state, i + v->total_args, splat->get(state, i));
      }
    } else if(splat_obj) {
      tup->put(state, v->total_args, splat_obj);
    }

    Arguments out_args(name, recv, block, arg_count, 0);
    out_args.use_tuple(tup, arg_count);

    LookupData lookup(recv, call_frame->module()->superclass(), G(sym_private));
    Dispatch dis(name);

    return dis.send(state, call_frame, lookup, out_args, eSuper);
  }

  Object* rbx_arg_error(STATE, CallFrame* call_frame, Arguments& args, int required) {
    Exception* exc =
        Exception::make_argument_error(state, required, args.total(), args.name());
    exc->locations(state, Location::from_call_stack(state, call_frame));
    state->raise_exception(exc);

    return NULL;
  }

  Object* rbx_string_dup(STATE, CallFrame* call_frame, Object* obj) {
    CPP_TRY

    return as<String>(obj)->string_dup(state);

    CPP_CATCH
  }

  Object* rbx_meta_to_s(STATE, CallFrame* call_frame, InlineCache* cache,
                        Object* obj)
  {
    if(kind_of<String>(obj)) return obj;

    Arguments args(cache->name, obj, cNil, 0, 0);
    Object* ret = cache->execute(state, call_frame, args);
    if(!ret) return 0;

    if(!kind_of<String>(ret)) {
      ret = obj->to_s(state, false);
    }

    return ret;
  }

  Object* rbx_create_block(STATE, CallFrame* call_frame, int index) {
    CPP_TRY

    Object* _lit = call_frame->cm->literals()->at(state, index);
    CompiledCode* cm = as<CompiledCode>(_lit);

    // TODO: We don't need to be doing this everytime.
    if(cm->scope()->nil_p()) {
      cm->scope(state, call_frame->constant_scope());
    }

    VMMethod* vmm = call_frame->cm->backend_method();
    GCTokenImpl gct;
    return BlockEnvironment::under_call_frame(state, gct, cm, vmm, call_frame);

    CPP_CATCH
  }

  Object* rbx_create_block_multi(STATE, CompiledCode* cm, int count, ...) {
    va_list ap;

    CallFrame* closest = 0;
    VariableScope* top = 0;
    VariableScope* parent = 0;

    va_start(ap, count);
    for(int i = 0; i < count; i++) {
      closest = va_arg(ap, CallFrame*);
      closest->scope->set_parent(parent);
      parent = closest->promote_scope(state);

      if(!top) {
        top = parent;
      } else {
        closest->flags |= CallFrame::cMultipleScopes;
        closest->top_scope_ = top;
      }
    }
    va_end(ap);

    // TODO: We don't need to be doing this everytime.
    cm->scope(state, closest->constant_scope());

    VMMethod* vmm = closest->cm->backend_method();
    GCTokenImpl gct;
    return BlockEnvironment::under_call_frame(state, gct, cm, vmm, closest);
  }

  Object* rbx_promote_variables(STATE, CallFrame* call_frame) {
    return call_frame->promote_scope(state);
  }


  Object* rbx_construct_splat(STATE, Arguments& args, size_t start, size_t total) {
    int splat_size = args.total() - total;
    if(splat_size > 0) {
      Array* ary = Array::create(state, splat_size);

      for(int i = 0, n = start; i < splat_size; i++, n++) {
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
    Object* recv = G(type);
    Arguments args(G(sym_coerce_to_array), recv, 1, &top);
    Dispatch dis(G(sym_coerce_to_array));

    return dis.send(state, call_frame, args);
  }

  Object* rbx_destructure_args(STATE, CallFrame* call_frame, Arguments& args) {
    if(args.total() == 1) {
      Object* obj = args.get_argument(0);
      if(Array* ary = try_as<Array>(obj)) {
        args.use_array(ary);
      } else if(CBOOL(obj->respond_to(state, state->symbol("to_ary"), cFalse))) {
        obj = obj->send(state, call_frame, state->symbol("to_ary"));
        if(Array* ary2 = try_as<Array>(obj)) {
          args.use_array(ary2);
        } else {
          Exception::type_error(state, "to_ary must return an Array", call_frame);
          return 0;
        }
      }
    }

    return cNil;
  }

  Object* rbx_destructure_inline_args(STATE, CallFrame* call_frame, Object* obj,
                                      StackVariables* vars, int size)
  {
    Array* ary = try_as<Array>(obj);

    if(!ary && CBOOL(obj->respond_to(state, state->symbol("to_ary"), cFalse))) {
      obj = obj->send(state, call_frame, state->symbol("to_ary"));
      if(Array* ary2 = try_as<Array>(obj)) {
        ary = ary2;
      } else {
        Exception::type_error(state, "to_ary must return an Array", call_frame);
        return 0;
      }
    }

    if(ary) {
      size_t limit = MIN((int)ary->size(), size);

      for(size_t i = 0; i < limit; i++) {
        vars->set_local(i, ary->get(state, i));
      }
    } else {
      assert(size > 0);
      vars->set_local(0, obj);
    }

    return cNil;
  }

  Object* rbx_cast_multi_value(STATE, CallFrame* call_frame, Object* top) {
    if(kind_of<Array>(top)) return top;
    return Array::to_ary(state, top, call_frame);
  }

  Object* rbx_add_scope(STATE, CallFrame* call_frame, Object* top) {
    CPP_TRY

    Module* mod = as<Module>(top);
    ConstantScope* scope = ConstantScope::create(state);
    scope->module(state, mod);
    scope->parent(state, call_frame->constant_scope());
    call_frame->cm->scope(state, scope);
    // call_frame->constant_scope_ = scope;

    return cNil;

    CPP_CATCH
  }

  Object* rbx_cast_for_splat_block_arg(STATE, CallFrame* call_frame, Arguments& args) {
    if(args.total() == 1) {
      Object* obj = args.get_argument(0);
      if(!kind_of<Array>(obj)) {
        /* Yes, you are reading this code correctly: In Ruby 1.8, calling a
         * block with these forms { |*| } and { |*a| } with a single argument
         * that is not an Array and which responds to #to_ary will cause #to_ary
         * to be called and its return value ignored. Ultimately, the original
         * object itself is wrapped in an Array and passed to the block.
         */
        if(CBOOL(obj->respond_to(state, state->symbol("to_ary"), cFalse))) {
          OnStack<1> os(state, obj);
          Object* ignored = obj->send(state, call_frame, state->symbol("to_ary"));
          if(!ignored->nil_p() && !kind_of<Array>(ignored)) {
            Exception::type_error(state, "to_ary must return an Array", call_frame);
            return 0;
          }
        }
      }
      Array* ary = Array::create(state, 1);
      ary->set(state, 0, obj);
      return ary;
    } else {
      Array* ary = Array::create(state, args.total());
      for(size_t i = 0; i < args.total(); i++) {
        ary->set(state, i, args.get_argument(i));
      }
      return ary;
    }
  }

  Object* rbx_cast_for_multi_block_arg(STATE, CallFrame* call_frame, Arguments& args) {
    /* If there is only one argument and that thing is an array...
     AND the thing being invoked is not a lambda... */
    if(!(call_frame->flags & CallFrame::cIsLambda) &&
        args.total() == 1) {
      Object* obj = args.get_argument(0);
      if(kind_of<Array>(obj)) {
        return obj;
      } else if(CBOOL(obj->respond_to(state, state->symbol("to_ary"), cFalse))) {
        obj = obj->send(state, call_frame, state->symbol("to_ary"));
        if(kind_of<Array>(obj)) {
          return obj;
        } else {
          Exception::type_error(state, "to_ary must return an Array", call_frame);
          return 0;
        }
      }
      // One argument and it's not Array or Array-ish at all, so fall through
      // and let it be wrapped in an array.
    }

    Array* ary = Array::create(state, args.total());
    assert(kind_of<Array>(ary));

    for(size_t i = 0; i < args.total(); i++) {
      assert(kind_of<Array>(ary));
      ary->set(state, i, args.get_argument(i));
    }

    assert(kind_of<Array>(ary));

    return ary;
  }

  Object* rbx_cast_for_splat_block_arg_varargs(STATE, CallFrame* call_frame,
                                               int count, ...)
  {
    va_list ap;

    if(count == 1) {
      va_start(ap, count);

      Object* obj = va_arg(ap, Object*);

      if(!kind_of<Array>(obj)) {
        /* Yes, you are reading this code correctly: In Ruby 1.8, calling a
         * block with these forms { |*| } and { |*a| } with a single argument
         * that is not an Array and which responds to #to_ary will cause #to_ary
         * to be called and its return value ignored. Ultimately, the original
         * object itself is wrapped in an Array and passed to the block.
         */
        if(CBOOL(obj->respond_to(state, state->symbol("to_ary"), cFalse))) {
          OnStack<1> os(state, obj);
          Object* ignored = obj->send(state, call_frame, state->symbol("to_ary"));
          if(!kind_of<Array>(ignored)) {
            Exception::type_error(state, "to_ary must return an Array", call_frame);
            obj = 0;
          }
        }
      }

      Array* ary = Array::create(state, 1);
      ary->set(state, 0, obj);
      obj = ary;

      va_end(ap);
      return obj;
    }

    Array* ary = Array::create(state, count);

    va_start(ap, count);
    for(int i = 0; i < count; i++) {
      ary->set(state, i, va_arg(ap, Object*));
    }
    va_end(ap);

    return ary;
  }

  Object* rbx_cast_for_multi_block_arg_varargs(STATE, CallFrame* call_frame,
                                               int count, ...)
  {
    va_list ap;

    /* If there is only one argument and that thing is an array... */
    if(count == 1) {
      va_start(ap, count);

      Object* obj = va_arg(ap, Object*);

      if(kind_of<Array>(obj)) {
        // Nothing! it's good.
      } else if(CBOOL(obj->respond_to(state, state->symbol("to_ary"), cFalse))) {
        obj = obj->send(state, call_frame, state->symbol("to_ary"));
        if(!kind_of<Array>(obj)) {
          Exception::type_error(state, "to_ary must return an Array", call_frame);
          obj = 0;
        }
      } else {
        Array* ary = Array::create(state, 1);
        ary->set(state, 0, obj);
        obj = ary;
      }

      va_end(ap);
      return obj;
    }

    Array* ary = Array::create(state, count);

    va_start(ap, count);
    for(int i = 0; i < count; i++) {
      ary->set(state, i, va_arg(ap, Object*));
    }
    va_end(ap);

    return ary;
  }

  Object* rbx_cast_for_single_block_arg(STATE, Arguments& args) {
    int k = args.total();
    if(k == 0) {
      return cNil;
    } else if(k == 1) {
      return args.get_argument(0);
    }

    Array* ary = Array::create(state, k);
    for(int i = 0; i < k; i++) {
      ary->set(state, i, args.get_argument(i));
    }

    return ary;
  }

  Object* rbx_check_serial(STATE, CallFrame* call_frame, InlineCache* cache,
                           int serial, Object* recv)
  {
    if(cache->update_and_validate(state, call_frame, recv)) {
      MethodCacheEntry* mce = cache->cache();

      if(mce && mce->method()->serial()->to_native() == serial) return cTrue;
    }

    return cFalse;
  }

  Object* rbx_check_serial_private(STATE, CallFrame* call_frame, InlineCache* cache,
                           int serial, Object* recv)
  {
    if(cache->update_and_validate_private(state, call_frame, recv)) {
      MethodCacheEntry* mce = cache->cache();

      if(mce && mce->method()->serial()->to_native() == serial) return cTrue;
    }

    return cFalse;
  }

  Object* rbx_find_const(STATE, CallFrame* call_frame, int index, Object* top) {
    CPP_TRY

    GCTokenImpl gct;
    bool found;
    Module* under = as<Module>(top);
    Symbol* sym = as<Symbol>(call_frame->cm->literals()->at(state, index));
    Object* res = Helpers::const_get_under(state, under, sym, &found);

    if(!found) {
      res = Helpers::const_missing_under(state, under, sym, call_frame);
    } else if(Autoload* autoload = try_as<Autoload>(res)) {
      res = autoload->resolve(state, gct, call_frame, under);
    }

    return res;

    CPP_CATCH
  }

  Object* rbx_instance_of(STATE, CallFrame* call_frame, Object* top, Object* b1) {
    CPP_TRY

    Class* cls = as<Class>(b1);
    if(top->class_object(state) == cls) return cTrue;
    return cFalse;

    CPP_CATCH
  }

  Object* rbx_kind_of(STATE, Object* top, Object* b1) {
    return top->kind_of_p(state, b1) ? cTrue : cFalse;
  }

  Object* rbx_make_array(STATE, int count, Object** args) {
    Array* ary = Array::create(state, count);
    for(int i = 0; i < count; i++) {
      ary->set(state, i, args[i]);
    }

    return ary;
  }

  Object* rbx_create_array(STATE, int count, ...) {
    va_list ap;

    Array* ary = Array::create(state, count);

    va_start(ap, count);
    for(int i = 0; i < count; i++) {
      Object* obj = va_arg(ap, Object*);
      ary->set(state, i, obj);
    }

    va_end(ap);

    return ary;
  }

  Object* rbx_meta_send_call(STATE, CallFrame* call_frame, int count, Object** args) {
    Object* t1 = args[0];

    Arguments out_args(G(sym_call), cNil, count, args+1);

    if(BlockEnvironment *env = try_as<BlockEnvironment>(t1)) {
      return env->call(state, call_frame, out_args);
    } else if(Proc* proc = try_as<Proc>(t1)) {
      return proc->call(state, call_frame, out_args);
    }

    return rbx_simple_send(state, call_frame, G(sym_call), count, args);
  }

  Object* rbx_yield_stack(STATE, CallFrame* call_frame, Object* block,
                          int count, Object** args)
  {
    Arguments out_args(G(sym_call), block, count, args);

    if(BlockEnvironment *env = try_as<BlockEnvironment>(block)) {
      return env->call(state, call_frame, out_args);
    } else if(Proc* proc = try_as<Proc>(block)) {
      return proc->yield(state, call_frame, out_args);
    } else if(block->nil_p()) {
      state->raise_exception(Exception::make_lje(state, call_frame));
      return NULL;
    }

    Dispatch dis(G(sym_call));

    return dis.send(state, call_frame, out_args);
  }

  Object* rbx_yield_splat(STATE, CallFrame* call_frame, Object* block,
                          int count, Object** stk)
  {
    Object* ary = stk[count];

    Arguments args(G(sym_call), block, count, stk);

    if(!ary->nil_p()) {
      args.append(state, as<Array>(ary));
    }

    if(BlockEnvironment *env = try_as<BlockEnvironment>(block)) {
      return env->call(state, call_frame, args);
    } else if(Proc* proc = try_as<Proc>(block)) {
      return proc->yield(state, call_frame, args);
    } else if(block->nil_p()) {
      state->raise_exception(Exception::make_lje(state, call_frame));
      return NULL;
    }

    Dispatch dis(G(sym_call));
    return dis.send(state, call_frame, args);
  }

  Object* rbx_meta_send_op_gt(STATE, CallFrame* call_frame, Object** stk) {
    CPP_TRY

    Object* t1 = stk[0];
    Object* t2 = stk[1];
    if(both_fixnum_p(t1, t2)) {
      native_int j = as<Integer>(t1)->to_native();
      native_int k = as<Integer>(t2)->to_native();
      return (j > k) ? cTrue : cFalse;
    }

    return rbx_simple_send(state, call_frame, G(sym_gt), 1, stk);

    CPP_CATCH
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
      return (t1 == t2) ? cFalse : cTrue;
    }

    return rbx_simple_send(state, call_frame, G(sym_nequal), 1, stk);
  }

  Object* rbx_meta_send_op_tequal(STATE, CallFrame* call_frame, Object** stk) {
    Object* t1 = stk[0];
    Object* t2 = stk[1];
    /* If both are fixnums, or both are symbols, compare the ops directly. */
    if((t1->fixnum_p() && t2->fixnum_p()) || (t1->symbol_p() && t2->symbol_p())) {
      return (t1 == t2) ? cFalse : cTrue;
    }

    return rbx_simple_send(state, call_frame, G(sym_tequal), 1, stk);
  }

  Object* rbx_passed_arg(STATE, Arguments& args, int index) {
    return (index < (int)args.total()) ? cTrue : cFalse;
  }

  // TODO remove this and use passed_arg
  Object* rbx_passed_blockarg(STATE, Arguments& args, int index) {
    return (index == (int)args.total()) ? cTrue : cFalse;
  }

  Object* rbx_push_const(STATE, CallFrame* call_frame, Symbol* sym) {
    GCTokenImpl gct;
    bool found;
    Object* res = Helpers::const_get(state, call_frame, sym, &found);

    if(!found) {
      res = Helpers::const_missing(state, sym, call_frame);
    } else if(Autoload* autoload = try_as<Autoload>(res)) {
      res = autoload->resolve(state, gct, call_frame);
    }

    return res;
  }

  Object* rbx_push_const_fast(STATE, CallFrame* call_frame, Symbol* sym,
                              int association_index) {
    Object* res = 0;

    Object* val = call_frame->cm->literals()->at(state, association_index);

    // See if the cache is present, if so, validate it and use the value
    GlobalCacheEntry* cache;
    if((cache = try_as<GlobalCacheEntry>(val)) != NULL) {
      if(cache->valid_p(state, call_frame->constant_scope())) {
        res = cache->value();
      }
    } else {
      cache = GlobalCacheEntry::empty(state);
      call_frame->cm->literals()->put(state, association_index, cache);
    }

    if(!res) {
      bool found = false;
      res = Helpers::const_get(state, call_frame, sym, &found);

      if(found) {
        GCTokenImpl gct;
        OnStack<2> os(state, cache, res);
        if(Autoload* autoload = try_as<Autoload>(res)) {
          res = autoload->resolve(state, gct, call_frame);
        }

        if(res) {
          cache->update(state, res, call_frame->constant_scope());
        }
      } else {
        res = Helpers::const_missing(state, sym, call_frame);
      }
    }

    return res;
  }

  Object* rbx_set_local_depth(STATE, CallFrame* call_frame, Object* top,
                              int depth, int index) {
    if(depth == 0) {
      Exception::internal_error(state, call_frame,
                                "illegal set_local_depth usage");
      return 0;
    } else {
      VariableScope* scope = call_frame->scope->parent();

      if(!scope || scope->nil_p()) {
        Exception::internal_error(state, call_frame,
                                  "illegal set_local_depth usage, no parent");
        return 0;
      }

      for(int j = 1; j < depth; j++) {
        scope = scope->parent();
        if(!scope || scope->nil_p()) {
          Exception::internal_error(state, call_frame,
                                    "illegal set_local_depth usage, no parent");
          return 0;
        }
      }

      if(index >= scope->number_of_locals()) {
        Exception::internal_error(state, call_frame,
                                  "illegal set_local_depth usage, bad index");
        return 0;
      }

      scope->set_local(state, index, top);
    }

    return top;
  }

  Object* rbx_set_local_from(STATE, CallFrame* call_frame, Object* top,
                             int depth, int index) {
    VariableScope* scope = call_frame->scope->parent();

    if(!scope || scope->nil_p()) {
      Exception::internal_error(state, call_frame,
                                "illegal set_local_depth usage, no parent");
      return 0;
    }

    for(int j = 1; j < depth; j++) {
      scope = scope->parent();
      if(!scope || scope->nil_p()) {
        Exception::internal_error(state, call_frame,
                                  "illegal set_local_depth usage, no parent");
        return 0;
      }

    }

    scope->set_local(state, index, top);

    return top;
  }

  Object* rbx_push_local_depth(STATE, CallFrame* call_frame,
                              int depth, int index) {
    if(depth == 0) {
      Exception::internal_error(state, call_frame,
                                "illegal push_local_depth usage");
      return 0;
    } else {
      VariableScope* scope = call_frame->scope->parent();

      if(!scope || scope->nil_p()) {
        Exception::internal_error(state, call_frame,
                                  "illegal push_local_depth usage, no parent");
        return 0;
      }

      for(int j = 1; j < depth; j++) {
        scope = scope->parent();
        if(!scope || scope->nil_p()) {
          Exception::internal_error(state, call_frame,
                                    "illegal push_local_depth usage, no parent");
          return 0;
        }
      }

      if(index >= scope->number_of_locals()) {
        Exception::internal_error(state, call_frame,
                                  "illegal push_local_depth usage, bad index");
        return 0;
      }

      return scope->get_local(index);
    }
  }

  Object* rbx_push_local_from(STATE, CallFrame* call_frame,
                              int depth, int index) {
    VariableScope* scope = call_frame->scope->parent();

    if(!scope || scope->nil_p()) {
      Exception::internal_error(state, call_frame,
                                "illegal push_local_depth usage, no parent");
      return 0;
    }

    for(int j = 1; j < depth; j++) {
      scope = scope->parent();

      if(!scope || scope->nil_p()) {
        Exception::internal_error(state, call_frame,
                                  "illegal push_local_depth usage, no parent");
        return 0;
      }
    }

    return scope->get_local(index);
  }

  Object* rbx_prologue_check(STATE, CallFrame* call_frame) {
    GCTokenImpl gct;

    if(!state->check_interrupts(gct, call_frame, &state)) return NULL;

    state->checkpoint(gct, call_frame);

    return cTrue;
  }

  Object* rbx_check_interrupts(STATE, CallFrame* call_frame) {
    GCTokenImpl gct;

    if(!state->check_async(call_frame)) return NULL;

    state->checkpoint(gct, call_frame);
    return cTrue;
  }

  int rbx_enter_unmanaged(STATE, CallFrame* call_frame) {
    GCTokenImpl gct;
    state->set_call_frame(call_frame);
    state->gc_independent(gct);
    return 0;
  }

  int rbx_exit_unmanaged(STATE, CallFrame* call_frame) {
    state->set_call_frame(call_frame);
    state->gc_dependent();
    return 0;
  }

  bool rbx_return_to_here(STATE, CallFrame* call_frame) {
    ThreadState* th = state->vm()->thread_state();
    if(th->raise_reason() != cReturn) return false;
    if(th->destination_scope() == call_frame->scope->on_heap()) return true;
    return false;
  }

  bool rbx_break_to_here(STATE, CallFrame* call_frame) {
    ThreadState* th = state->vm()->thread_state();
    if(th->raise_reason() != cBreak) return false;
    if(th->destination_scope() == call_frame->scope->on_heap()) return true;
    return false;
  }

  Object* rbx_clear_raise_value(STATE) {
    Object* val = state->vm()->thread_state()->raise_value();
    state->vm()->thread_state()->clear_return();
    return val;
  }

  bool rbx_raising_exception(STATE) {
    return state->vm()->thread_state()->raise_reason() == cException;
  }

  Object* rbx_current_exception(STATE) {
    return state->vm()->thread_state()->current_exception();
  }

  Object* rbx_clear_exception(STATE) {
    state->vm()->thread_state()->clear_raise();
    return cNil;
  }

  Object* rbx_push_exception_state(STATE) {
    return state->vm()->thread_state()->state_as_object(state);
  }

  Object* rbx_restore_exception_state(STATE, CallFrame* call_frame, Object* top) {
    if(top->nil_p()) {
      state->vm()->thread_state()->clear();
    } else {
      state->vm()->thread_state()->set_state(state, top);
    }

    return cNil;
  }

  Object* rbx_push_system_object(STATE, int which) {
    switch(which) {
    case 0:
      return G(object);
    case 1:
      return G(rubinius);
    case 2:
      return G(type);
    default:
      return cNil;
    }
  }

  Object* rbx_push_ivar(STATE, Object* self, Symbol* name) {
    return self->get_ivar(state, name);
  }

  Object* rbx_set_ivar(STATE, CallFrame* call_frame, Object* self, Symbol* name, Object* val) {
    if(self->reference_p() && self->is_frozen_p()) {
      Exception::frozen_error(state, call_frame);
      return NULL;
    }

    return self->set_ivar(state, name, val);
  }

  Object* rbx_push_my_field(STATE, Object* self, int which) {
    return self->get_field(state, which);
  }

  Object* rbx_set_my_field(STATE, Object* self, int which, Object* val) {
    self->set_field(state, which, val);
    return val;
  }

  Object* rbx_set_const(STATE, CallFrame* call_frame, Symbol* name, Object* val) {
    call_frame->constant_scope()->module()->set_const(state, name, val);
    return val;
  }

  Object* rbx_set_const_at(STATE, Symbol* name, Object* mod, Object* val) {
    // TODO if the as<> fails, the process will abort().
    as<Module>(mod)->set_const(state, name, val);
    return val;
  }

  Object* rbx_set_literal(STATE, CallFrame* call_frame, int which, Object* val) {
    call_frame->cm->literals()->put(state, which, val);
    return cNil;
  }

  Object* rbx_shift_array(STATE, CallFrame* call_frame, Object** loc) {
    CPP_TRY

    Array* array = as<Array>(*loc);
    size_t size = (size_t)array->size();

    if(size == 0) return cNil;

    size_t j = size - 1;
    Object* shifted_value = array->get(state, 0);
    Array* smaller_array = Array::create(state, j);

    for(size_t i = 0; i < j; i++) {
      smaller_array->set(state, i, array->get(state, i+1));
    }

    *loc = smaller_array;
    return shifted_value;

    CPP_CATCH
  }

  Object* rbx_string_append(STATE, CallFrame* call_frame, Object* left, Object* right) {
    CPP_TRY

    return as<String>(left)->append(state, as<String>(right));

    CPP_CATCH
  }

  Object* rbx_string_build(STATE, CallFrame* call_frame, int count, Object** parts) {
    size_t size = 0;

    bool tainted = false;
    bool untrusted = false;

    bool check_encoding = false;
    Encoding* enc = nil<Encoding>();

    // Figure out the total size
    for(int i = 0; i < count; i++) {
      Object* obj = parts[i];
      String* str = try_as<String>(obj);

      if(obj->reference_p()) {
        tainted |= obj->is_tainted_p();
        untrusted |= obj->is_untrusted_p();
      }

      if(str) {
        native_int cur_size = str->byte_size();
        native_int data_size = as<ByteArray>(str->data())->size();
        if(unlikely(cur_size > data_size)) {
          cur_size = data_size;
        }
        size += cur_size;
      } else {
        // This isn't how MRI does this. If sub isn't a String, it converts
        // the original object via any_to_s, not the bad value returned from #to_s.
        // This quite a bit harder to implement in rubinius atm, so I'm opting for
        // this way instead.

        str = obj->to_s(state, false);

        tainted |= str->is_tainted_p();
        untrusted |= str->is_untrusted_p();
        native_int cur_size = str->byte_size();
        native_int data_size = as<ByteArray>(str->data())->size();
        if(unlikely(cur_size > data_size)) {
          cur_size = data_size;
        }
        size += cur_size;

        parts[i] = str;
      }

      if(!LANGUAGE_18_ENABLED(state)) {
        /* The String::encoding() accessor (without state) returns the raw
         * Encoding attribute. If it is only ever cNil or all values are the
         * same, we don't need to check Encoding compatibility later.
         *
         * TODO: Consider the case when -K is set (not implemented yet).
         */
        if(!check_encoding) {
          Encoding* str_enc = str->encoding();
          if(!str_enc->nil_p()) {
            if(enc->nil_p()) {
              enc = str_enc;
            } else if(str_enc != enc) {
              check_encoding = true;
              enc = nil<Encoding>();
            }
          }
        }
      }
    }

    String* str = String::create(state, 0, size);
    uint8_t* pos = str->byte_address();
    native_int str_size = 0;

    for(int i = 0; i < count; i++) {
      // We can force here because we've typed check them above.
      String* sub = force_as<String>(parts[i]);

      native_int sub_size = sub->byte_size();
      native_int data_size = as<ByteArray>(sub->data())->size();
      if(unlikely(sub_size > data_size)) {
        sub_size = data_size;
      }

      if(!LANGUAGE_18_ENABLED(state)) {
        if(check_encoding) {
          if(i < count - 1) {
            str->num_bytes(state, Fixnum::from(str_size));

            Encoding* enc = Encoding::compatible_p(state, str, sub);

            if(enc->nil_p()) {
              Exception::encoding_compatibility_error(state, str, sub, call_frame);
              return 0;
            } else {
              str->encoding(state, enc);
            }
          } else {
            str->encoding(state, sub->encoding());
          }
        }
      }

      memcpy(pos + str_size, sub->byte_address(), sub_size);
      str_size += sub_size;
    }

    if(!LANGUAGE_18_ENABLED(state)) {
      /* We had to set the size of the result String before every Encoding check
       * so we have to set it to the final size here.
       */
      if(check_encoding) str->num_bytes(state, Fixnum::from(size));
      if(!enc->nil_p()) str->encoding(state, enc);
    }

    if(tainted) str->set_tainted();
    if(untrusted) str->set_untrusted();

    return str;
  }

  Object* rbx_raise_return(STATE, CallFrame* call_frame, Object* top) {
    if(!(call_frame->flags & CallFrame::cIsLambda) &&
       !call_frame->scope_still_valid(call_frame->top_scope(state))) {
      Exception* exc = Exception::make_exception(state, G(jump_error), "unexpected return");
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->raise_exception(exc);
    } else {
      if(call_frame->flags & CallFrame::cIsLambda) {
        state->vm()->thread_state()->raise_return(top, call_frame->promote_scope(state));
      } else {
        state->vm()->thread_state()->raise_return(top, call_frame->top_scope(state));
      }
    }

    return cNil;
  }

  Object* rbx_ensure_return(STATE, CallFrame* call_frame, Object* top) {
    state->vm()->thread_state()->raise_return(top, call_frame->promote_scope(state));
    return cNil;
  }

  Object* rbx_raise_break(STATE, CallFrame* call_frame, Object* top) {
    if(call_frame->flags & CallFrame::cIsLambda) {
      // We have to use raise_return here because the jit code
      // jumps to raising the exception right away.
      state->vm()->thread_state()->raise_return(top,
                                          call_frame->promote_scope(state));
    } else if(call_frame->scope_still_valid(call_frame->scope->parent())) {
      state->vm()->thread_state()->raise_break(top, call_frame->scope->parent());
    } else {
      Exception* exc = Exception::make_exception(state, G(jump_error), "attempted to break to exited method");
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->raise_exception(exc);
    }
    return cNil;
  }

  bool rbx_check_class(STATE, Object* obj, int id) {
    return obj->lookup_begin(state)->class_id() == id;
  }

  Object* rbx_get_ivar(STATE, Object* self, Symbol* name) {
    return self->get_ivar(state, name);
  }

  void rbx_set_table_ivar(STATE, Object* self, Symbol* name, Object* val) {
    self->set_table_ivar(state, name, val);
  }

  Object* rbx_continue_uncommon(STATE, CallFrame* call_frame,
                                int32_t entry_ip, native_int sp,
                                CallFrame* method_call_frame,
                                jit::RuntimeDataHolder* rd,
                                int32_t unwind_count,
                                int32_t* unwinds)
  {
    LLVMState::get(state)->add_uncommons_taken();

    VMMethod* vmm = call_frame->cm->backend_method();

    /*
    InlineCache* cache = 0;

    if(vmm->opcodes[call_frame->ip()] == InstructionSequence::insn_send_stack) {
      cache = reinterpret_cast<InlineCache*>(vmm->opcodes[call_frame->ip() + 1]);
    } else if(vmm->opcodes[call_frame->ip()] == InstructionSequence::insn_send_method) {
      cache = reinterpret_cast<InlineCache*>(vmm->opcodes[call_frame->ip() + 1]);
    } else if(vmm->opcodes[call_frame->ip()] == InstructionSequence::insn_send_stack_with_block) {
      cache = reinterpret_cast<InlineCache*>(vmm->opcodes[call_frame->ip() + 1]);
    }

    if(cache && cache->name->symbol_p()) {
      std::cout << "Uncommon trap for send: " << cache->name->c_str(state) << "\n";
    } else {
      std::cout << "Unknown uncommon try reason.\n";
    }
    */

    if(call_frame->is_inline_frame()) {
      // Fix up this inlined block.
      if(vmm->parent()) {
        CallFrame* creator = call_frame->previous->previous;
        assert(creator);

        VariableScope* parent = creator->promote_scope(state);
        call_frame->scope->set_parent(parent);

        // Only support one depth!
        assert(!creator->cm->backend_method()->parent());
      }
    }

    return VMMethod::uncommon_interpreter(state, vmm, call_frame,
                                          entry_ip, sp,
                                          method_call_frame, rd,
                                          unwind_count, unwinds);
  }

  Object* rbx_restart_interp(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args) {
    return VMMethod::execute(state, call_frame, exec, mod, args);
  }

  Object* rbx_continue_debugging(STATE, CallFrame* call_frame,
                                 int32_t entry_ip, native_int sp,
                                 CallFrame* method_call_frame,
                                 int32_t unwind_count,
                                 int32_t* input_unwinds,
                                 Object* top_of_stack)
  {
    VMMethod* vmm = call_frame->cm->backend_method();

    if(call_frame->is_inline_frame()) {
      // Fix up this inlined block.
      if(vmm->parent()) {
        CallFrame* creator = call_frame->previous->previous;
        assert(creator);

        VariableScope* parent = creator->promote_scope(state);
        call_frame->scope->set_parent(parent);

        // Only support one depth!
        assert(!creator->cm->backend_method()->parent());
      }
    }

    call_frame->ip_ = entry_ip;

    VMMethod::InterpreterState is;
    UnwindInfo unwinds[kMaxUnwindInfos];

    for(int i = 0, j = 0; j < unwind_count; i += 3, j++) {
      UnwindInfo& uw = unwinds[j];
      uw.target_ip = input_unwinds[i];
      uw.stack_depth = input_unwinds[i + 1];
      uw.type = (UnwindType)input_unwinds[i + 2];
    }

    // Push the top of the stack into the call_frame->stk so the interpreter
    // sees it. This is done here rather than by the JIT to simplify the
    // JIT's callsite.
    if(top_of_stack) {
      call_frame->stk[++sp] = top_of_stack;
    }

    return VMMethod::debugger_interpreter_continue(state, vmm, call_frame,
                                          sp, is, unwind_count, unwinds);
  }

  Object* rbx_flush_scope(STATE, StackVariables* vars) {
    vars->flush_to_heap(state);
    return cNil;
  }

  // FFI helpers
  native_int rbx_ffi_to_int(STATE, Object* obj, bool* valid) {
    if(Integer* i = try_as<Integer>(obj)) {
      *valid = true;
      return i->to_native();
    }

    Exception* exc =
      Exception::make_type_error(state, Fixnum::type, obj, "invalid type for FFI");
    state->raise_exception(exc);

    *valid = false;
    return 0;
  }

  float rbx_ffi_to_float(STATE, Object* obj, bool* valid) {
    if(Float* i = try_as<Float>(obj)) {
      *valid = true;
      return i->val;
    }

    Exception* exc =
      Exception::make_type_error(state, Float::type, obj, "invalid type for FFI");
    state->raise_exception(exc);

    *valid = false;
    return 0.0;
  }

  double rbx_ffi_to_double(STATE, Object* obj, bool* valid) {
    if(Float* i = try_as<Float>(obj)) {
      *valid = true;
      return i->val;
    }

    Exception* exc =
      Exception::make_type_error(state, Float::type, obj, "invalid type for FFI");
    state->raise_exception(exc);

    *valid = false;
    return 0.0;
  }

  uint64_t rbx_ffi_to_int64(STATE, Object* obj, bool* valid) {
    if(Integer* i = try_as<Integer>(obj)) {
      *valid = true;
      return i->to_long_long();
    }

    Exception* exc =
      Exception::make_type_error(state, Fixnum::type, obj, "invalid type for FFI");
    state->raise_exception(exc);

    *valid = false;
    return 0ULL;
  }

  void* rbx_ffi_to_ptr(STATE, Object* obj, bool* valid) {
    if(Pointer* ptr = try_as<Pointer>(obj)) {
      *valid = true;
      return ptr->pointer;
    } else if(obj->nil_p()) {
      *valid = true;
      return 0;
    }

    Exception* exc =
      Exception::make_type_error(state, Fixnum::type, obj, "invalid type for FFI");
    state->raise_exception(exc);

    *valid = false;
    return 0;
  }

  char* rbx_ffi_to_string(STATE, Object* obj, bool* valid) {
    if(String* str = try_as<String>(obj)) {
      *valid = true;
      return const_cast<char*>(str->c_str(state));
    }

    Exception* exc =
      Exception::make_type_error(state, Fixnum::type, obj, "invalid type for FFI");
    state->raise_exception(exc);

    *valid = false;
    return 0;
  }

  Object* rbx_ffi_from_int32(STATE, int32_t ll) {
    return Integer::from(state, ll);
  }

  Object* rbx_ffi_from_int64(STATE, int64_t ll) {
    return Integer::from(state, ll);
  }

  Object* rbx_ffi_from_float(STATE, float val) {
    return Float::create(state, val);
  }

  Object* rbx_ffi_from_double(STATE, double val) {
    return Float::create(state, val);
  }

  Object* rbx_ffi_from_ptr(STATE, void* ptr) {
    if(!ptr) return cNil;
    return Pointer::create(state, ptr);
  }

  Object* rbx_ffi_from_string(STATE, char* ptr) {
    if(!ptr) return cNil;
    String* str = String::create(state, ptr);
    str->taint(state);
    return str;
  }

  Object* rbx_ffi_from_string_with_pointer(STATE, char* ptr) {
    Object* s;
    Object* p;

    if(ptr) {
      s = String::create(state, ptr);
      s->taint(state);
      p = Pointer::create(state, ptr);
    } else {
      s = p = cNil;
    }

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, s);
    ary->set(state, 1, p);

    return ary;
  }

  Float* rbx_float_allocate(STATE) {
    return Float::create(state, 0.0);
  }

  Class* rbx_class_of(STATE, Object* obj) {
    return obj->class_object(state);
  }

  Object* rbx_make_proc(STATE, CallFrame* call_frame) {
    Object* obj = call_frame->scope->block();
    if(CBOOL(obj)) {
      Proc* prc;
      if(BlockEnvironment *env = try_as<BlockEnvironment>(obj)) {
        prc = Proc::create(state, G(proc));
        prc->block(state, env);
      } else if(Proc* p = try_as<Proc>(obj)) {
        prc = p;
      } else {
        Exception::internal_error(state, call_frame, "invalid block type");
        return 0;
      }

      return prc;
    } else {
      return cNil;
    }
  }

  Object* rbx_string_hash(STATE, String* str) {
    return Integer::from(state, str->hash_string(state));
  }

  Object* rbx_create_instance(STATE, CallFrame* call_frame, Class* cls) {
    CPP_TRY

    GCTokenImpl gct;
    return cls->allocate(state, gct, call_frame);

    CPP_CATCH
  }
}

#endif

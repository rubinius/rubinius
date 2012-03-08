/* A simple C++ wrapper around libffi. */

#include "vm/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#ifdef RBX_WINDOWS
#include <malloc.h>
#else
#include <dlfcn.h>
#endif

#include "prelude.hpp"
#include "object_utils.hpp"
#include "vm.hpp"
#include "objectmemory.hpp"
#include "on_stack.hpp"

#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/float.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/nativefunction.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/system.hpp"
#include "builtin/location.hpp"
#include "builtin/nativemethod.hpp"

#include "ontology.hpp"

#include "ffi_util.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"

#include "builtin/nativefunction.hpp"

#include "instruments/tooling.hpp"

namespace rubinius {

  void NativeFunction::init(STATE) {
    GO(native_function).set(ontology::new_class(state,
          "NativeFunction", G(executable), G(rubinius)));
    G(native_function)->set_object_type(state, NativeFunctionType);
  }

  FFIData* FFIData::create(NativeFunction* func, int count, int* types,
                           int ret, NativeFunction** callbacks)
  {
    void* ep;

    FFIData* data = new FFIData(func, count, types, ret, callbacks);
    data->closure = reinterpret_cast<ffi_closure*>(
                      ffi_closure_alloc(sizeof(ffi_closure), &ep));

    data->ep = ep;

    return data;
  }

  FFIData::FFIData(NativeFunction* func,  int count, int* types, int ret, NativeFunction** callbacks)
    : closure(0)
    , callable(0)
    , function(func)
    , callbacks(callbacks)
    , arg_count(count)
    , arg_types(types)
    , ret_type(ret)
  {}

  FFIData::~FFIData() {
    free(arg_types);
    XFREE(cif.arg_types);
    if(closure) ffi_closure_free(closure);
  }

  /* Run when a NativeFunction is executed.  Executes the related C function.
   */
  Object* NativeFunction::execute(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                                  Arguments& args) {
    NativeFunction* nfunc = as<NativeFunction>(exec);

    state->set_call_frame(call_frame);

    try {

#ifdef RBX_PROFILER
      if(unlikely(state->vm()->tooling())) {
        tooling::MethodEntry method(state, exec, mod, args);
        return nfunc->call(state, args, call_frame);
      } else {
        return nfunc->call(state, args, call_frame);
      }
#else
      return nfunc->call(state, args, msg, call_frame);
#endif

    } catch(TypeError &e) {
      Exception* exc =
        Exception::make_type_error(state, e.type, e.object, e.reason);
      exc->locations(state, Location::from_call_stack(state, call_frame));

      state->raise_exception(exc);
      return NULL;
    }
  }

  size_t NativeFunction::type_size(size_t type) {
    switch(type) {
      case RBX_FFI_TYPE_CHAR:
      case RBX_FFI_TYPE_UCHAR:
      case RBX_FFI_TYPE_BOOL:
        return sizeof(char);

      case RBX_FFI_TYPE_SHORT:
      case RBX_FFI_TYPE_USHORT:
        return sizeof(short);

      case RBX_FFI_TYPE_INT:
      case RBX_FFI_TYPE_UINT:
        return sizeof(int);

      case RBX_FFI_TYPE_LONG:
      case RBX_FFI_TYPE_ULONG:
        return sizeof(long);

      case RBX_FFI_TYPE_LONG_LONG:
      case RBX_FFI_TYPE_ULONG_LONG:
        return sizeof(long long);

      case RBX_FFI_TYPE_FLOAT:
        return sizeof(float);

      case RBX_FFI_TYPE_DOUBLE:
        return sizeof(double);

      case RBX_FFI_TYPE_PTR:
      case RBX_FFI_TYPE_STRING:
      case RBX_FFI_TYPE_STATE:
      case RBX_FFI_TYPE_STRPTR:
      case RBX_FFI_TYPE_OBJECT:
        return sizeof(void*);

      default:
        rubinius::bug("Unsupported FFI type");
		return 0;	// Keep GCC happy
    }
  }

  Fixnum* NativeFunction::type_size_prim(STATE, Fixnum* type) {
    return Fixnum::from(NativeFunction::type_size(type->to_native()));
  }

  NativeFunction* NativeFunction::create(STATE, Symbol* name, int args) {
    NativeFunction* nf = state->new_object<NativeFunction>(G(native_function));
    nf->primitive(state, state->symbol("nativefunction_call"));
    nf->required(state, Fixnum::from(args));
    nf->serial(state, Fixnum::from(0));
    nf->name(state, name);
    nf->file(state, state->symbol("<system>"));

    nf->set_executor(NativeFunction::execute);
    nf->inliners_ = 0;
    nf->ffi_data = 0;

    return nf;
  }

  void NativeFunction::prep(STATE, int arg_count, int *arg_types,
                            int ret_type, NativeFunction** callbacks) {

    ffi_type** types;
    ffi_status status;
    ffi_type* rtype;
    int i;

    types = ALLOC_N(ffi_type*, arg_count);

    for(i = 0; i < arg_count; i++) {
      switch(arg_types[i]) {
      case RBX_FFI_TYPE_CHAR:
        types[i] = &ffi_type_schar;
        break;
      case RBX_FFI_TYPE_UCHAR:
      case RBX_FFI_TYPE_BOOL:
        types[i] = &ffi_type_uchar;
        break;
      case RBX_FFI_TYPE_SHORT:
        types[i] = &ffi_type_sshort;
        break;
      case RBX_FFI_TYPE_USHORT:
        types[i] = &ffi_type_ushort;
        break;
      case RBX_FFI_TYPE_INT:
        types[i] = &ffi_type_sint;
        break;
      case RBX_FFI_TYPE_UINT:
        types[i] = &ffi_type_uint;
        break;
      case RBX_FFI_TYPE_LONG:
        types[i] = &ffi_type_slong;
        break;
      case RBX_FFI_TYPE_ULONG:
        types[i] = &ffi_type_ulong;
        break;
      case RBX_FFI_TYPE_FLOAT:
        types[i] = &ffi_type_float;
        break;
      case RBX_FFI_TYPE_DOUBLE:
        types[i] = &ffi_type_double;
        break;
      case RBX_FFI_TYPE_LONG_LONG:
        types[i] = &ffi_type_sint64;
        break;
      case RBX_FFI_TYPE_ULONG_LONG:
        types[i] = &ffi_type_uint64;
        break;
      case RBX_FFI_TYPE_OBJECT:
      case RBX_FFI_TYPE_PTR:
      case RBX_FFI_TYPE_CALLBACK:
      case RBX_FFI_TYPE_STRING:
      case RBX_FFI_TYPE_STATE:
        types[i] = &ffi_type_pointer;
        break;
      }
    }

    switch(ret_type) {
    case RBX_FFI_TYPE_CHAR:
      rtype = &ffi_type_schar;
      break;
    case RBX_FFI_TYPE_UCHAR:
    case RBX_FFI_TYPE_BOOL:
      rtype = &ffi_type_uchar;
      break;
    case RBX_FFI_TYPE_SHORT:
      rtype = &ffi_type_sshort;
      break;
    case RBX_FFI_TYPE_USHORT:
      rtype = &ffi_type_ushort;
      break;
    case RBX_FFI_TYPE_INT:
      rtype = &ffi_type_sint;
      break;
    case RBX_FFI_TYPE_UINT:
      rtype = &ffi_type_uint;
      break;
    case RBX_FFI_TYPE_LONG:
      rtype = &ffi_type_slong;
      break;
    case RBX_FFI_TYPE_ULONG:
      rtype = &ffi_type_ulong;
      break;
    case RBX_FFI_TYPE_FLOAT:
      rtype = &ffi_type_float;
      break;
    case RBX_FFI_TYPE_DOUBLE:
      rtype = &ffi_type_double;
      break;
    case RBX_FFI_TYPE_LONG_LONG:
      rtype = &ffi_type_sint64;
      break;
    case RBX_FFI_TYPE_ULONG_LONG:
      rtype = &ffi_type_uint64;
      break;
    case RBX_FFI_TYPE_OBJECT:
    case RBX_FFI_TYPE_PTR:
    case RBX_FFI_TYPE_STRING:
    case RBX_FFI_TYPE_STATE:
    case RBX_FFI_TYPE_STRPTR:
      rtype = &ffi_type_pointer;
      break;
    default:
    case RBX_FFI_TYPE_VOID:
      rtype = &ffi_type_void;
      break;
    }

    int* out_arg_types = ALLOC_N(int, arg_count);
    memcpy(out_arg_types, arg_types, sizeof(int) * arg_count);
    NativeFunction** out_callbacks = ALLOC_N(NativeFunction*, arg_count);
    memcpy(out_callbacks, callbacks, sizeof(NativeFunction*) * arg_count);

    FFIData* data = FFIData::create(this, arg_count, out_arg_types, ret_type, out_callbacks);

    status = ffi_prep_cif(&data->cif, FFI_DEFAULT_ABI, arg_count, rtype, types);

    if(status != FFI_OK) {
      XFREE(out_arg_types);
      XFREE(out_callbacks);
      if(status != FFI_OK) rubinius::bug("ffi_prep_cif failed");
    }

    state->shared().om->add_code_resource(data);
    this->ffi_data = data;
  }

  /* The main interface function, handles looking up the pointer in the library,
   * generating the stub, wrapping it up and attaching it to the module.
   */
  NativeFunction* NativeFunction::generate(STATE, Pointer* ptr, Symbol* name,
                                           Array* args, Object* ret)
  {
    int* arg_types;
    int ret_type;
    int i, tot, arg_count;
    Object* type;
    NativeFunction** callbacks;

    tot = args->size();
    arg_count = tot;

    NativeFunction* cb = 0;

    if(tot > 0) {
      arg_types = ALLOC_N(int, tot);
      callbacks = ALLOC_N(NativeFunction*, tot);

      for(i = 0; i < tot; i++) {
        type = args->get(state, i);
        if((cb = try_as<NativeFunction>(type))) {
          arg_types[i] = RBX_FFI_TYPE_CALLBACK;
          callbacks[i] = cb;
        } else {
          if(!type->fixnum_p()) {
            XFREE(arg_types);
            XFREE(callbacks);
            return (NativeFunction*)Primitives::failure();
          }
          arg_types[i] = as<Integer>(type)->to_native();
          callbacks[i] = NULL;

          /* State can only be passed as the first arg, and it's invisible,
             ie doesn't get seen as in onbound arg by ruby. But it can ONLY
             be the first arg. */
          if(arg_types[i] == RBX_FFI_TYPE_STATE) {
            if(i == 0) {
              arg_count--;
            } else {
              XFREE(arg_types);
              XFREE(callbacks);
              return (NativeFunction*)Primitives::failure();
            }
          }
        }
      }
    } else {
      arg_types = NULL;
      callbacks = NULL;
    }

    ret_type = as<Integer>(ret)->to_native();

    NativeFunction* func = NativeFunction::create(state, name, arg_count);
    func->prep(state, tot, arg_types, ret_type, callbacks);
    func->ffi_data->ep = ptr->pointer;

    if(arg_types) XFREE(arg_types);
    if(callbacks) XFREE(callbacks);

    return func;
  }

  static void invoke_callback(ffi_cif* cif, void* retval,
                              void** parameters, void* user_data) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    FFIData* stub =
      reinterpret_cast<FFIData*>(user_data);

    GCTokenImpl gct;
    State* state = env->state();

    state->gc_dependent();

    Array* args = Array::create(state, stub->arg_count);

    for(size_t i = 0; i < stub->arg_count; i++) {
      switch(stub->arg_types[i]) {
      case RBX_FFI_TYPE_CHAR:
        args->set(state, i, Fixnum::from(*(int8_t*)parameters[i]));
        break;
      case RBX_FFI_TYPE_UCHAR:
        args->set(state, i, Fixnum::from(*(uint8_t*)parameters[i]));
        break;
      case RBX_FFI_TYPE_BOOL:
        args->set(state, i, (*(uint8_t*)parameters[i]) ? cTrue : cFalse);
        break;
      case RBX_FFI_TYPE_SHORT:
        args->set(state, i, Fixnum::from(*(int16_t*)parameters[i]));
        break;
      case RBX_FFI_TYPE_USHORT:
        args->set(state, i, Fixnum::from(*(uint16_t*)parameters[i]));
        break;
      case RBX_FFI_TYPE_INT:
        args->set(state, i, Fixnum::from(*(int*)parameters[i]));
        break;
      case RBX_FFI_TYPE_UINT:
        args->set(state, i, Fixnum::from(*(unsigned int*)parameters[i]));
        break;
      case RBX_FFI_TYPE_LONG:
        args->set(state, i, Fixnum::from(*(long*)parameters[i]));
        break;
      case RBX_FFI_TYPE_ULONG:
        args->set(state, i, Fixnum::from(*(unsigned long*)parameters[i]));
        break;
      case RBX_FFI_TYPE_FLOAT:
        args->set(state, i, Float::create(state, *(float*)parameters[i]));
        break;
      case RBX_FFI_TYPE_DOUBLE:
        args->set(state, i, Float::create(state, *(double*)parameters[i]));
        break;
      case RBX_FFI_TYPE_LONG_LONG:
        args->set(state, i, Fixnum::from(*(long long*)parameters[i]));
        break;
      case RBX_FFI_TYPE_ULONG_LONG:
        args->set(state, i, Fixnum::from(*(unsigned long long*)parameters[i]));
        break;
      case RBX_FFI_TYPE_OBJECT:
        args->set(state, i, *(Object**)parameters[i]);
        break;
      case RBX_FFI_TYPE_PTR: {
        void* ptr = *(void**)parameters[i];
        args->set(state, i, Pointer::create(state,
              ptr ? Pointer::create(state, ptr) : cNil));
        break;
      }
      case RBX_FFI_TYPE_STRING: {
        char* ptr = *(char**)parameters[i];
        Object* obj = cNil;
        if(ptr) {
          obj = String::create(state, ptr);
          obj->taint(state);
        }

        args->set(state, i, obj);
        break;
      }
      case RBX_FFI_TYPE_STRPTR: {
        char* result = *(char**)parameters[i];
        Object* s;
        Object* p;

        if(result == NULL) {
          s = p = cNil;
        } else {
          s = String::create(state, result);
          s->taint(state);
          p = Pointer::create(state, result);
        }

        Array* ary = Array::create(state, 2);
        ary->set(state, 0, s);
        ary->set(state, 1, p);

        args->set(state, i, ary);
        break;
      }
      default:
      case RBX_FFI_TYPE_VOID:
        args->set(state, i, cNil);
        break;
      }
    }

    Object* obj = stub->callable->send(state, env->current_call_frame(),
                                       state->symbol("call"),
                                       args, cNil, true);

    // Ug. An exception is being raised...
    if(!obj) {
      // For now, print out a warning to stderr and just eat it.
      std::cerr << "[WARNING] Exception raised by callback, ignoring.\n";
      state->vm()->thread_state()->clear();
      obj = cNil;
    }

    switch(stub->ret_type) {
    case RBX_FFI_TYPE_CHAR:
    case RBX_FFI_TYPE_SHORT:
    case RBX_FFI_TYPE_INT:
    case RBX_FFI_TYPE_LONG:
      if(Integer* io = try_as<Integer>(obj)) {
        *((ffi_sarg*)retval) = io->to_native();
      } else {
        *((ffi_sarg*)retval) = 0;
      }
      break;
    case RBX_FFI_TYPE_UCHAR:
    case RBX_FFI_TYPE_USHORT:
    case RBX_FFI_TYPE_UINT:
    case RBX_FFI_TYPE_ULONG:
      if(Integer* io = try_as<Integer>(obj)) {
        *((ffi_arg*)retval) = io->to_native();
      } else {
        *((ffi_arg*)retval) = 0;
      }
      break;
    case RBX_FFI_TYPE_BOOL:
      if(CBOOL(obj)) {
        *((ffi_arg*)retval) = 1;
      } else {
        *((ffi_arg*)retval) = 0;
      }
      break;
    case RBX_FFI_TYPE_FLOAT:
      if(Float* flt = try_as<Float>(obj)) {
        *((float*)retval) = flt->val;
      } else {
        *((float*)retval) = 0.0;
      }
      break;
    case RBX_FFI_TYPE_DOUBLE:
      if(Float* flt = try_as<Float>(obj)) {
        *((double*)retval) = flt->val;
      } else {
        *((double*)retval) = 0.0;
      }
      break;
    case RBX_FFI_TYPE_LONG_LONG:
      if(Integer* io = try_as<Integer>(obj)) {
        *((ffi_sarg*)retval) = io->to_long_long();
      } else {
        *((ffi_sarg*)retval) = 0;
      }
      break;
    case RBX_FFI_TYPE_ULONG_LONG:
      if(Integer* io = try_as<Integer>(obj)) {
        *((ffi_arg*)retval) = io->to_ulong_long();
      } else {
        *((ffi_arg*)retval) = 0;
      }
      break;
    case RBX_FFI_TYPE_OBJECT:
      *((Object**)retval) = obj;
      break;
    case RBX_FFI_TYPE_PTR:
      if(Pointer* ptr = try_as<Pointer>(obj)) {
        *((void**)retval) = ptr->pointer;
      } else {
        *((void**)retval) = 0;
      }
      break;
    default:
    case RBX_FFI_TYPE_VOID:
      *((ffi_arg*)retval) = 0;
      break;
    }

    state->gc_independent(gct);
  }


  Array* NativeFunction::generate_tramp(STATE, Object* obj, Symbol* name,
                                        Array* args, Object* ret)
  {
    int* arg_types;
    int ret_type;
    int i, tot;
    Object* type;
    NativeFunction** callbacks;

    tot = args->size();
    NativeFunction* cb = 0;

    if(tot > 0) {
      arg_types = ALLOC_N(int, tot);
      callbacks = ALLOC_N(NativeFunction*, tot);

      for(i = 0; i < tot; i++) {
        type = args->get(state, i);
        if((cb = try_as<NativeFunction>(type))) {
          arg_types[i] = RBX_FFI_TYPE_CALLBACK;
          callbacks[i] = cb;
        } else {
          if(!type->fixnum_p()) {
            XFREE(arg_types);
            XFREE(callbacks);
            return nil<Array>();
          }
          arg_types[i] = as<Integer>(type)->to_native();
          callbacks[i] = NULL;

          /* State can not be passed. */
          if(arg_types[i] == RBX_FFI_TYPE_STATE) {
            XFREE(arg_types);
            XFREE(callbacks);
            return nil<Array>();
          }
        }
      }
    } else {
      arg_types = NULL;
      callbacks = NULL;
    }

    ret_type = as<Integer>(ret)->to_native();

    NativeFunction* func = NativeFunction::create(state, name, tot);
    func->prep(state, tot, arg_types, ret_type, callbacks);

    func->ffi_data->callable = obj;

    int status = ffi_prep_closure_loc(func->ffi_data->closure, &func->ffi_data->cif,
        invoke_callback, func->ffi_data, func->ffi_data->ep);

    if(arg_types) XFREE(arg_types);
    if(callbacks) XFREE(callbacks);
    if(status != FFI_OK) {
      return force_as<Array>(Primitives::failure());
    }

    Array* ary = Array::create(state, 2);
    ary->set(state, 0, func);
    ary->set(state, 1, Pointer::create(state, func->ffi_data->ep));

    return ary;
  }

  Pointer* NativeFunction::adjust_tramp(STATE, Object* obj, NativeFunction* orig) {
    Symbol* ffi_tramp = state->symbol("ffi_tramp");

    Object* existing = obj->get_ivar(state, ffi_tramp);
    if(NativeFunction* f = try_as<NativeFunction>(existing)) {
      return Pointer::create(state, f->ffi_data->ep);
    }

    NativeFunction* func = NativeFunction::create(state, ffi_tramp,
                             orig->ffi_data->arg_count);

    func->prep(state, orig->ffi_data->arg_count, orig->ffi_data->arg_types,
               orig->ffi_data->ret_type, orig->ffi_data->callbacks);

    func->ffi_data->callable = obj;

    int status = ffi_prep_closure_loc(func->ffi_data->closure, &func->ffi_data->cif,
        invoke_callback, func->ffi_data, func->ffi_data->ep);

    if(status != FFI_OK) {
      return nil<Pointer>();
    }

    obj->set_ivar(state, ffi_tramp, func);

    return Pointer::create(state, func->ffi_data->ep);
  }

  Object* NativeFunction::call(STATE, Arguments& args, CallFrame* call_frame)
  {
    Object* ret;
    Object* obj;
    GCTokenImpl gct;

    bool use_cb_block = false;

    if(args.total() != ffi_data->arg_count) {
      if(args.total() + 1 == ffi_data->arg_count &&
          args.block() && !args.block()->nil_p())
      {
        use_cb_block = true;
      } else {
        Exception* exc =
          Exception::make_argument_error(state, ffi_data->arg_count,
                                         args.total(), args.name());
        exc->locations(state, Location::from_call_stack(state, call_frame));
        state->raise_exception(exc);

        return NULL;
      }
    }

    // Because we call back into ruby to do conversions.
    RootBuffer vrf(state->vm()->root_buffers(), args.arguments(), args.total());

    void** values = ALLOCA_N(void*, ffi_data->arg_count);
    void** heap_allocations = ALLOCA_N(void*, ffi_data->arg_count);
    size_t i;

    for(i = 0; i < ffi_data->arg_count; i++) {
      heap_allocations[i] = NULL;

      switch(ffi_data->arg_types[i]) {
      case RBX_FFI_TYPE_CHAR: {
        char* tmp = ALLOCA(char);
        obj = args.get_argument(i);
        type_assert(state, obj, FixnumType, "converting to char");
        *tmp = (char)as<Fixnum>(obj)->to_native();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_UCHAR: {
        unsigned char* tmp = ALLOCA(unsigned char);
        obj = args.get_argument(i);
        type_assert(state, obj, FixnumType, "converting to uchar");
        *tmp = (unsigned char)as<Fixnum>(obj)->to_native();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_BOOL: {
        unsigned char* tmp = ALLOCA(unsigned char);
        obj = args.get_argument(i);
        *tmp = (unsigned char)CBOOL(obj);
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_SHORT: {
        short* tmp = ALLOCA(short);
        obj = args.get_argument(i);
        type_assert(state, obj, FixnumType, "converting to short");
        *tmp = (short)as<Fixnum>(obj)->to_native();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_USHORT: {
        unsigned short* tmp = ALLOCA(unsigned short);
        obj = args.get_argument(i);
        type_assert(state, obj, FixnumType, "converting to ushort");
        *tmp = (unsigned short)as<Fixnum>(obj)->to_native();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_INT: {
        int* tmp = ALLOCA(int);
        obj = args.get_argument(i);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_int();
        } else {
          type_assert(state, obj, BignumType, "converting to int");
          *tmp = as<Bignum>(obj)->to_int();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_UINT: {
        unsigned int* tmp = ALLOCA(unsigned int);
        obj = args.get_argument(i);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_uint();
        } else {
          type_assert(state, obj, BignumType, "converting to unsigned int");
          *tmp = as<Bignum>(obj)->to_uint();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_LONG: {
        long* tmp = ALLOCA(long);
        obj = args.get_argument(i);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_long();
        } else {
          type_assert(state, obj, BignumType, "converting to long");
          *tmp = as<Bignum>(obj)->to_long();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_ULONG: {
        unsigned long* tmp = ALLOCA(unsigned long);
        obj = args.get_argument(i);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_ulong();
        } else {
          type_assert(state, obj, BignumType, "converting to unsigned long");
          *tmp = as<Bignum>(obj)->to_ulong();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_FLOAT: {
        float* tmp = ALLOCA(float);
        obj = args.get_argument(i);
        type_assert(state, obj, FloatType, "converting to float");
        *tmp = (float)as<Float>(obj)->to_double(state);
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_DOUBLE: {
        double* tmp = ALLOCA(double);
        obj = args.get_argument(i);
        type_assert(state, obj, FloatType, "converting to double");
        *tmp = as<Float>(obj)->to_double(state);
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_LONG_LONG: {
        long long* tmp = ALLOCA(long long);
        obj = args.get_argument(i);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_long_long();
        } else {
          type_assert(state, obj, BignumType, "converting to long long");
          *tmp = as<Bignum>(obj)->to_long_long();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_ULONG_LONG: {
        unsigned long long* tmp = ALLOCA(unsigned long long);
        obj = args.get_argument(i);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_ulong_long();
        } else {
          type_assert(state, obj, BignumType, "converting to unsigned long long");
          *tmp = as<Bignum>(obj)->to_ulong_long();
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_STATE: {
        VM** tmp = ALLOCA(VM*);
        *tmp = state->vm();
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_OBJECT: {
        Object** tmp = ALLOCA(Object*);
        obj = args.get_argument(i);
        *tmp = obj;
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_PTR: {
        void** tmp = ALLOCA(void*);
        obj = args.get_argument(i);
        if(obj->nil_p()) {
          *tmp = NULL;
        } else {
          Pointer* mp = try_as<Pointer>(obj);
          if(!mp) {
            if(String* so = try_as<String>(obj)) {
              int size;
              size = so->byte_size();

              char* data = ALLOCA_N(char, size + 1);
              memcpy(data, so->c_str(state), size);
              data[size] = 0;
              *tmp = data;
            } else if(CBOOL(obj->respond_to(state, state->symbol("to_ptr"), cTrue))) {
              Object* o2 = obj->send(state, call_frame, state->symbol("to_ptr"));
              type_assert(state, o2, PointerType, "converting to pointer");
              mp = as<Pointer>(o2);
              *tmp = mp->pointer;
            } else {
              type_assert(state, obj, PointerType, "converting to pointer");
            }
          } else {
            *tmp = mp->pointer;
          }
        }
        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_CALLBACK: {
        void** tmp = ALLOCA(void*);
        if(use_cb_block) {
          obj = args.block();
        } else {
          obj = args.get_argument(i);
        }

        if(obj->reference_p()) {
          Pointer* ptr = NativeFunction::adjust_tramp(state, obj, ffi_data->callbacks[i]);
          *tmp = ptr->pointer;
        } else {
          *tmp = NULL;
        }

        values[i] = tmp;
        break;
      }
      case RBX_FFI_TYPE_STRING: {
        char** tmp = ALLOCA(char*);
        obj = args.get_argument(i);

        if(obj->nil_p()) {
          *tmp = NULL;
        } else {
          int size;
          String* so;

          so = as<String>(obj);
          size = so->byte_size();

          char* data = (char *)malloc(sizeof(char) * (size + 1));

          if (!data) {
            rubinius::bug("could not allocate memory for string");
          }

          memcpy(data, so->c_str(state), size);
          data[size] = 0;

          heap_allocations[i] = data;
          *tmp = data;
        }
        values[i] = tmp;
        break;
      }
      }
    }

    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    CallFrame* saved_frame = env->current_call_frame();
    env->set_current_call_frame(call_frame);

    state->set_call_frame(call_frame);

    // Make sure we have this on the stack
    NativeFunction* self = this;
    OnStack<1> on(state, self);

    // We need to have ffi_data on the stack since
    // a GC might move this and there ffi_data will
    // point at the wrong place in memory
    FFIData* ffi_data_local = ffi_data;
    state->gc_independent(gct);

    switch(ffi_data_local->ret_type) {
    case RBX_FFI_TYPE_CHAR: {
      ffi_arg result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_UCHAR: {
      ffi_arg result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_BOOL: {
      ffi_arg result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = (result != 0) ? cTrue : cFalse;
      break;
    }
    case RBX_FFI_TYPE_SHORT: {
      ffi_arg result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_USHORT: {
      ffi_arg result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_INT: {
      ffi_arg result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Integer::from(state, (native_int)result);
      break;
    }
    case RBX_FFI_TYPE_UINT: {
      ffi_arg result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Integer::from(state, (unsigned int)result);
      break;
    }
    case RBX_FFI_TYPE_LONG: {
      long result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_ULONG: {
      unsigned long result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_FLOAT: {
      float result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Float::create(state, (double)result);
      break;
    }
    case RBX_FFI_TYPE_DOUBLE: {
      double result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Float::create(state, result);
      break;
    }
    case RBX_FFI_TYPE_LONG_LONG: {
      long long result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_ULONG_LONG: {
      unsigned long long result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_OBJECT: {
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &ret, values);
      state->gc_dependent();
      break;
    }
    case RBX_FFI_TYPE_PTR: {
      void* result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      if(result == NULL) {
        ret = cNil;
      } else {
        ret = Pointer::create(state, result);
      }
      break;
    }
    case RBX_FFI_TYPE_STRING: {
      char* result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      if(result == NULL) {
        ret = cNil;
      } else {
        ret = String::create(state, result);
        ret->taint(state);
      }
      break;
    }
    case RBX_FFI_TYPE_STRPTR: {
      char* result;
      Object* s;
      Object* p;

      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();

      if(result == NULL) {
        s = p = cNil;
      } else {
        s = String::create(state, result);
        s->taint(state);
        p = Pointer::create(state, result);
      }

      Array* ary = Array::create(state, 2);
      ary->set(state, 0, s);
      ary->set(state, 1, p);
      ret = ary;
      break;
    }
    default:
    case RBX_FFI_TYPE_VOID: {
      ffi_arg result;
      ffi_call(&ffi_data_local->cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent();
      ret = cNil;
      break;
    }
    }

    env->set_current_call_frame(saved_frame);

    for(i = 0; i < ffi_data_local->arg_count; i++) {
      if (heap_allocations[i]) {
        free(heap_allocations[i]);
      }
    }

    return ret;
  }

  void NativeFunction::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);
    mark_inliners(obj, mark);

    NativeFunction* func = force_as<NativeFunction>(obj);

    if(func->ffi_data) {
      func->ffi_data->set_mark();

      if(func->ffi_data->callable) {
        Object* tmp = mark.call(func->ffi_data->callable);
        if(tmp) {
          func->ffi_data->callable = tmp;
          mark.just_set(obj, tmp);
        }
      }
      for(size_t i = 0; i<func->ffi_data->arg_count; i++) {
        if(func->ffi_data->callbacks[i]) {
          Object* tmp = mark.call(func->ffi_data->callbacks[i]);
          if(tmp) {
            func->ffi_data->callbacks[i] = force_as<NativeFunction>(tmp);
            mark.just_set(obj, tmp);
          }
        }
      }

      func->ffi_data->function = func;
    }
  }

}

#include "arguments.hpp"
#include "builtin/array.hpp"
#include "builtin/class.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/ffi_pointer.hpp"
#include "builtin/float.hpp"
#include "builtin/location.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/nativefunction.hpp"
#include "builtin/nativemethod.hpp"
#include "builtin/proc.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/system.hpp"
#include "call_frame.hpp"
#include "dtrace/dtrace.h"
#include "ffi_util.hpp"
#include "instruments/tooling.hpp"
#include "object_utils.hpp"
#include "objectmemory.hpp"
#include "on_stack.hpp"
#include "ontology.hpp"

namespace rubinius {

  void NativeFunction::init(STATE) {
    GO(native_function).set(ontology::new_class(state,
          "NativeFunction", G(executable), G(rubinius)));
    G(native_function)->set_object_type(state, NativeFunctionType);
  }

  FFIData* FFIData::create(STATE, NativeFunction* func, int count, FFIArgInfo* args,
                           FFIArgInfo* ret)
  {
    void* ep;

    FFIData* data = new FFIData(state, func, count, args, ret);
    data->closure = reinterpret_cast<ffi_closure*>(
                      ffi_closure_alloc(sizeof(ffi_closure), &ep));

    data->ep = ep;

    return data;
  }

  FFIData::FFIData(STATE, NativeFunction* func,  int count, FFIArgInfo* args, FFIArgInfo* ret)
    : closure(0)
    , shared(&state->shared())
    , callable(0)
    , function(func)
    , args_info(args)
    , ret_info(*ret)
    , arg_count(count)
  {}

  FFIData::~FFIData() {
    if(args_info) XFREE(args_info);
    if(cif.arg_types) XFREE(cif.arg_types);
    if(closure) ffi_closure_free(closure);
  }

  /* Run when a NativeFunction is executed.  Executes the related C function.
   */
  Object* NativeFunction::execute(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                                  Arguments& args) {
    NativeFunction* nfunc = as<NativeFunction>(exec);

    state->set_call_frame(call_frame);

    try {
      OnStack<2> os(state, exec, mod);
#ifdef RBX_PROFILER
      if(unlikely(state->vm()->tooling())) {
        tooling::MethodEntry method(state, exec, mod, args);
        RUBINIUS_METHOD_FFI_ENTRY_HOOK(state, mod, args.name(), call_frame);
        Object* ret = nfunc->call(state, args, call_frame);
        RUBINIUS_METHOD_FFI_RETURN_HOOK(state, mod, args.name(), call_frame);
        return ret;
      } else {
        RUBINIUS_METHOD_FFI_ENTRY_HOOK(state, mod, args.name(), call_frame);
        Object* ret = nfunc->call(state, args, call_frame);
        RUBINIUS_METHOD_FFI_RETURN_HOOK(state, mod, args.name(), call_frame);
        return ret;
      }
#else
      RUBINIUS_METHOD_FFI_ENTRY_HOOK(state, mod, args.name(), call_frame);
      Object* ret = nfunc->call(state, args, call_frame);
      RUBINIUS_METHOD_FFI_RETURN_HOOK(state, mod, args.name(), call_frame);
      return ret;
#endif

    } catch(TypeError &e) {
      Exception* exc =
        Exception::make_type_error(state, e.type, e.object, e.reason);
      exc->locations(state, Location::from_call_stack(state, call_frame));

      state->raise_exception(exc);
      RUBINIUS_METHOD_FFI_RETURN_HOOK(state, mod, args.name(), call_frame);
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
      case RBX_FFI_TYPE_ENUM:
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
      case RBX_FFI_TYPE_STRPTR:
        return sizeof(void*);

      default:
        rubinius::bug("Unsupported FFI type");
        return 0; // Keep GCC happy
    }
  }

  Fixnum* NativeFunction::type_size_prim(STATE, Fixnum* type) {
    return Fixnum::from(NativeFunction::type_size(type->to_native()));
  }

  NativeFunction* NativeFunction::create(STATE, Symbol* name, int args) {
    NativeFunction* nf = state->new_object<NativeFunction>(G(native_function));
    nf->primitive(state, state->symbol("nativefunction_call"));
    nf->required(state, Fixnum::from(args));
    nf->varargs(state, cFalse);
    nf->serial(state, Fixnum::from(0));
    nf->name(state, name);
    nf->file(state, state->symbol("<system>"));

    nf->set_executor(NativeFunction::execute);
    nf->inliners_ = 0;
    nf->prim_index_ = -1;
    nf->custom_call_site_ = false;
    nf->ffi_data = 0;

    return nf;
  }

  bool NativeFunction::ffi_arg_info(STATE, Object* type, FFIArgInfo* args_info) {
    if(type->fixnum_p()) {
      args_info->type = as<Fixnum>(type)->to_int();
      args_info->enum_obj = NULL;
      args_info->callback = NULL;
      return true;
    } else if(type->symbol_p()) {
      LookupTable* tbl = try_as<LookupTable>(G(ffi)->get_const(state, "TypeDefs"));
      if(!tbl) return false;
      Fixnum* fix = try_as<Fixnum>(tbl->aref(state, type));
      if(!fix) return false;
      args_info->type = fix->to_int();
      args_info->enum_obj = NULL;
      args_info->callback = NULL;
      return true;
    } else if(NativeFunction* cb = try_as<NativeFunction>(type)) {
      args_info->type = RBX_FFI_TYPE_CALLBACK;
      args_info->enum_obj = NULL;
      args_info->callback = cb;
      return true;
    } else if(CBOOL(type->respond_to(state, state->symbol("[]"), cTrue))) {
      args_info->type = RBX_FFI_TYPE_ENUM;
      args_info->enum_obj = type;
      args_info->callback = NULL;
      return true;
    }
    return false;
  }

  ffi_type* NativeFunction::ffi_type_info(FFIArgInfo* info) {
    switch(info->type) {
    case RBX_FFI_TYPE_CHAR:
      return &ffi_type_schar;
    case RBX_FFI_TYPE_UCHAR:
    case RBX_FFI_TYPE_BOOL:
      return &ffi_type_uchar;
    case RBX_FFI_TYPE_SHORT:
      return &ffi_type_sshort;
    case RBX_FFI_TYPE_USHORT:
      return &ffi_type_ushort;
    case RBX_FFI_TYPE_INT:
    case RBX_FFI_TYPE_ENUM:
      return &ffi_type_sint;
    case RBX_FFI_TYPE_UINT:
      return &ffi_type_uint;
    case RBX_FFI_TYPE_LONG:
      return &ffi_type_slong;
    case RBX_FFI_TYPE_ULONG:
      return &ffi_type_ulong;
    case RBX_FFI_TYPE_FLOAT:
      return &ffi_type_float;
    case RBX_FFI_TYPE_DOUBLE:
      return &ffi_type_double;
    case RBX_FFI_TYPE_LONG_LONG:
      return &ffi_type_sint64;
    case RBX_FFI_TYPE_ULONG_LONG:
      return &ffi_type_uint64;
    case RBX_FFI_TYPE_PTR:
    case RBX_FFI_TYPE_CALLBACK:
    case RBX_FFI_TYPE_STRING:
    case RBX_FFI_TYPE_STRPTR:
      return &ffi_type_pointer;
    case RBX_FFI_TYPE_VOID:
    default:
      return &ffi_type_void;
    }
    return 0;
  }

  void NativeFunction::prep(STATE, int arg_count, FFIArgInfo* args,
                            FFIArgInfo* ret) {

    ffi_status status;
    ffi_type* rtype;
    ffi_type** types = 0;
    FFIArgInfo* out_args_info = 0;

    if(arg_count > 0) {
      types = ALLOC_N(ffi_type*, arg_count);

      for(int i = 0; i < arg_count; i++) {
        types[i] = ffi_type_info(&args[i]);
      }

      out_args_info = ALLOC_N(FFIArgInfo, arg_count);
      memcpy(out_args_info, args, sizeof(FFIArgInfo) * arg_count);
    }

    rtype = ffi_type_info(ret);

    FFIData* data = FFIData::create(state, this, arg_count, out_args_info, ret);

    status = ffi_prep_cif(&data->cif, FFI_DEFAULT_ABI, arg_count, rtype, types);

    if(status != FFI_OK) {
      delete data;
      rubinius::bug("ffi_prep_cif failed");
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
    int arg_count;
    FFIArgInfo ret_info;
    FFIArgInfo* args_info;

    bool varargs = false;
    arg_count = args->size();

    if(arg_count > 0) {
      args_info = ALLOC_N(FFIArgInfo, arg_count);

      for(int i = 0; i < arg_count; i++) {
        Object* type = args->get(state, i);
        if(!ffi_arg_info(state, type, &(args_info[i]))) {
          XFREE(args_info);
          return (NativeFunction*)Primitives::failure();
        }
        if(args_info[i].type == RBX_FFI_TYPE_VARARGS) {
          if(i == arg_count - 1) {
            arg_count--;
            varargs = true;
          } else {
            XFREE(args_info);
            Exception::argument_error(state, "Only last argument can be varargs");
          }
        }
      }
    } else {
      args_info = NULL;
    }

    if(!ffi_arg_info(state, ret, &ret_info)) {
      XFREE(args_info);
      return (NativeFunction*)Primitives::failure();
    }

    if(ret_info.type == RBX_FFI_TYPE_VARARGS) {
      if(args_info) XFREE(args_info);
      Exception::argument_error(state, "Return value can't be of varargs type");
    }

    NativeFunction* func = NativeFunction::create(state, name, arg_count);
    if(varargs) func->varargs(state, cTrue);
    func->prep(state, arg_count, args_info, &ret_info);
    func->ffi_data->ep = ptr->pointer;

    if(args_info) XFREE(args_info);

    return func;
  }

  static void invoke_callback(ffi_cif* cif, void* retval,
                              void** parameters, void* user_data) {
    NativeMethodEnvironment* env = NativeMethodEnvironment::get();
    FFIData* stub =
      reinterpret_cast<FFIData*>(user_data);

    GCTokenImpl gct;
    VM* vm = 0;

    int calculate_stack = 0;
    if(!env) {
      // Apparently we're running in a new thread here, setup
      // everything we need here.
      vm = stub->shared->new_vm();
      // Detect the stack size and set it up in the VM object
      size_t stack_size;
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_attr_getstacksize (&attr, &stack_size);
      vm->set_root_stack(reinterpret_cast<uintptr_t>(&calculate_stack), stack_size);

      // Setup nativemethod handles into thread local
      State state(vm);
      NativeMethod::init_thread(&state);
      env = NativeMethodEnvironment::get();
    }

    State* state = env->state();

    state->gc_dependent(gct, state->vm()->saved_call_frame());

    Array* args = Array::create(state, stub->arg_count);
    OnStack<1> os(state, args);

    for(size_t i = 0; i < stub->arg_count; i++) {
      switch(stub->args_info[i].type) {
      case RBX_FFI_TYPE_CHAR:
        args->set(state, i, Fixnum::from(*(int8_t*)parameters[i]));
        break;
      case RBX_FFI_TYPE_UCHAR:
        args->set(state, i, Fixnum::from(*(uint8_t*)parameters[i]));
        break;
      case RBX_FFI_TYPE_BOOL:
        args->set(state, i, RBOOL(*(uint8_t*)parameters[i]));
        break;
      case RBX_FFI_TYPE_SHORT:
        args->set(state, i, Fixnum::from(*(int16_t*)parameters[i]));
        break;
      case RBX_FFI_TYPE_USHORT:
        args->set(state, i, Fixnum::from(*(uint16_t*)parameters[i]));
        break;
      case RBX_FFI_TYPE_INT:
        args->set(state, i, Integer::from(state, *(int*)parameters[i]));
        break;
      case RBX_FFI_TYPE_UINT:
        args->set(state, i, Integer::from(state, *(unsigned int*)parameters[i]));
        break;
      case RBX_FFI_TYPE_LONG:
        args->set(state, i, Integer::from(state, *(long*)parameters[i]));
        break;
      case RBX_FFI_TYPE_ULONG:
        args->set(state, i, Integer::from(state, *(unsigned long*)parameters[i]));
        break;
      case RBX_FFI_TYPE_FLOAT:
        args->set(state, i, Float::create(state, *(float*)parameters[i]));
        break;
      case RBX_FFI_TYPE_DOUBLE:
        args->set(state, i, Float::create(state, *(double*)parameters[i]));
        break;
      case RBX_FFI_TYPE_LONG_LONG:
        args->set(state, i, Integer::from(state, *(long long*)parameters[i]));
        break;
      case RBX_FFI_TYPE_ULONG_LONG:
        args->set(state, i, Integer::from(state, *(unsigned long long*)parameters[i]));
        break;
      case RBX_FFI_TYPE_ENUM:{
        Array* ary = Array::create(state, 1);
        ary->set(state, 0, Fixnum::from(*(int*)parameters[i]));
        Object* result = stub->args_info[i].enum_obj->send(state, env->current_call_frame(), state->symbol("symbol"), ary);
        if(!result) {
          std::cerr << "[WARNING] Exception raised by callback, ignoring.\n";
          state->vm()->thread_state()->clear();
          result = cNil;
        }
        args->set(state, i, result);
        break;
      }
      case RBX_FFI_TYPE_CALLBACK: {
        void* ptr = *(void**)parameters[i];
        if(ptr) {
          NativeFunction* orig = stub->args_info[i].callback;
          NativeFunction* func = NativeFunction::create(state, state->symbol("ffi_tramp"), orig->ffi_data->arg_count);
          func->prep(state, orig->ffi_data->arg_count, orig->ffi_data->args_info, &orig->ffi_data->ret_info);
          func->ffi_data->ep = ptr;
          Proc* prc = Proc::create(state, state->vm()->shared.globals.proc.get());
          prc->bound_method(state, func);
          args->set(state, i, prc);
        } else {
          args->set(state, i, cNil);
        }
        break;
      }
      case RBX_FFI_TYPE_PTR: {
        void* ptr = *(void**)parameters[i];
        if(ptr) {
          args->set(state, i, Pointer::create(state, ptr));
        } else {
          args->set(state, i, cNil);
        }
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
                                       G(sym_call), args);

    // Ug. An exception is being raised...
    if(!obj) {
      // For now, print out a warning to stderr and just eat it.
      std::cerr << "[WARNING] Exception raised by callback, ignoring.\n";
      state->vm()->thread_state()->clear();
      obj = cNil;
    }

    switch(stub->ret_info.type) {
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
    case RBX_FFI_TYPE_PTR:
      if(Pointer* ptr = try_as<Pointer>(obj)) {
        *((void**)retval) = ptr->pointer;
      } else {
        *((void**)retval) = 0;
      }
      break;
    case RBX_FFI_TYPE_ENUM: {
      Array* ary = Array::create(state, 1);
      ary->set(state, 0, obj);
      Object* value = stub->ret_info.enum_obj->send(state, env->current_call_frame(), state->symbol("[]"), ary);
      if(!value) {
        // For now, print out a warning to stderr and just eat it.
        std::cerr << "[WARNING] Exception raised by callback, ignoring.\n";
        state->vm()->thread_state()->clear();
        value = cNil;
      }
      if(value->nil_p()) {
        *((Object**)retval) = value;
      } else {
        *((ffi_sarg*)retval) = as<Integer>(value)->to_native();
      }
      break;
    }
    case RBX_FFI_TYPE_CALLBACK: {
      Pointer* ptr = NativeFunction::adjust_tramp(state, obj, stub->ret_info.callback);
      *((void**)retval) = ptr->pointer;
      break;
    }
    default:
    case RBX_FFI_TYPE_VOID:
      *((ffi_arg*)retval) = 0;
      break;
    }

    state->gc_independent(gct, env->current_call_frame());
    if(vm) {
      NativeMethod::cleanup_thread(state);
      vm->set_call_frame(0);
      VM::discard(state, vm);
    }
  }


  Array* NativeFunction::generate_tramp(STATE, Object* obj, Symbol* name,
                                        Array* args, Object* ret)
  {
    int tot;
    FFIArgInfo ret_info;
    FFIArgInfo* args_info;

    tot = args->size();

    if(tot > 0) {
      args_info = ALLOC_N(FFIArgInfo, tot);

      for(int i = 0; i < tot; i++) {
        Object* type = args->get(state, i);
        if(!ffi_arg_info(state, type, &(args_info[i]))) {
          XFREE(args_info);
          return nil<Array>();
        }
      }
    } else {
      args_info = NULL;
    }

    if(!ffi_arg_info(state, ret, &ret_info)) {
      if(args_info) XFREE(args_info);
      return nil<Array>();
    }

    NativeFunction* func = NativeFunction::create(state, name, tot);
    func->prep(state, tot, args_info, &ret_info);

    func->ffi_data->callable = obj;

    int status = ffi_prep_closure_loc(func->ffi_data->closure, &func->ffi_data->cif,
        invoke_callback, func->ffi_data, func->ffi_data->ep);

    if(args_info) XFREE(args_info);
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

    func->prep(state, orig->ffi_data->arg_count, orig->ffi_data->args_info,
               &orig->ffi_data->ret_info);

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

    size_t given_args = args.total();

    if(args.block() && !args.block()->nil_p()) {
      given_args++;
      use_cb_block = true;
    }

    if(!CBOOL(varargs()) && given_args != ffi_data->arg_count) {
      Exception* exc =
        Exception::make_argument_error(state, ffi_data->arg_count,
                                         args.total(), args.name());
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->raise_exception(exc);

      return NULL;
    }

    size_t req_count = required()->to_native();
    size_t arg_count = ffi_data->arg_count;

    if(CBOOL(varargs())) {
      if((given_args - req_count) & 1) {
        Exception* exc =
           Exception::make_exception(state, G(exc_arg), "Unbalanced type / value tuples for varargs");
        exc->locations(state, Location::from_call_stack(state, call_frame));
        state->raise_exception(exc);
        return NULL;
      }
      arg_count += (given_args - req_count) >> 1;
    }

    // Because we call back into ruby to do conversions.
    RootBuffer vrf(state->vm()->root_buffers(), args.arguments(), args.total());

    // Make sure we have this on the stack
    NativeFunction* self = this;
    OnStack<1> on(state, self);

    // We need to have ffi_data on the stack since
    // a GC might move this and there ffi_data will
    // point at the wrong place in memory
    FFIData* ffi_data_local = self->ffi_data;

    size_t ffi_index = 0;
    size_t obj_index = 0;

    void* values[arg_count];
    void* heap_allocations[arg_count];
    for(size_t i = 0; i < arg_count; i++) {
      heap_allocations[i] = 0;
    }

    ffi_type** types = NULL;
    ffi_cif* cif = &ffi_data_local->cif;
    ffi_type* rtype = NULL;

    if(CBOOL(varargs())) {
      // If we have a varargs call, we need to copy the data. We can't
      // modify this inline, since if we have multiple threads running
      // the same native function it could corrupt data.
      //
      // Since these are pretty small memory structures, we allocate
      // them on the stack so they are cleaned up automatically and we
      // don't have to allocate heap memory on each call.
      rtype = cif->rtype;
      types = ALLOCA_N(ffi_type*, arg_count);
      for(unsigned i = 0; i < req_count; ++i) {
        types[i] = cif->arg_types[i];
      }
      cif = ALLOCA(ffi_cif);
    }

    for(; ffi_index < arg_count; ffi_index++) {
      FFIArgInfo* arg_info;
      if(ffi_index >= req_count) {
        // This happens when we handle the additional arguments
        // for a varargs call. We need to retrieve the type
        // information for the additional arguments and set those up.
        arg_info = ALLOCA(FFIArgInfo);
        Object* type = args.get_argument(obj_index);
        if(!ffi_arg_info(state, type, arg_info)) {
          // Looks like we couldn't find the type
          std::ostringstream msg;
          msg << "Could not find type: " << type->to_string(state, true);

          Exception* exc =
             Exception::make_exception(state, G(exc_arg), msg.str().c_str());
          exc->locations(state, Location::from_call_stack(state, call_frame));
          state->raise_exception(exc);
          for(size_t i = 0; i < arg_count; i++) {
            if(heap_allocations[i]) {
              XFREE(heap_allocations[i]);
            }
          }
          return NULL;
        }
        types[ffi_index] = ffi_type_info(arg_info);
        obj_index++;
        obj = args.get_argument(obj_index);
        obj_index++;
      } else {
        arg_info = &ffi_data_local->args_info[ffi_index];

        // Handle cases where we don't use an argument from the argument list.
        // This happens when we pass in STATE or use the given block as a
        // callback for the FFI function.
        if(arg_info->type == RBX_FFI_TYPE_CALLBACK && use_cb_block) {
          obj = args.block();
        } else {
          obj = args.get_argument(obj_index);
          obj_index++;
        }
      }

      switch(arg_info->type) {
      case RBX_FFI_TYPE_CHAR: {
        char* tmp = ALLOCA(char);
        type_assert(state, obj, FixnumType, "converting to char");
        *tmp = (char)as<Fixnum>(obj)->to_native();
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_UCHAR: {
        unsigned char* tmp = ALLOCA(unsigned char);
        type_assert(state, obj, FixnumType, "converting to uchar");
        *tmp = (unsigned char)as<Fixnum>(obj)->to_native();
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_BOOL: {
        unsigned char* tmp = ALLOCA(unsigned char);
        *tmp = (unsigned char)CBOOL(obj);
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_SHORT: {
        short* tmp = ALLOCA(short);
        type_assert(state, obj, FixnumType, "converting to short");
        *tmp = (short)as<Fixnum>(obj)->to_native();
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_USHORT: {
        unsigned short* tmp = ALLOCA(unsigned short);
        type_assert(state, obj, FixnumType, "converting to ushort");
        *tmp = (unsigned short)as<Fixnum>(obj)->to_native();
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_INT: {
        int* tmp = ALLOCA(int);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_int();
        } else {
          type_assert(state, obj, BignumType, "converting to int");
          *tmp = as<Bignum>(obj)->to_int();
        }
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_UINT: {
        unsigned int* tmp = ALLOCA(unsigned int);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_uint();
        } else {
          type_assert(state, obj, BignumType, "converting to unsigned int");
          *tmp = as<Bignum>(obj)->to_uint();
        }
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_LONG: {
        long* tmp = ALLOCA(long);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_long();
        } else {
          type_assert(state, obj, BignumType, "converting to long");
          *tmp = as<Bignum>(obj)->to_long();
        }
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_ULONG: {
        unsigned long* tmp = ALLOCA(unsigned long);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_ulong();
        } else {
          type_assert(state, obj, BignumType, "converting to unsigned long");
          *tmp = as<Bignum>(obj)->to_ulong();
        }
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_FLOAT: {
        float* tmp = ALLOCA(float);
        type_assert(state, obj, FloatType, "converting to float");
        *tmp = (float)as<Float>(obj)->to_double(state);
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_DOUBLE: {
        double* tmp = ALLOCA(double);
        type_assert(state, obj, FloatType, "converting to double");
        *tmp = as<Float>(obj)->to_double(state);
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_LONG_LONG: {
        long long* tmp = ALLOCA(long long);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_long_long();
        } else {
          type_assert(state, obj, BignumType, "converting to long long");
          *tmp = as<Bignum>(obj)->to_long_long();
        }
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_ULONG_LONG: {
        unsigned long long* tmp = ALLOCA(unsigned long long);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_ulong_long();
        } else {
          type_assert(state, obj, BignumType, "converting to unsigned long long");
          *tmp = as<Bignum>(obj)->to_ulong_long();
        }
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_PTR: {
        void** tmp = ALLOCA(void*);
        if(obj->nil_p()) {
          *tmp = NULL;
        } else {
          Pointer* mp = try_as<Pointer>(obj);
          if(!mp) {
            if(String* so = try_as<String>(obj)) {
              size_t size;
              size = so->byte_size();

              char data[size + 1];
              memcpy(data, so->byte_address(), size);
              data[size] = 0;

              heap_allocations[ffi_index] = data;
              *tmp = data;
            } else if(CBOOL(obj->respond_to(state, state->symbol("to_ptr"), cTrue))) {
              Object* o2 = obj->send(state, call_frame, state->symbol("to_ptr"));
              if(!o2) {
                for(size_t i = 0; i < arg_count; i++) {
                  if(heap_allocations[i]) {
                    XFREE(heap_allocations[i]);
                  }
                }
                return 0;
              }
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
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_ENUM: {
        int* tmp = ALLOCA(int);
        if(obj->fixnum_p()) {
          *tmp = as<Fixnum>(obj)->to_int();
        } else {
          Array* ary = Array::create(state, 1);
          ary->set(state, 0, obj);
          Object* val = arg_info->enum_obj->send(state, call_frame, state->symbol("[]"), ary);
          if(!val) {
            for(size_t i = 0; i < arg_count; i++) {
              if(heap_allocations[i]) {
                XFREE(heap_allocations[i]);
              }
            }
            return 0;
          }
          if(val->nil_p()) {
            *tmp = 0;
          } else {
            if(val->fixnum_p()) {
              *tmp = as<Fixnum>(val)->to_int();
            } else {
              type_assert(state, val, BignumType, "converting to int");
              *tmp = as<Bignum>(val)->to_int();
            }
          }
        }
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_CALLBACK: {
        void** tmp = ALLOCA(void*);
        if(obj->reference_p()) {
          Pointer* ptr = NativeFunction::adjust_tramp(state, obj, arg_info->callback);
          *tmp = ptr->pointer;
        } else {
          *tmp = NULL;
        }
        values[ffi_index] = tmp;
        break;
      }
      case RBX_FFI_TYPE_STRING: {
        char** tmp = ALLOCA(char*);
        if(obj->nil_p()) {
          *tmp = NULL;
        } else {
          size_t size;
          String* so;

          so = as<String>(obj);
          size = so->byte_size();

          char* data = ALLOC_N(char, size + 1);

          if(!data) {
            rubinius::bug("could not allocate memory for string");
          }

          memcpy(data, so->byte_address(), size);
          data[size] = 0;

          heap_allocations[ffi_index] = data;
          *tmp = data;
        }
        values[ffi_index] = tmp;
        break;
      }
      }
    }

    if(CBOOL(varargs())) {
      // We need to call ffi_prep_cif_var for each call, since we need
      // to provide the actual number of arguments for each call.
      int status = ffi_prep_cif_var(cif, FFI_DEFAULT_ABI, req_count, arg_count, rtype, types);
      if(status != FFI_OK) {
        rubinius::bug("ffi_prep_cif failed");
      }
    }

    NativeMethodEnvironment* env = state->vm()->native_method_environment;
    CallFrame* saved_frame = env->current_call_frame();
    env->set_current_call_frame(call_frame);

    state->vm()->interrupt_with_signal();
    state->gc_independent(gct, call_frame);

    switch(ffi_data_local->ret_info.type) {
    case RBX_FFI_TYPE_CHAR: {
      ffi_arg result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_UCHAR: {
      ffi_arg result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_BOOL: {
      ffi_arg result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = RBOOL(result);
      break;
    }
    case RBX_FFI_TYPE_SHORT: {
      ffi_arg result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_USHORT: {
      ffi_arg result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Fixnum::from((native_int)result);
      break;
    }
    case RBX_FFI_TYPE_INT: {
      ffi_arg result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Integer::from(state, (native_int)result);
      break;
    }
    case RBX_FFI_TYPE_UINT: {
      ffi_arg result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Integer::from(state, (unsigned int)result);
      break;
    }
    case RBX_FFI_TYPE_LONG: {
      long result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_ULONG: {
      unsigned long result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_FLOAT: {
      float result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Float::create(state, (double)result);
      break;
    }
    case RBX_FFI_TYPE_DOUBLE: {
      double result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Float::create(state, result);
      break;
    }
    case RBX_FFI_TYPE_LONG_LONG: {
      long long result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_ULONG_LONG: {
      unsigned long long result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = Integer::from(state, result);
      break;
    }
    case RBX_FFI_TYPE_PTR: {
      void* result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      if(result == NULL) {
        ret = cNil;
      } else {
        ret = Pointer::create(state, result);
      }
      break;
    }
    case RBX_FFI_TYPE_ENUM: {
      ffi_arg result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      Array* ary = Array::create(state, 1);
      ary->set(state, 0, Integer::from(state, (native_int)result));
      ret = ffi_data->ret_info.enum_obj->send(state, call_frame, state->symbol("symbol"), ary);
      break;
    }
    case RBX_FFI_TYPE_CALLBACK: {
      void* result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      if(result == NULL) {
        ret = cNil;
      } else {
        NativeFunction* orig = ffi_data_local->ret_info.callback;
        NativeFunction* func = NativeFunction::create(state, state->symbol("ffi_tramp"), orig->ffi_data->arg_count);
        func->prep(state, orig->ffi_data->arg_count, orig->ffi_data->args_info, &orig->ffi_data->ret_info);
        func->ffi_data->ep = result;
        Proc* prc = Proc::create(state, state->vm()->shared.globals.proc.get());
        prc->bound_method(state, func);
        ret = (Object*)prc;
      }
      break;
    }
    case RBX_FFI_TYPE_STRING: {
      char* result;
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
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

      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);

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
      ffi_call(cif, FFI_FN(ffi_data_local->ep), &result, values);
      state->gc_dependent(gct, call_frame);
      ret = cNil;
      break;
    }
    }

    env->set_current_call_frame(saved_frame);

    for(size_t i = 0; i < arg_count; i++) {
      if(heap_allocations[i]) {
        XFREE(heap_allocations[i]);
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
        FFIArgInfo* arg = &func->ffi_data->args_info[i];
        if(arg->callback) {
          Object* tmp = mark.call(arg->callback);
          if(tmp) {
            arg->callback = force_as<NativeFunction>(tmp);
            mark.just_set(obj, tmp);
          }
        }
        if(arg->enum_obj) {
          Object* tmp = mark.call(arg->enum_obj);
          if(tmp) {
            arg->enum_obj = tmp;
            mark.just_set(obj, tmp);
          }
        }
      }
      FFIArgInfo* arg = &func->ffi_data->ret_info;
      if(arg->callback) {
        Object* tmp = mark.call(arg->callback);
        if(tmp) {
          arg->callback = force_as<NativeFunction>(tmp);
          mark.just_set(obj, tmp);
        }
      }
      if(arg->enum_obj) {
        Object* tmp = mark.call(arg->enum_obj);
        if(tmp) {
          arg->enum_obj = tmp;
          mark.just_set(obj, tmp);
        }
      }
      func->ffi_data->function = func;
    }
  }

}

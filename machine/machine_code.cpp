#include "config.h"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "memory.hpp"
#include "defines.hpp"
#include "machine_code.hpp"
#include "interpreter.hpp"

#include "object_utils.hpp"

#include "class/array.hpp"
#include "class/compiled_code.hpp"
#include "class/exception.hpp"
#include "class/fixnum.hpp"
#include "class/iseq.hpp"
#include "class/jit.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"
#include "class/class.hpp"
#include "class/location.hpp"
#include "class/constant_cache.hpp"
#include "class/call_site.hpp"

#include "instructions.hpp"

#include "instruments/timing.hpp"

#include "raise_reason.hpp"
#include "on_stack.hpp"

#include "configuration.hpp"
#include "dtrace/dtrace.h"

#ifdef RBX_WINDOWS
#include <malloc.h>
#endif

/*
 * An internalization of a CompiledCode which holds the instructions for the
 * method.
 */
namespace rubinius {

  void MachineCode::bootstrap(STATE) {
  }

  /*
   * Turns a CompiledCode's InstructionSequence into a C array of opcodes.
   */
  MachineCode::MachineCode(STATE, CompiledCode* code)
    : run(MachineCode::interpreter)
    , total(code->iseq()->opcodes()->num_fields())
    , type(NULL)
    , keywords(!code->keywords()->nil_p())
    , total_args(code->total_args()->to_native())
    , required_args(code->required_args()->to_native())
    , post_args(code->post_args()->to_native())
    , splat_position(-1)
    , stack_size(code->stack_size()->to_native())
    , number_of_locals(code->number_of_locals())
    , iregisters(0)
    , dregisters(0)
    , sample_count(0)
    , call_count(0)
    , uncommon_count(0)
    , _call_site_count_(0)
    , _constant_cache_count_(0)
    , _references_count_(0)
    , _references_(NULL)
    , _description_(NULL)
    , unspecialized(NULL)
    , fallback(NULL)
    , execute_status_(eInterpret)
    , name_(code->name())
    , method_id_(state->shared().inc_method_count(state))
    , debugging(false)
    , flags(0)
  {
    if(keywords) {
      keywords_count = code->keywords()->num_fields() / 2;
    }

    if(code->experimental_tag_p()) {
      opcodes = new opcode[total+1];

      run = (InterpreterRunner)Interpreter::execute;
      Interpreter::prepare(state, code, this);
    } else {
      opcodes = new opcode[total];

      fill_opcodes(state, code);
    }

    if(Fixnum* pos = try_as<Fixnum>(code->splat())) {
      splat_position = pos->to_native();
    }

    for(int i = 0; i < cMaxSpecializations; i++) {
      specializations[i].class_data.raw = 0;
      specializations[i].execute = 0;
    }

    state->shared().om->add_code_resource(state, this);
  }

  MachineCode::~MachineCode() {
#ifdef RBX_GC_DEBUG
    memset(opcodes, 0xFF, total * sizeof(opcode));
#endif
    delete[] opcodes;

    if(references()) {
#ifdef RBX_GC_DEBUG
      memset(references(), 0xFF, references_count() * sizeof(size_t));
#endif
      delete[] references();
    }

    if(description()) delete description();
  }

  void MachineCode::finalize(STATE) {
    for(size_t i = 0; i < references_count(); i++) {
      if(size_t ip = references()[i]) {
        if(CallSite* call_site = try_as<CallSite>(
              reinterpret_cast<Object*>(opcodes[ip])))
        {
          call_site->finalize(state);
        }
      }
    }
  }

  int MachineCode::size() {
    return sizeof(MachineCode) +
      (total * sizeof(opcode)) + // opcodes
      (total * sizeof(void*));
  }

  void MachineCode::fill_opcodes(STATE, CompiledCode* original) {
    Tuple* lits = original->literals();
    Tuple* ops = original->iseq()->opcodes();

    size_t rcount = 0;
    size_t rindex = 0;
    size_t calls_count = 0;
    size_t constants_count = 0;

    for(size_t width = 0, ip = 0; ip < total; ip += width) {
      opcode op = opcodes[ip] = as<Fixnum>(ops->at(ip))->to_native();
      width = InstructionSequence::instruction_width(opcodes[ip]);

      switch(width) {
      case 4:
        opcodes[ip + 3] = as<Fixnum>(ops->at(state, ip + 3))->to_native();
        // fall through
      case 3:
        opcodes[ip + 2] = as<Fixnum>(ops->at(state, ip + 2))->to_native();
        // fall through
      case 2:
        opcodes[ip + 1] = as<Fixnum>(ops->at(state, ip + 1))->to_native();
        break;
      case 1:
        continue;
      }

      switch(op) {
      case InstructionSequence::insn_create_block:
      case InstructionSequence::insn_push_literal:
      case InstructionSequence::insn_push_memo:
      case InstructionSequence::insn_check_serial:
      case InstructionSequence::insn_check_serial_private:
      case InstructionSequence::insn_send_super_stack_with_block:
      case InstructionSequence::insn_send_super_stack_with_splat:
      case InstructionSequence::insn_zsuper:
      case InstructionSequence::insn_send_vcall:
      case InstructionSequence::insn_send_method:
      case InstructionSequence::insn_send_stack:
      case InstructionSequence::insn_send_stack_with_block:
      case InstructionSequence::insn_send_stack_with_splat:
      case InstructionSequence::insn_object_to_s:
      case InstructionSequence::insn_push_const:
      case InstructionSequence::insn_find_const:
        rcount++;
      }
    }

    references_count(rcount);
    references(new size_t[rcount]);

    bool allow_private = false;
    bool is_super = false;

    for(size_t width = 0, ip = 0; ip < total; ip += width) {
      width = InstructionSequence::instruction_width(opcodes[ip]);

      switch(opcode op = opcodes[ip]) {
      case InstructionSequence::insn_push_int:
        opcodes[ip + 1] = reinterpret_cast<opcode>(Fixnum::from(opcodes[ip + 1]));
        break;
      case InstructionSequence::insn_create_block: {
        references()[rindex++] = ip + 1;

        Object* value = reinterpret_cast<Object*>(lits->at(opcodes[ip + 1]));

        if(CompiledCode* code = try_as<CompiledCode>(value)) {
          opcodes[ip + 1] = reinterpret_cast<opcode>(code);
        } else {
          opcodes[ip + 1] = reinterpret_cast<opcode>(as<String>(value));
        }
        break;
      }
      case InstructionSequence::insn_push_memo:
      case InstructionSequence::insn_push_literal: {
        references()[rindex++] = ip + 1;

        Object* value = as<Object>(lits->at(opcodes[ip + 1]));
        opcodes[ip + 1] = reinterpret_cast<opcode>(value);
        break;
      }
      case InstructionSequence::insn_set_ivar:
      case InstructionSequence::insn_push_ivar:
      case InstructionSequence::insn_set_const:
      case InstructionSequence::insn_set_const_at: {
        Symbol* sym = as<Symbol>(lits->at(opcodes[ip + 1]));
        opcodes[ip + 1] = reinterpret_cast<opcode>(sym);
        break;
      }
      case InstructionSequence::insn_invoke_primitive: {
        Symbol* name = as<Symbol>(lits->at(opcodes[ip + 1]));

        InvokePrimitive invoker = Primitives::get_invoke_stub(state, name);
        opcodes[ip + 1] = reinterpret_cast<intptr_t>(invoker);
        break;
      }
      case InstructionSequence::insn_allow_private:
        allow_private = true;

        break;
      case InstructionSequence::insn_send_super_stack_with_block:
      case InstructionSequence::insn_send_super_stack_with_splat:
      case InstructionSequence::insn_zsuper:
        is_super = true;
        // fall through
      case InstructionSequence::insn_send_vcall:
      case InstructionSequence::insn_send_method:
      case InstructionSequence::insn_send_stack:
      case InstructionSequence::insn_send_stack_with_block:
      case InstructionSequence::insn_send_stack_with_splat:
      case InstructionSequence::insn_object_to_s:
      case InstructionSequence::insn_check_serial:
      case InstructionSequence::insn_check_serial_private: {
        references()[rindex++] = ip + 1;
        calls_count++;

        Symbol* name = try_as<Symbol>(lits->at(opcodes[ip + 1]));
        if(!name) name = nil<Symbol>();

        CallSite* call_site = CallSite::create(state, name, ip);

        if(op == InstructionSequence::insn_send_vcall) {
          allow_private = true;
          call_site->set_is_vcall();
        } else if(op == InstructionSequence::insn_object_to_s) {
          allow_private = true;
        }

        if(allow_private) call_site->set_is_private();
        if(is_super) call_site->set_is_super();

        store_call_site(state, original, ip, call_site);
        is_super = false;
        allow_private = false;

        break;
      }
      case InstructionSequence::insn_push_const:
      case InstructionSequence::insn_find_const: {
        references()[rindex++] = ip + 1;
        constants_count++;

        Symbol* name = as<Symbol>(lits->at(opcodes[ip + 1]));

        ConstantCache* cache = ConstantCache::empty(state, name, original, ip);
        store_constant_cache(state, original, ip, cache);

        break;
      }
      }
    }

    call_site_count(calls_count);
    constant_cache_count(constants_count);
  }

  CallSite* MachineCode::call_site(STATE, int ip) {
    Object* obj = reinterpret_cast<Object*>(opcodes[ip + 1]);
    return as<CallSite>(obj);
  }

  ConstantCache* MachineCode::constant_cache(STATE, int ip) {
    Object* obj = reinterpret_cast<Object*>(opcodes[ip + 1]);
    return as<ConstantCache>(obj);
  }

  Tuple* MachineCode::call_sites(STATE) {
    size_t count = call_site_count();
    Tuple* sites = Tuple::create(state, count);

    for(size_t i = 0, j = 0;
        i < count && j < references_count();
        j++)
    {
      if(CallSite* site =
          try_as<CallSite>(reinterpret_cast<Object*>(opcodes[references()[j]])))
      {
        sites->put(state, i++, site);
      }
    }

    return sites;
  }

  Tuple* MachineCode::constant_caches(STATE) {
    size_t count = constant_cache_count();
    Tuple* caches = Tuple::create(state, count);

    for(size_t i = 0, j = 0;
        i < count && j < references_count();
        j++)
    {
      if(ConstantCache* cache =
          try_as<ConstantCache>(reinterpret_cast<Object*>(opcodes[references()[j]])))
      {
        caches->put(state, i++, cache);
      }
    }

    return caches;
  }

  void MachineCode::store_call_site(STATE, CompiledCode* code, int ip, CallSite* call_site) {
    atomic::memory_barrier();
    opcodes[ip + 1] = reinterpret_cast<intptr_t>(call_site);
    state->memory()->write_barrier(code, call_site);
  }

  void MachineCode::store_constant_cache(STATE, CompiledCode* code, int ip, ConstantCache* constant_cache) {
    atomic::memory_barrier();
    opcodes[ip + 1] = reinterpret_cast<intptr_t>(constant_cache);
    state->memory()->write_barrier(code, constant_cache);
  }

  void MachineCode::set_description(STATE) {
    if(description()) return;

    CallFrame* call_frame = state->vm()->call_frame();

    Class* klass = call_frame->self()->class_object(state);
    Module* method_module = call_frame->module();

    std::string* desc = new std::string();

    if(kind_of<SingletonClass>(method_module)) {
      desc->append(method_module->debug_str(state));
      desc->append(".");
    } else if(method_module != klass) {
      desc->append(method_module->debug_str(state));
      desc->append("(");
      desc->append(klass->debug_str(state));
      desc->append(")");
    } else {
      desc->append(klass->debug_str(state));
      desc->append("#");
    }

    desc->append(name()->cpp_str(state));

    description(desc);
  }

  // Argument handler implementations

  // For when the method expects no arguments at all (no splat, nothing)
  class NoArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      return args.total() == 0;
    }
  };

  // For when the method expects 1 and only 1 argument
  class OneArgument {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      if(args.total() != 1) return false;
      scope->set_local(0, args.get_argument(0));
      return true;
    }
  };

  // For when the method expects 2 and only 2 arguments
  class TwoArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      if(args.total() != 2) return false;
      scope->set_local(0, args.get_argument(0));
      scope->set_local(1, args.get_argument(1));
      return true;
    }
  };

  // For when the method expects 3 and only 3 arguments
  class ThreeArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      if(args.total() != 3) return false;
      scope->set_local(0, args.get_argument(0));
      scope->set_local(1, args.get_argument(1));
      scope->set_local(2, args.get_argument(2));
      return true;
    }
  };

  // For when the method expects a fixed number of arguments (no splat)
  class FixedArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      if((native_int)args.total() != mcode->total_args) return false;

      for(native_int i = 0; i < mcode->total_args; i++) {
        scope->set_local(i, args.get_argument(i));
      }

      return true;
    }
  };

  // For when a method takes all arguments as a splat
  class SplatOnlyArgument {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope,
                     Arguments& args)
    {
      const size_t total = args.total();
      Array* ary = Array::create(state, total);

      for(size_t i = 0; i < total; i++) {
        ary->set(state, i, args.get_argument(i));
      }

      scope->set_local(mcode->splat_position, ary);
      return true;
    }
  };

  // The fallback, can handle all cases
  class GenericArguments {
  public:
    static bool call(STATE, MachineCode* mcode, StackVariables* scope, Arguments& args) {
      /* There are 5 types of arguments, illustrated here:
       *    m(a, b=1, *c, d, e: 2)
       *
       *  where:
       *    a is a head (pre optional/splat) fixed position argument
       *    b is an optional argument
       *    c is a rest argument
       *    d is a post (optional/splat) argument
       *    e is a keyword argument, which may be required (having no default
       *      value), optional, or keyword rest argument (**kw).
       *
       * The arity checking above ensures that we have at least one argument
       * on the stack for each fixed position argument (ie arguments a and d
       * above).
       *
       * We assign the arguments in the following order: first the fixed
       * arguments (head and post) and possibly the keyword argument, then the
       * optional arguments, and the remainder (if any) are combined in an
       * array for the rest argument.
       *
       * We assign values from the sender's side to local variables on the
       * receiver's side. Which values to assign are computed as follows:
       *
       *  sender indexes (arguments)
       *  -v-v-v-v-v-v-v-v-v-v-v-v--
       *
       *   0...H  H...H+ON  H+ON...N-P-K  N-P-K...N-K  N-K
       *   |      |         |             |            |
       *   H      O         R             P            K
       *   |      |         |             |            |
       *   0...H  H...H+O   RI            PI...PI+P    KI
       *
       *  -^-^-^-^-^-^-^-^-^-^-^-^-
       *  receiver indexes (locals)
       *
       * where:
       *
       *  arguments passed by sender
       *  --------------------------
       *    N  : total number of arguments passed
       *    H* : number of head arguments
       *    E  : number of extra arguments
       *    ON : number or arguments assigned to optional parameters
       *    RN : number of arguments assigned to the rest argument
       *    P* : number of post arguments
       *    K  : number of keyword arguments passed, 1 if the last argument is
       *         a Hash or if #to_hash returns a Hash, 0 otherwise
       *
       *  parameters defined by receiver
       *  ------------------------------
       *    T  : total number of parameters
       *    M  : number of head + post parameters
       *    H* : number of head parameters
       *    O  : number of optional parameters
       *    RP : true if a rest parameter is defined, false otherwise
       *    RI : index of rest parameter if RP is true, else -1
       *    P* : number of post parameters
       *    PI : index of the first post parameter
       *    KP : true if a keyword parameter is defined, false otherwise
       *    KA : true if the keyword argument was extracted from the passed
       *         argument leaving a remaining value
       *    KI : index of keyword rest parameter
       *
       *  (*) The values of H and P are fixed and they represent the same
       *  values at both the sender and receiver, so they are named the same.
       *
       *  formulas
       *  --------
       *    K  = KP && !KA && N > M ? 1 : 0
       *    E  = N - M - K
       *    O  = T - M - (keywords ? 1 : 0)
       *    ON = (X = MIN(O, E)) > 0 ? X : 0
       *    RN = RP && (X = E - ON) > 0 ? X : 0
       *    PI = H + O + (RP ? 1 : 0)
       *    KI = RP ? T : T - 1
       *
       */

      const native_int N = args.total();
      const native_int T = mcode->total_args;
      const native_int M = mcode->required_args;
      const native_int O = T - M - (mcode->keywords ? 1 : 0);

      /* TODO: Clean up usage to uniformly refer to 'splat' as N arguments
       * passed from sender at a single position and 'rest' as N arguments
       * collected into a single argument at the receiver.
       */
      const native_int RI = mcode->splat_position;
      const bool RP = (RI >= 0);

      // expecting 0, got 0.
      if(T == 0 && N == 0) {
        if(RP) {
          scope->set_local(mcode->splat_position, Array::create(state, 0));
        }

        return true;
      }

      // Too few args!
      if(N < M) return false;

      // Too many args (no rest argument!)
      if(!RP && N > T) return false;

      const native_int P  = mcode->post_args;
      const native_int H  = M - P;

      Object* kw = 0;
      Object* kw_remainder = 0;
      bool KP = false;
      bool KA = false;

      if(mcode->keywords && N > M) {
        Object* obj = args.get_argument(N - 1);
        Object* arguments[2] = { obj, RBOOL(O > 0 || RP) };

        Arguments args(G(sym_keyword_object), G(runtime), 2, arguments);
        Dispatch dispatch(G(sym_keyword_object));

        if(Object* kw_result = dispatch.send(state, args)) {
          if(Array* ary = try_as<Array>(kw_result)) {
            Object* o = 0;

            if(!(o = ary->get(state, 0))->nil_p()) {
              kw_remainder = o;
              KA = true;
            }

            kw = ary->get(state, 1);
            KP = true;
          }
        } else {
          return false;
        }
      }

      const native_int K = (KP && !KA && N > M) ? 1 : 0;
      const native_int E = N - M - K;

      // Too many arguments
      if(mcode->keywords && !RP && !KP && E > O) return false;

      native_int X;

      const native_int ON = (X = MIN(O, E)) > 0 ? X : 0;
      const native_int RN = (RP && (X = E - ON) > 0) ? X : 0;
      const native_int PI = H + O + (RP ? 1 : 0);
      const native_int KI = RP ? T : T - 1;

      native_int a = 0;   // argument index
      native_int l = 0;   // local index

      // head arguments
      if(H > 0) {
        for(; a < H; l++, a++) {
          scope->set_local(l, args.get_argument(a));
        }
      }

      // optional arguments
      if(O > 0) {
        for(; l < H + O && a < H + ON; l++, a++) {
          if(unlikely(kw_remainder && !RP && (a == N - 1))) {
            scope->set_local(l, kw_remainder);
          } else {
            scope->set_local(l, args.get_argument(a));
          }
        }

        for(; l < H + O; l++) {
          scope->set_local(l, G(undefined));
        }
      }

      // rest arguments
      if(RP) {
        Array* ary;

        if(RN > 0) {
          ary = Array::create(state, RN);

          for(int i = 0; i < RN && a < N - P - K; i++, a++) {
            if(unlikely(kw_remainder && (a == N - 1))) {
              ary->set(state, i, kw_remainder);
            } else {
              ary->set(state, i, args.get_argument(a));
            }
          }
        } else {
          ary = Array::create(state, 0);
        }

        scope->set_local(RI, ary);
      }

      // post arguments
      if(P > 0) {
        for(l = PI; l < PI + P && a < N - K; l++, a++) {
          scope->set_local(l, args.get_argument(a));
        }
      }

      // keywords
      if(kw) {
        scope->set_local(KI, kw);
      }

      return true;
    }
  };

  /*
   * Looks at the opcodes for this method and optimizes instance variable
   * access by using special byte codes.
   *
   * For push_ivar, uses push_my_field when the instance variable has an
   * index assigned.  Same for set_ivar/store_my_field.
   */

  void MachineCode::specialize(STATE, CompiledCode* original, TypeInfo* ti) {
    type = ti;
    for(size_t i = 0; i < total;) {
      opcode op = opcodes[i];

      if(op == InstructionSequence::insn_push_ivar) {
        native_int sym = as<Symbol>(reinterpret_cast<Object*>(opcodes[i + 1]))->index();

        TypeInfo::Slots::iterator it = ti->slots.find(sym);
        if(it != ti->slots.end()) {
          opcodes[i] = InstructionSequence::insn_push_my_offset;
          opcodes[i + 1] = ti->slot_locations[it->second];
        }
      } else if(op == InstructionSequence::insn_set_ivar) {
        native_int sym = as<Symbol>(reinterpret_cast<Object*>(opcodes[i + 1]))->index();

        TypeInfo::Slots::iterator it = ti->slots.find(sym);
        if(it != ti->slots.end()) {
          opcodes[i] = InstructionSequence::insn_store_my_field;
          opcodes[i + 1] = it->second;
        }
      }

      i += InstructionSequence::instruction_width(op);
    }
  }

  void MachineCode::setup_argument_handler() {
    // Firstly, use the generic case that handles all cases
    fallback = &MachineCode::execute_specialized<GenericArguments>;

    // If there are no optionals, only a fixed number of positional arguments.
    if(total_args == required_args) {
      // if no arguments are expected
      if(total_args == 0) {
        // and there is no splat, use the fastest case.
        if(splat_position == -1) {
          fallback = &MachineCode::execute_specialized<NoArguments>;

        // otherwise use the splat only case.
        } else if(!keywords) {
          fallback = &MachineCode::execute_specialized<SplatOnlyArgument>;
        }
      // Otherwise use the few specialized cases iff there is no splat
      } else if(splat_position == -1) {
        switch(total_args) {
        case 1:
          fallback = &MachineCode::execute_specialized<OneArgument>;
          break;
        case 2:
          fallback = &MachineCode::execute_specialized<TwoArguments>;
          break;
        case 3:
          fallback = &MachineCode::execute_specialized<ThreeArguments>;
          break;
        default:
          fallback = &MachineCode::execute_specialized<FixedArguments>;
          break;
        }
      }
    }
  }

  /* This is the execute implementation used by normal Ruby code,
   * as opposed to Primitives or FFI functions.
   * It prepares a Ruby method for execution.
   * Here, +exec+ is a MachineCode instance accessed via the +machine_code+ slot on
   * CompiledCode.
   *
   * This method works as a template even though it's here because it's never
   * called from outside of this file. Thus all the template expansions are done.
   */
  template <typename ArgumentHandler>
    Object* MachineCode::execute_specialized(STATE,
        Executable* exec, Module* mod, Arguments& args)
    {
      CompiledCode* code = as<CompiledCode>(exec);
      MachineCode* mcode = code->machine_code();

      StackVariables* scope = ALLOCA_STACKVARIABLES(mcode->number_of_locals);
      // Originally, I tried using msg.module directly, but what happens is if
      // super is used, that field is read. If you combine that with the method
      // being called recursively, msg.module can change, causing super() to
      // look in the wrong place.
      //
      // Thus, we have to cache the value in the StackVariables.
      scope->initialize(args.recv(), args.block(), mod, mcode->number_of_locals);

      // If argument handling fails..
      if(ArgumentHandler::call(state, mcode, scope, args) == false) {
        if(state->vm()->thread_state()->raise_reason() == cNone) {
          Exception* exc =
            Exception::make_argument_error(state, mcode->total_args, args.total(), args.name());
          exc->locations(state, Location::from_call_stack(state));
          state->raise_exception(exc);
        }

        return NULL;
      }

      CallFrame* previous_frame = NULL;
      CallFrame* call_frame = ALLOCA_CALL_FRAME(mcode->stack_size);

      call_frame->prepare(mcode->stack_size);

      call_frame->previous = NULL;
      call_frame->lexical_scope_ = code->scope();
      call_frame->dispatch_data = NULL;
      call_frame->compiled_code = code;
      call_frame->flags = 0;
      call_frame->top_scope_ = NULL;
      call_frame->scope = scope;
      call_frame->arguments = &args;

      if(!state->vm()->push_call_frame(state, call_frame, previous_frame)) {
        return NULL;
      }

      mcode->call_count++;

      Object* value = 0;

      RUBINIUS_METHOD_ENTRY_HOOK(state, scope->module(), args.name());
      value = (*mcode->run)(state, mcode);
      RUBINIUS_METHOD_RETURN_HOOK(state, scope->module(), args.name());

      if(!state->vm()->pop_call_frame(state, previous_frame)) {
        return NULL;
      }

      return value;
    }

  /** This is used as a fallback way of entering the interpreter */
  Object* MachineCode::execute(STATE, Executable* exec, Module* mod, Arguments& args) {
    return execute_specialized<GenericArguments>(state, exec, mod, args);
  }

  Object* MachineCode::execute_as_script(STATE, CompiledCode* code) {
    MachineCode* mcode = code->machine_code();

    StackVariables* scope = ALLOCA_STACKVARIABLES(mcode->number_of_locals);
    // Originally, I tried using msg.module directly, but what happens is if
    // super is used, that field is read. If you combine that with the method
    // being called recursively, msg.module can change, causing super() to
    // look in the wrong place.
    //
    // Thus, we have to cache the value in the StackVariables.
    scope->initialize(G(main), cNil, G(object), mcode->number_of_locals);

    CallFrame* previous_frame = 0;
    CallFrame* call_frame = ALLOCA_CALL_FRAME(mcode->stack_size);

    call_frame->prepare(mcode->stack_size);

    Arguments args(state->symbol("__script__"), G(main), cNil, 0, 0);

    call_frame->previous = NULL;
    call_frame->lexical_scope_ = code->scope();
    call_frame->dispatch_data = 0;
    call_frame->compiled_code = code;
    call_frame->flags = CallFrame::cScript | CallFrame::cTopLevelVisibility;
    call_frame->top_scope_ = 0;
    call_frame->scope = scope;
    call_frame->arguments = &args;

    if(!state->vm()->push_call_frame(state, call_frame, previous_frame)) {
      return NULL;
    }

    state->vm()->checkpoint(state);

    // Don't generate profiling info here, it's expected
    // to be done by the caller.

    Object* value = (*mcode->run)(state, mcode);

    if(!state->vm()->pop_call_frame(state, previous_frame)) {
      return NULL;
    }

    return value;
  }

  // If +disable+ is set, then the method is tagged as not being
  // available for JIT.
  void MachineCode::deoptimize(STATE, CompiledCode* original, bool disable)
  {
    G(jit)->start_method_update(state);

    bool still_others = false;

    /* TODO: JIT
    for(int i = 0; i < cMaxSpecializations; i++) {
      if(!rd) {
        specializations[i].class_data.raw = 0;
        specializations[i].execute = 0;
        specializations[i].jit_data = 0;
      } else if(specializations[i].jit_data == rd) {
        specializations[i].class_data.raw = 0;
        specializations[i].execute = 0;
        specializations[i].jit_data = 0;
      } else if(specializations[i].jit_data) {
        still_others = true;
      }
    }

    if(!rd || original->jit_data() == rd) {
      unspecialized = 0;
      original->jit_data(0);
    }

    if(original->jit_data()) still_others = true;
    */

    if(!still_others) {
      execute_status_ = eInterpret;

      // This resets execute to use the interpreter
      original->set_executor(fallback);
    }

    if(disable) {
      execute_status_ = eJITDisable;
      original->set_executor(fallback);
    } else if(execute_status_ == eJITDisable && still_others) {
      execute_status_ = eJIT;
    }

    if(original->execute == CompiledCode::specialized_executor) {
      bool found = false;

      for(int i = 0; i < cMaxSpecializations; i++) {
        if(specializations[i].execute) found = true;
      }

      if(unspecialized) found = true;

      if(!found) rubinius::bug("no specializations!");
    }

    G(jit)->end_method_update(state);
  }

  /*
   * Ensures the specified IP value is a valid address.
   */
  bool MachineCode::validate_ip(STATE, size_t ip) {
    /* Ensure ip is valid */
    MachineCode::Iterator iter(this);
    for(; !iter.end(); iter.inc()) {
      if(iter.position() >= ip) break;
    }
    return ip == iter.position();
  }
}

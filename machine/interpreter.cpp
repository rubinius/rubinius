#include "interpreter.hpp"
#include "machine_code.hpp"

#include "interpreter/addresses.hpp"

#include "class/call_site.hpp"
#include "class/compiled_code.hpp"
#include "class/constant_cache.hpp"
#include "class/location.hpp"

namespace rubinius {
  void Interpreter::prepare(STATE, CompiledCode* compiled_code, MachineCode* machine_code) {
    Tuple* lits = compiled_code->literals();
    Tuple* ops = compiled_code->iseq()->opcodes();

    opcode* opcodes = machine_code->opcodes;
    size_t total = machine_code->total;

    size_t rcount = 0;
    size_t rindex = 0;
    size_t calls_count = 0;
    size_t constants_count = 0;

    // Set exception handler IP
    // TODO: handle in the bytecode compiler
    opcodes[total-1] =
      reinterpret_cast<intptr_t>(instructions::data_run_exception.interpreter_address);

    for(size_t width = 0, ip = 0; ip < total; ip += width) {
      opcode op = as<Fixnum>(ops->at(ip))->to_native();
      width = Interpreter::instruction_data_(op).width;

      opcodes[ip] =
        reinterpret_cast<intptr_t>(Interpreter::instruction_data_(op).interpreter_address);

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
      case instructions::data_create_block.id:
      case instructions::data_push_literal.id:
      case instructions::data_push_memo.id:
      case instructions::data_check_serial.id:
      case instructions::data_check_serial_private.id:
      case instructions::data_send_super_stack_with_block.id:
      case instructions::data_send_super_stack_with_splat.id:
      case instructions::data_zsuper.id:
      case instructions::data_send_vcall.id:
      case instructions::data_send_method.id:
      case instructions::data_send_stack.id:
      case instructions::data_send_stack_with_block.id:
      case instructions::data_send_stack_with_splat.id:
      case instructions::data_object_to_s.id:
      case instructions::data_push_const.id:
      case instructions::data_find_const.id:
        rcount++;
      }
    }

    machine_code->references_count(rcount);
    machine_code->references(new size_t[rcount]);

    bool allow_private = false;
    bool is_super = false;

    for(size_t width = 0, ip = 0; ip < total; ip += width) {
      opcode op = as<Fixnum>(ops->at(ip))->to_native();
      width = Interpreter::instruction_data_(op).width;

      switch(op) {
      case instructions::data_push_int.id:
        opcodes[ip + 1] = reinterpret_cast<opcode>(Fixnum::from(opcodes[ip + 1]));
        break;
      case instructions::data_create_block.id: {
        machine_code->references()[rindex++] = ip + 1;

        Object* value = reinterpret_cast<Object*>(lits->at(opcodes[ip + 1]));

        if(CompiledCode* code = try_as<CompiledCode>(value)) {
          opcodes[ip + 1] = reinterpret_cast<opcode>(code);
        } else {
          opcodes[ip + 1] = reinterpret_cast<opcode>(as<String>(value));
        }
        break;
      }
      case instructions::data_push_memo.id:
      case instructions::data_push_literal.id: {
        machine_code->references()[rindex++] = ip + 1;

        Object* value = as<Object>(lits->at(opcodes[ip + 1]));
        opcodes[ip + 1] = reinterpret_cast<opcode>(value);
        break;
      }
      case instructions::data_set_ivar.id:
      case instructions::data_push_ivar.id:
      case instructions::data_set_const.id:
      case instructions::data_set_const_at.id: {
        Symbol* sym = as<Symbol>(lits->at(opcodes[ip + 1]));
        opcodes[ip + 1] = reinterpret_cast<opcode>(sym);
        break;
      }
      case instructions::data_invoke_primitive.id: {
        Symbol* name = as<Symbol>(lits->at(opcodes[ip + 1]));

        InvokePrimitive invoker = Primitives::get_invoke_stub(state, name);
        opcodes[ip + 1] = reinterpret_cast<intptr_t>(invoker);
        break;
      }
      case instructions::data_allow_private.id:
        allow_private = true;

        break;
      case instructions::data_send_super_stack_with_block.id:
      case instructions::data_send_super_stack_with_splat.id:
      case instructions::data_zsuper.id:
        is_super = true;
        // fall through
      case instructions::data_send_vcall.id:
      case instructions::data_send_method.id:
      case instructions::data_send_stack.id:
      case instructions::data_send_stack_with_block.id:
      case instructions::data_send_stack_with_splat.id:
      case instructions::data_object_to_s.id:
      case instructions::data_check_serial.id:
      case instructions::data_check_serial_private.id: {
        machine_code->references()[rindex++] = ip + 1;
        calls_count++;

        Symbol* name = try_as<Symbol>(lits->at(opcodes[ip + 1]));
        if(!name) name = nil<Symbol>();

        CallSite* call_site = CallSite::create(state, name, ip);

        if(op == instructions::data_send_vcall.id) {
          allow_private = true;
          call_site->set_is_vcall();
        } else if(op == instructions::data_object_to_s.id) {
          allow_private = true;
        }

        if(allow_private) call_site->set_is_private();
        if(is_super) call_site->set_is_super();

        machine_code->store_call_site(state, compiled_code, ip, call_site);
        is_super = false;
        allow_private = false;

        break;
      }
      case instructions::data_push_const.id:
      case instructions::data_find_const.id: {
        machine_code->references()[rindex++] = ip + 1;
        constants_count++;

        Symbol* name = as<Symbol>(lits->at(opcodes[ip + 1]));

        ConstantCache* cache = ConstantCache::empty(state, name, compiled_code, ip);
        machine_code->store_constant_cache(state, compiled_code, ip, cache);

        break;
      }
      }
    }

    machine_code->call_site_count(calls_count);
    machine_code->constant_cache_count(constants_count);
  }

  intptr_t Interpreter::execute(STATE, MachineCode* const machine_code) {
    InterpreterState is;
    UnwindInfoSet unwinds;
    Exception* exception = 0;
    intptr_t* opcodes = (intptr_t*)machine_code->opcodes;

    CallFrame* call_frame = state->vm()->call_frame();
    call_frame->ret_ip_ = machine_code->total - 2;
    call_frame->exception_ip_ = machine_code->total - 1;
    call_frame->stack_ptr_ = call_frame->stk - 1;
    call_frame->machine_code = machine_code;
    call_frame->is = &is;
    call_frame->unwinds = &unwinds;

    try {
      return ((Instruction)opcodes[call_frame->ip()])(state, call_frame, opcodes);
    } catch(TypeError& e) {
      exception = Exception::make_type_error(state, e.type, e.object, e.reason);
      exception->locations(state, Location::from_call_stack(state));

      call_frame->scope->flush_to_heap(state);
    } catch(const RubyException& exc) {
      if(exc.exception->locations()->nil_p()) {
        exc.exception->locations(state, Location::from_call_stack(state));
      }
      exception = exc.exception;
    } catch(const std::exception& e) {
      exception = Exception::make_interpreter_error(state, e.what());
      exception->locations(state, Location::from_call_stack(state));

      call_frame->scope->flush_to_heap(state);
    } catch(...) {
      exception = Exception::make_interpreter_error(state, "unknown C++ exception thrown");
      exception->locations(state, Location::from_call_stack(state));

      call_frame->scope->flush_to_heap(state);
    }

    state->raise_exception(exception);
    return 0;
  }
}

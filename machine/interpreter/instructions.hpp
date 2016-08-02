#ifndef RBX_INSNS_HPP
#define RBX_INSNS_HPP

/*
#include "builtin/object.hpp"
#include "builtin/array.hpp"
#include "builtin/autoload.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/class.hpp"
#include "builtin/code_db.hpp"
#include "builtin/compiled_code.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/iseq.hpp"
#include "builtin/lexical_scope.hpp"
#include "builtin/native_method.hpp"
#include "builtin/lookup_table.hpp"
#include "builtin/proc.hpp"
#include "builtin/thread.hpp"
#include "builtin/system.hpp"
#include "builtin/constant_cache.hpp"
#include "builtin/location.hpp"
#include "builtin/thread_state.hpp"
#include "builtin/call_site.hpp"
#include "builtin/variable_scope.hpp"

#include "call_frame.hpp"

#include "memory.hpp"
#include "arguments.hpp"
#include "dispatch.hpp"
#include "instructions.hpp"
#include "interpreter.hpp"
#include "configuration.hpp"
#include "on_stack.hpp"

#include "helpers.hpp"
*/

#include "interpreter/prototypes.hpp"

using namespace rubinius;

typedef intptr_t (*Instruction)(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

#define CALL_FLAG_CONCAT 2

#define next_int          opcodes[call_frame->inc_ip()]
#define store_ip(ip)      (call_frame->set_ip(ip))
#define store_literal(x)  (x)

#define STACK_PTR call_frame->stack_ptr_

/** We have to use the local here we need to evaluate val before we alter
 * the stack. The reason is evaluating val might throw an exception. The
 * old code used an undefined behavior, this forces the order. */
#define stack_push(val) ({ Object* __stack_v = (val); *++STACK_PTR = __stack_v; })
#define stack_pop() (*STACK_PTR--)
#define stack_set_top(val) *STACK_PTR = (val)

#define stack_top() (*STACK_PTR)
#define stack_back(count) (*(STACK_PTR - count))
#define stack_clear(count) STACK_PTR -= count

#define stack_position(where) (STACK_PTR = call_frame->stk + where)
#define stack_calculate_sp() (STACK_PTR - call_frame->stk)
#define stack_back_position(count) (STACK_PTR - (count - 1))

#define stack_local(which) call_frame->stk[/*machine_code->stack_size - */which - 1]

#define both_fixnum_p(_p1, _p2) ((uintptr_t)(_p1) & (uintptr_t)(_p2) & TAG_FIXNUM)

#define CHECK_AND_PUSH(val) \
   if(val == NULL) { return NULL; } \
   else { stack_push(val); }

#define interp_assert(code) if(!(code)) { Exception::internal_error(state, "assertion failed: " #code); RUN_EXCEPTION(); }

#define RUN_EXCEPTION() (1)

#define SET_CALL_FLAGS(val) (val) // is.call_flags = (val)
#define CALL_FLAGS()        (1) // is.call_flags

#endif

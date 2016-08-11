#ifndef RBX_INTERPRETER_INSTRUCTIONS_HPP
#define RBX_INTERPRETER_INSTRUCTIONS_HPP

#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"
#include "machine_code.hpp"

#include "interpreter/prototypes.hpp"
#include "instructions/data.hpp"

using namespace rubinius;

typedef intptr_t (*Instruction)(STATE, CallFrame* call_frame, intptr_t const opcodes[]);

#define CALL_FLAG_CONCAT 2

#define argument(n)       opcodes[call_frame->ip() + n + 1]

// TODO: instructions
#define store_literal(x)  (x)

#define STACK_PTR call_frame->stack_ptr_

/* We have to use the local here we need to evaluate val before we alter the
 * stack. The reason is evaluating val might throw an exception. The old code
 * used an undefined behavior, this forces the order.
 */
#define stack_push(val) ({ Object* __stack_v = (val); *++STACK_PTR = __stack_v; })
#define stack_pop() (*STACK_PTR--)
#define stack_set_top(val) *STACK_PTR = (val)

#define stack_top() (*STACK_PTR)
#define stack_back(count) (*(STACK_PTR - count))
#define stack_clear(count) STACK_PTR -= count

#define stack_position(where) (STACK_PTR = call_frame->stk + where)
#define stack_calculate_sp() (STACK_PTR - call_frame->stk)
#define stack_back_position(count) (STACK_PTR - (count - 1))

#define stack_local(which) call_frame->stk[call_frame->machine_code->stack_size - which - 1]

#define both_fixnum_p(_p1, _p2) ((uintptr_t)(_p1) & (uintptr_t)(_p2) & TAG_FIXNUM)

#define CHECK_AND_PUSH(val) \
  if(val == NULL) { \
    return false; \
  } else { \
    stack_push(val); \
    return true; \
  }

#define interp_assert(code) if(!(code)) { \
  Exception::internal_error(state, "assertion failed: " #code); \
}

#define SET_CALL_FLAGS(val) call_frame->is->call_flags = (val)
#define CALL_FLAGS()        call_frame->is->call_flags

#endif

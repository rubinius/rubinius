#ifndef RBX_INSTRUCTIONS_DATA_HPP
#define RBX_INSTRUCTIONS_DATA_HPP

#include <stdint.h>

#include "defines.hpp"
#include "call_frame.hpp"
#include "instructions.hpp"

#include "interpreter/prototypes.hpp"

namespace rubinius {
  namespace instructions {
    struct InstructionData {
      const char* name;
      const int id;
      const int width;
      const int read;
      const int read_arg0;
      const int read_arg1;
      const int read_arg2;
      const int write;
      const int write_arg0;
      const int write_arg1;
      const int write_arg2;
      intptr_t (*interpreter_address)(STATE, CallFrame*, intptr_t const[]);

      int consumed(intptr_t arg0, intptr_t arg1, intptr_t arg2) const {
        return read + (arg0*read_arg0) + (arg1*read_arg1) + (arg2*read_arg2);
      }

      int produced(intptr_t arg0, intptr_t arg1, intptr_t arg2) const {
        return write + (arg0*write_arg0) + (arg1*write_arg1) + (arg2*write_arg2);
      }

      int stack_effect(intptr_t arg0, intptr_t arg1, intptr_t arg2) const {
        return produced(arg0, arg1, arg2) - consumed(arg0, arg1, arg2);
      }
    };

    const InstructionData constexpr data_add_scope = {
      "add_scope", 0, 1, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::add_scope
    };
    const InstructionData constexpr data_allow_private = {
      "allow_private", 1, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::allow_private
    };
    const InstructionData constexpr data_cast_array = {
      "cast_array", 2, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::cast_array
    };
    const InstructionData constexpr data_cast_for_multi_block_arg = {
      "cast_for_multi_block_arg", 3, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::cast_for_multi_block_arg
    };
    const InstructionData constexpr data_cast_for_single_block_arg = {
      "cast_for_single_block_arg", 4, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::cast_for_single_block_arg
    };
    const InstructionData constexpr data_cast_for_splat_block_arg = {
      "cast_for_splat_block_arg", 5, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::cast_for_splat_block_arg
    };
    const InstructionData constexpr data_cast_multi_value = {
      "cast_multi_value", 6, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::cast_multi_value
    };
    const InstructionData constexpr data_check_frozen = {
      "check_frozen", 7, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::check_frozen
    };
    const InstructionData constexpr data_check_interrupts = {
      "check_interrupts", 8, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::check_interrupts
    };
    const InstructionData constexpr data_check_serial = {
      "check_serial", 9, 3, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::check_serial
    };
    const InstructionData constexpr data_check_serial_private = {
      "check_serial_private", 10, 3, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::check_serial_private
    };
    const InstructionData constexpr data_clear_exception = {
      "clear_exception", 11, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::clear_exception
    };
    const InstructionData constexpr data_create_block = {
      "create_block", 12, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::create_block
    };
    const InstructionData constexpr data_dup = {
      "dup", 13, 1, 1, 0, 0, 0, 2, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::dup
    };
    const InstructionData constexpr data_dup_many = {
      "dup_many", 14, 2, 0, 1, 0, 0, 0, (1 * 2), (0 * 2), (0 * 0),
      rubinius::interpreter::dup_many
    };
    const InstructionData constexpr data_ensure_return = {
      "ensure_return", 15, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::ensure_return
    };
    const InstructionData constexpr data_find_const = {
      "find_const", 16, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::find_const
    };
    const InstructionData constexpr data_goto = {
      "goto", 17, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_
    };
    const InstructionData constexpr data_goto_if_equal = {
      "goto_if_equal", 18, 2, 2, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_if_equal
    };
    const InstructionData constexpr data_goto_if_false = {
      "goto_if_false", 19, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_if_false
    };
    const InstructionData constexpr data_goto_if_nil = {
      "goto_if_nil", 20, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_if_nil
    };
    const InstructionData constexpr data_goto_if_not_equal = {
      "goto_if_not_equal", 21, 2, 2, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_if_not_equal
    };
    const InstructionData constexpr data_goto_if_not_nil = {
      "goto_if_not_nil", 22, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_if_not_nil
    };
    const InstructionData constexpr data_goto_if_not_undefined = {
      "goto_if_not_undefined", 23, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_if_not_undefined
    };
    const InstructionData constexpr data_goto_if_true = {
      "goto_if_true", 24, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_if_true
    };
    const InstructionData constexpr data_goto_if_undefined = {
      "goto_if_undefined", 25, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_if_undefined
    };
    const InstructionData constexpr data_instance_of = {
      "instance_of", 26, 1, 2, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::instance_of
    };
    const InstructionData constexpr data_invoke_primitive = {
      "invoke_primitive", 27, 3, 0, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::invoke_primitive
    };
    const InstructionData constexpr data_kind_of = {
      "kind_of", 28, 1, 2, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::kind_of
    };
    const InstructionData constexpr data_make_array = {
      "make_array", 29, 2, 0, 1, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::make_array
    };
    const InstructionData constexpr data_move_down = {
      "move_down", 30, 2, 0, 1, 0, 0, 0, (1 * 1), (0 * 1), (0 * 0),
      rubinius::interpreter::move_down
    };
    const InstructionData constexpr data_noop = {
      "noop", 31, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::noop
    };
    const InstructionData constexpr data_object_to_s = {
      "object_to_s", 32, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::object_to_s
    };
    const InstructionData constexpr data_passed_arg = {
      "passed_arg", 33, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::passed_arg
    };
    const InstructionData constexpr data_passed_blockarg = {
      "passed_blockarg", 34, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::passed_blockarg
    };
    const InstructionData constexpr data_pop = {
      "pop", 35, 1, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::pop
    };
    const InstructionData constexpr data_pop_many = {
      "pop_many", 36, 2, 0, 1, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::pop_many
    };
    const InstructionData constexpr data_pop_unwind = {
      "pop_unwind", 37, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::pop_unwind
    };
    const InstructionData constexpr data_push_block = {
      "push_block", 38, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_block
    };
    const InstructionData constexpr data_push_block_arg = {
      "push_block_arg", 39, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_block_arg
    };
    const InstructionData constexpr data_push_const = {
      "push_const", 40, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_const
    };
    const InstructionData constexpr data_push_cpath_top = {
      "push_cpath_top", 41, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_cpath_top
    };
    const InstructionData constexpr data_push_current_exception = {
      "push_current_exception", 42, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_current_exception
    };
    const InstructionData constexpr data_push_exception_state = {
      "push_exception_state", 43, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_exception_state
    };
    const InstructionData constexpr data_push_false = {
      "push_false", 44, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_false
    };
    const InstructionData constexpr data_push_has_block = {
      "push_has_block", 45, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_has_block
    };
    const InstructionData constexpr data_push_int = {
      "push_int", 46, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_int
    };
    const InstructionData constexpr data_push_ivar = {
      "push_ivar", 47, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_ivar
    };
    const InstructionData constexpr data_push_literal = {
      "push_literal", 48, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_literal
    };
    const InstructionData constexpr data_push_local = {
      "push_local", 49, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_local
    };
    const InstructionData constexpr data_push_local_depth = {
      "push_local_depth", 50, 3, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_local_depth
    };
    const InstructionData constexpr data_push_memo = {
      "push_memo", 51, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_memo
    };
    const InstructionData constexpr data_push_mirror = {
      "push_mirror", 52, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_mirror
    };
    const InstructionData constexpr data_push_my_field = {
      "push_my_field", 53, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_my_field
    };
    const InstructionData constexpr data_push_my_offset = {
      "push_my_offset", 54, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_my_offset
    };
    const InstructionData constexpr data_push_nil = {
      "push_nil", 55, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_nil
    };
    const InstructionData constexpr data_push_proc = {
      "push_proc", 56, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_proc
    };
    const InstructionData constexpr data_push_rubinius = {
      "push_rubinius", 57, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_rubinius
    };
    const InstructionData constexpr data_push_scope = {
      "push_scope", 58, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_scope
    };
    const InstructionData constexpr data_push_self = {
      "push_self", 59, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_self
    };
    const InstructionData constexpr data_push_stack_local = {
      "push_stack_local", 60, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_stack_local
    };
    const InstructionData constexpr data_push_true = {
      "push_true", 61, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_true
    };
    const InstructionData constexpr data_push_type = {
      "push_type", 62, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_type
    };
    const InstructionData constexpr data_push_undef = {
      "push_undef", 63, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_undef
    };
    const InstructionData constexpr data_push_variables = {
      "push_variables", 64, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_variables
    };
    const InstructionData constexpr data_raise_break = {
      "raise_break", 65, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::raise_break
    };
    const InstructionData constexpr data_raise_exc = {
      "raise_exc", 66, 1, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::raise_exc
    };
    const InstructionData constexpr data_raise_return = {
      "raise_return", 67, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::raise_return
    };
    const InstructionData constexpr data_reraise = {
      "reraise", 68, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::reraise
    };
    const InstructionData constexpr data_restore_exception_state = {
      "restore_exception_state", 69, 1, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::restore_exception_state
    };
    const InstructionData constexpr data_ret = {
      "ret", 70, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::ret
    };
    const InstructionData constexpr data_rotate = {
      "rotate", 71, 2, 0, 1, 0, 0, 0, (1 * 1), (0 * 1), (0 * 0),
      rubinius::interpreter::rotate
    };
    const InstructionData constexpr data_send_method = {
      "send_method", 72, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_method
    };
    const InstructionData constexpr data_send_stack = {
      "send_stack", 73, 3, 1, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_stack
    };
    const InstructionData constexpr data_send_stack_with_block = {
      "send_stack_with_block", 74, 3, 2, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_stack_with_block
    };
    const InstructionData constexpr data_send_stack_with_splat = {
      "send_stack_with_splat", 75, 3, 3, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_stack_with_splat
    };
    const InstructionData constexpr data_send_super_stack_with_block = {
      "send_super_stack_with_block", 76, 3, 1, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_super_stack_with_block
    };
    const InstructionData constexpr data_send_super_stack_with_splat = {
      "send_super_stack_with_splat", 77, 3, 2, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_super_stack_with_splat
    };
    const InstructionData constexpr data_send_vcall = {
      "send_vcall", 78, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_vcall
    };
    const InstructionData constexpr data_set_call_flags = {
      "set_call_flags", 79, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_call_flags
    };
    const InstructionData constexpr data_set_const = {
      "set_const", 80, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_const
    };
    const InstructionData constexpr data_set_const_at = {
      "set_const_at", 81, 2, 2, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_const_at
    };
    const InstructionData constexpr data_set_ivar = {
      "set_ivar", 82, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_ivar
    };
    const InstructionData constexpr data_set_local = {
      "set_local", 83, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_local
    };
    const InstructionData constexpr data_set_local_depth = {
      "set_local_depth", 84, 3, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_local_depth
    };
    const InstructionData constexpr data_set_stack_local = {
      "set_stack_local", 85, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_stack_local
    };
    const InstructionData constexpr data_setup_unwind = {
      "setup_unwind", 86, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::setup_unwind
    };
    const InstructionData constexpr data_shift_array = {
      "shift_array", 87, 1, 1, 0, 0, 0, 2, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::shift_array
    };
    const InstructionData constexpr data_store_my_field = {
      "store_my_field", 88, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::store_my_field
    };
    const InstructionData constexpr data_string_append = {
      "string_append", 89, 1, 2, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::string_append
    };
    const InstructionData constexpr data_string_build = {
      "string_build", 90, 2, 0, 1, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::string_build
    };
    const InstructionData constexpr data_string_dup = {
      "string_dup", 91, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::string_dup
    };
    const InstructionData constexpr data_swap = {
      "swap", 92, 1, 2, 0, 0, 0, 2, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::swap
    };
    const InstructionData constexpr data_unwind = {
      "unwind", 93, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::unwind
    };
    const InstructionData constexpr data_yield_debugger = {
      "yield_debugger", 94, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::yield_debugger
    };
    const InstructionData constexpr data_yield_splat = {
      "yield_splat", 95, 2, 1, 1, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::yield_splat
    };
    const InstructionData constexpr data_yield_stack = {
      "yield_stack", 96, 2, 0, 1, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::yield_stack
    };
    const InstructionData constexpr data_zsuper = {
      "zsuper", 97, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::zsuper
    };
    const InstructionData constexpr data_push_file = {
      "push_file", 98, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_file
    };
    const InstructionData constexpr data_p_any = {
      "p_any", 99, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_any
    };
    const InstructionData constexpr data_p_call = {
      "p_call", 100, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_call
    };
    const InstructionData constexpr data_p_char = {
      "p_char", 101, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_char
    };
    const InstructionData constexpr data_p_char_set = {
      "p_char_set", 102, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_char_set
    };
    const InstructionData constexpr data_p_choice = {
      "p_choice", 103, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_choice
    };
    const InstructionData constexpr data_p_commit = {
      "p_commit", 104, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_commit
    };
    const InstructionData constexpr data_p_commit_back = {
      "p_commit_back", 105, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_commit_back
    };
    const InstructionData constexpr data_p_commit_partial = {
      "p_commit_partial", 106, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_commit_partial
    };
    const InstructionData constexpr data_p_end = {
      "p_end", 107, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_end
    };
    const InstructionData constexpr data_p_fail = {
      "p_fail", 108, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_fail
    };
    const InstructionData constexpr data_p_fail_twice = {
      "p_fail_twice", 109, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_fail_twice
    };
    const InstructionData constexpr data_p_jump = {
      "p_jump", 110, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_jump
    };
    const InstructionData constexpr data_p_return = {
      "p_return", 111, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_return
    };
    const InstructionData constexpr data_p_span = {
      "p_span", 112, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_span
    };
    const InstructionData constexpr data_p_test_any = {
      "p_test_any", 113, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_test_any
    };
    const InstructionData constexpr data_p_test_char = {
      "p_test_char", 114, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_test_char
    };
    const InstructionData constexpr data_p_test_char_set = {
      "p_test_char_set", 115, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_test_char_set
    };
    const InstructionData constexpr data_p_init = {
      "p_init", 116, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::p_init
    };
    const InstructionData constexpr data_m_bytes = {
      "m_bytes", 117, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::m_bytes
    };
    const InstructionData constexpr data_m_counter = {
      "m_counter", 118, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::m_counter
    };
    const InstructionData constexpr data_m_sum = {
      "m_sum", 119, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::m_sum
    };
    const InstructionData constexpr data_m_value = {
      "m_value", 120, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::m_value
    };
    const InstructionData constexpr data_m_time_stamp = {
      "m_time_stamp", 121, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::m_time_stamp
    };
    const InstructionData constexpr data_m_timer_start = {
      "m_timer_start", 122, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::m_timer_start
    };
    const InstructionData constexpr data_m_timer_stop = {
      "m_timer_stop", 123, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::m_timer_stop
    };

    const InstructionData constexpr data_b_if_serial = {
      "b_if_serial", 124, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::b_if_serial
    };
    const InstructionData constexpr data_b_if_int = {
      "b_if_int", 125, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::b_if_int
    };
    const InstructionData constexpr data_b_if = {
      "b_if", 126, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::b_if
    };

    const InstructionData constexpr data_r_load_local = {
      "r_load_local", 127, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_local
    };
    const InstructionData constexpr data_r_store_local = {
      "r_store_local", 128, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_store_local
    };
    const InstructionData constexpr data_r_load_local_depth = {
      "r_load_local_depth", 129, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_local_depth
    };
    const InstructionData constexpr data_r_store_local_depth = {
      "r_store_local_depth", 130, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_store_local_depth
    };
    const InstructionData constexpr data_r_load_stack = {
      "r_load_stack", 131, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_stack
    };
    const InstructionData constexpr data_r_store_stack = {
      "r_store_stack", 132, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_store_stack
    };
    const InstructionData constexpr data_r_load_literal = {
      "r_load_literal", 133, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_literal
    };
    const InstructionData constexpr data_r_load_int = {
      "r_load_int", 134, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_int
    };
    const InstructionData constexpr data_r_store_int = {
      "r_store_int", 135, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_store_int
    };
    const InstructionData constexpr data_r_copy = {
      "r_copy", 136, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_copy
    };
    const InstructionData constexpr data_n_iadd = {
      "n_iadd", 137, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_iadd
    };
    const InstructionData constexpr data_n_isub = {
      "n_isub", 138, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_isub
    };
    const InstructionData constexpr data_n_imul = {
      "n_imul", 139, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_imul
    };
    const InstructionData constexpr data_n_idiv = {
      "n_idiv", 140, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_idiv
    };
    const InstructionData constexpr data_n_iadd_o = {
      "n_iadd_o", 141, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_iadd_o
    };
    const InstructionData constexpr data_n_isub_o = {
      "n_isub_o", 142, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_isub_o
    };
    const InstructionData constexpr data_n_imul_o = {
      "n_imul_o", 143, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_imul_o
    };
    const InstructionData constexpr data_n_idiv_o = {
      "n_idiv_o", 144, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_idiv_o
    };
    const InstructionData constexpr data_n_ieq = {
      "n_ieq", 145, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ieq
    };
    const InstructionData constexpr data_n_ine = {
      "n_ine", 146, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ine
    };
    const InstructionData constexpr data_n_ilt = {
      "n_ilt", 147, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ilt
    };
    const InstructionData constexpr data_n_ile = {
      "n_ile", 148, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ile
    };
    const InstructionData constexpr data_n_igt = {
      "n_igt", 149, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_igt
    };
    const InstructionData constexpr data_n_ige = {
      "n_ige", 150, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ige
    };
    const InstructionData constexpr data_n_ipopcnt = {
      "n_ipopcnt", 151, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ipopcnt
    };
    const InstructionData constexpr data_m_log = {
      "m_log", 152, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::m_log
    };
    const InstructionData constexpr data_m_debug = {
      "m_debug", 153, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::m_debug
    };
    const InstructionData constexpr data_e_cache_method_p = {
      "e_cache_method_p", 154, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_cache_method_p
    };
    const InstructionData constexpr data_e_cache_function_p = {
      "e_cache_function_p", 155, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_cache_function_p
    };
    const InstructionData constexpr data_e_cache_value_p = {
      "e_cache_value_p", 156, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_cache_value_p
    };
    const InstructionData constexpr data_e_cache_method = {
      "e_cache_method", 157, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_cache_method
    };
    const InstructionData constexpr data_e_cache_function = {
      "e_cache_function", 158, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_cache_function
    };
    const InstructionData constexpr data_e_cache_value = {
      "e_cache_value", 159, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_cache_value
    };
    const InstructionData constexpr data_e_resolve_method = {
      "e_resolve_method", 160, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_resolve_method
    };
    const InstructionData constexpr data_e_resolve_receiver_method = {
      "e_resolve_receiver_method", 161, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_resolve_receiver_method
    };
    const InstructionData constexpr data_e_resolve_function = {
      "e_resolve_function", 162, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_resolve_function
    };
    const InstructionData constexpr data_e_resolve_scope_constant = {
      "e_resolve_scope_constant", 163, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_resolve_scope_constant
    };
    const InstructionData constexpr data_e_resolve_path_constant = {
      "e_resolve_path_constant", 164, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_resolve_path_constant
    };
    const InstructionData constexpr data_e_signature = {
      "e_signature", 165, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_signature
    };
    const InstructionData constexpr data_e_check_signature = {
      "e_check_signature", 166, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_check_signature
    };
    const InstructionData constexpr data_e_invoke_method = {
      "e_invoke_method", 167, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_invoke_method
    };
    const InstructionData constexpr data_e_invoke_function = {
      "e_invoke_function", 168, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::e_invoke_function
    };
    const InstructionData constexpr data_a_instance = {
      "a_instance", 169, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_instance
    };
    const InstructionData constexpr data_a_kind = {
      "a_kind", 170, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_kind
    };
    const InstructionData constexpr data_a_method = {
      "a_method", 171, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_method
    };
    const InstructionData constexpr data_a_receiver_method = {
      "a_receiver_method", 172, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_receiver_method
    };
    const InstructionData constexpr data_a_type = {
      "a_type", 173, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_type
    };
    const InstructionData constexpr data_a_function = {
      "a_function", 174, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_function
    };
    const InstructionData constexpr data_a_equal = {
      "a_equal", 175, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_equal
    };
    const InstructionData constexpr data_a_not_equal = {
      "a_not_equal", 176, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_not_equal
    };
    const InstructionData constexpr data_a_less = {
      "a_less", 177, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_less
    };
    const InstructionData constexpr data_a_less_equal = {
      "a_less_equal", 178, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_less_equal
    };
    const InstructionData constexpr data_a_greater = {
      "a_greater", 179, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_greater
    };
    const InstructionData constexpr data_a_greater_equal = {
      "a_greater_equal", 180, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::a_greater_equal
    };
    const InstructionData constexpr data_goto_past = {
      "goto_past", 181, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_past
    };
    const InstructionData constexpr data_goto_future = {
      "goto_future", 182, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::goto_future
    };
    const InstructionData constexpr data_r_load_0 = {
      "r_load_0", 183, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_0
    };
    const InstructionData constexpr data_r_load_1 = {
      "r_load_1", 184, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_1
    };
    const InstructionData constexpr data_r_load_nil = {
      "r_load_nil", 185, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_nil
    };
    const InstructionData constexpr data_r_load_false = {
      "r_load_false", 186, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_false
    };
    const InstructionData constexpr data_r_load_true = {
      "r_load_true", 187, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_true
    };
    const InstructionData constexpr data_call_send = {
      "call_send", 188, 3, 1, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::call_send
    };
    const InstructionData constexpr data_call = {
      "call", 189, 3, 1, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::call
    };
    const InstructionData constexpr data_call_0 = {
      "call_0", 190, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::call_0
    };
    const InstructionData constexpr data_push_tagged_nil = {
      "push_tagged_nil", 191, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::push_tagged_nil
    };
    const InstructionData constexpr data_r_load_bool = {
      "r_load_bool", 192, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_bool
    };
    const InstructionData constexpr data_r_load_m_binops = {
      "r_load_m_binops", 193, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_m_binops
    };
    const InstructionData constexpr data_r_load_f_binops = {
      "r_load_f_binops", 194, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_f_binops
    };
    const InstructionData constexpr data_r_ret = {
      "r_ret", 195, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_ret
    };
    const InstructionData constexpr data_n_imod = {
      "data_n_imod", 196, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_imod
    };
    const InstructionData constexpr data_n_ineg = {
      "data_n_ineg", 197, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ineg
    };
    const InstructionData constexpr data_n_inot = {
      "data_n_inot", 198, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_inot
    };
    const InstructionData constexpr data_n_iand = {
      "data_n_iand", 199, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_iand
    };
    const InstructionData constexpr data_n_ior = {
      "data_n_ior", 200, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ior
    };
    const InstructionData constexpr data_n_ixor = {
      "data_n_ixor", 201, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ixor
    };
    const InstructionData constexpr data_n_ishl = {
      "data_n_ishl", 202, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ishl
    };
    const InstructionData constexpr data_n_ishr = {
      "data_n_ishr", 203, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ishr
    };
    const InstructionData constexpr data_n_imod_o = {
      "data_n_imod_o", 204, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_imod_o
    };
    const InstructionData constexpr data_n_ineg_o = {
      "data_n_ineg_o", 205, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ineg_o
    };
    const InstructionData constexpr data_n_ishl_o = {
      "data_n_ishl_o", 206, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ishl_o
    };
    const InstructionData constexpr data_n_ishr_o = {
      "data_n_ishr_o", 207, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ishr_o
    };
    const InstructionData constexpr data_n_promote = {
      "data_n_promote", 208, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_promote
    };
    const InstructionData constexpr data_n_eadd = {
      "n_eadd", 209, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_eadd
    };
    const InstructionData constexpr data_n_esub = {
      "n_esub", 210, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_esub
    };
    const InstructionData constexpr data_n_emul = {
      "n_emul", 211, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_emul
    };
    const InstructionData constexpr data_n_ediv = {
      "n_ediv", 212, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ediv
    };
    const InstructionData constexpr data_n_emod = {
      "data_n_emod", 213, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_emod
    };
    const InstructionData constexpr data_n_eneg = {
      "data_n_eneg", 214, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_eneg
    };
    const InstructionData constexpr data_n_enot = {
      "data_n_enot", 215, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_enot
    };
    const InstructionData constexpr data_n_eand = {
      "data_n_eand", 216, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_eand
    };
    const InstructionData constexpr data_n_eor = {
      "data_n_eor", 217, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_eor
    };
    const InstructionData constexpr data_n_exor = {
      "data_n_exor", 218, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_exor
    };
    const InstructionData constexpr data_n_eshl = {
      "data_n_eshl", 219, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_eshl
    };
    const InstructionData constexpr data_n_eshr = {
      "data_n_eshr", 220, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_eshr
    };
    const InstructionData constexpr data_n_epopcnt = {
      "n_epopcnt", 221, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_epopcnt
    };
    const InstructionData constexpr data_n_eeq = {
      "n_eeq", 222, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_eeq
    };
    const InstructionData constexpr data_n_ene = {
      "n_ene", 223, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ene
    };
    const InstructionData constexpr data_n_elt = {
      "n_elt", 224, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_elt
    };
    const InstructionData constexpr data_n_ele = {
      "n_ele", 225, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ele
    };
    const InstructionData constexpr data_n_egt = {
      "n_egt", 226, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_egt
    };
    const InstructionData constexpr data_n_ege = {
      "n_ege", 227, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ege
    };
    const InstructionData constexpr data_n_dadd = {
      "n_dadd", 228, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dadd
    };
    const InstructionData constexpr data_n_dsub = {
      "n_dsub", 229, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dsub
    };
    const InstructionData constexpr data_n_dmul = {
      "n_dmul", 230, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dmul
    };
    const InstructionData constexpr data_n_ddiv = {
      "n_ddiv", 231, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ddiv
    };
    const InstructionData constexpr data_n_dmod = {
      "data_n_dmod", 232, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dmod
    };
    const InstructionData constexpr data_n_dneg = {
      "data_n_dneg", 233, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dneg
    };
    const InstructionData constexpr data_n_deq = {
      "n_deq", 234, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_deq
    };
    const InstructionData constexpr data_n_dne = {
      "n_dne", 235, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dne
    };
    const InstructionData constexpr data_n_dlt = {
      "n_dlt", 236, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dlt
    };
    const InstructionData constexpr data_n_dle = {
      "n_dle", 237, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dle
    };
    const InstructionData constexpr data_n_dgt = {
      "n_dgt", 238, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dgt
    };
    const InstructionData constexpr data_n_dge = {
      "n_dge", 239, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dge
    };
    const InstructionData constexpr data_r_load_float = {
      "r_load_float", 240, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_float
    };
    const InstructionData constexpr data_r_store_float = {
      "r_store_float", 241, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_store_float
    };
    const InstructionData constexpr data_b_if_eint = {
      "b_if_eint", 242, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::b_if_eint
    };
    const InstructionData constexpr data_b_if_float = {
      "b_if_float", 243, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::b_if_float
    };
    const InstructionData constexpr data_r_load_2 = {
      "r_load_2", 244, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_2
    };
    const InstructionData constexpr data_r_load_neg1 = {
      "r_load_neg1", 245, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_neg1
    };
    const InstructionData constexpr data_n_iinc = {
      "n_iinc", 246, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_iinc
    };
    const InstructionData constexpr data_n_idec = {
      "n_idec", 247, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_idec
    };
    const InstructionData constexpr data_n_isize = {
      "n_isize", 248, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_isize
    };
    const InstructionData constexpr data_n_esize = {
      "n_esize", 249, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_esize
    };
    const InstructionData constexpr data_n_ibits = {
      "n_ibits", 250, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ibits
    };
    const InstructionData constexpr data_n_ebits = {
      "n_ebits", 251, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ebits
    };
    const InstructionData constexpr data_r_load_self = {
      "r_load_self", 252, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_self
    };
    const InstructionData constexpr data_n_istr = {
      "n_istr", 253, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_istr
    };
    const InstructionData constexpr data_n_iflt = {
      "n_iflt", 254, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_iflt
    };
    const InstructionData constexpr data_n_icmp = {
      "n_icmp", 255, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_icmp
    };
    const InstructionData constexpr data_n_idivmod = {
      "n_idivmod", 256, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_idivmod
    };
    const InstructionData constexpr data_n_ipow_o = {
      "n_ipow_o", 257, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ipow_o
    };
    const InstructionData constexpr data_n_estr = {
      "n_estr", 258, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_estr
    };
    const InstructionData constexpr data_n_eflt = {
      "n_eflt", 259, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_eflt
    };
    const InstructionData constexpr data_n_ecmp = {
      "n_ecmp", 260, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ecmp
    };
    const InstructionData constexpr data_n_edivmod = {
      "n_edivmod", 261, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_edivmod
    };
    const InstructionData constexpr data_n_epow = {
      "n_epow", 262, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_epow
    };
    const InstructionData constexpr data_n_dstr = {
      "n_dstr", 263, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dstr
    };
    const InstructionData constexpr data_n_dcmp = {
      "n_dcmp", 264, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dcmp
    };
    const InstructionData constexpr data_n_ddivmod = {
      "n_ddivmod", 265, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_ddivmod
    };
    const InstructionData constexpr data_n_dpow = {
      "n_dpow", 266, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dpow
    };
    const InstructionData constexpr data_n_demote = {
      "n_demote", 267, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_demote
    };
    const InstructionData constexpr data_n_dinf = {
      "n_dinf", 268, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dinf
    };
    const InstructionData constexpr data_n_dnan = {
      "n_dnan", 269, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dnan
    };
    const InstructionData constexpr data_n_dclass = {
      "n_dclass", 270, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dclass
    };
    const InstructionData constexpr data_n_dsign = {
      "n_dsign", 271, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::n_dsign
    };
    const InstructionData constexpr data_r_load_ref_addr = {
      "r_load_ref_addr", 272, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_ref_addr
    };
    const InstructionData constexpr data_r_load_byte_addr = {
      "r_load_byte_addr", 273, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_byte_addr
    };
    const InstructionData constexpr data_r_load_ref = {
      "r_load_ref", 274, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_ref
    };
    const InstructionData constexpr data_r_store_ref = {
      "r_store_ref", 275, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_store_ref
    };
    const InstructionData constexpr data_r_load_byte = {
      "r_load_byte", 276, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_byte
    };
    const InstructionData constexpr data_r_store_byte = {
      "r_store_byte", 277, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_store_byte
    };
    const InstructionData constexpr data_r_load_handle = {
      "r_load_handle", 278, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_handle
    };
    const InstructionData constexpr data_r_store_handle = {
      "r_store_handle", 279, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_store_handle
    };
    const InstructionData constexpr data_r_load_ivar = {
      "r_load_ivar", 280, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_ivar
    };
    const InstructionData constexpr data_r_store_ivar = {
      "r_store_ivar", 281, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_store_ivar
    };
    const InstructionData constexpr data_r_load_index = {
      "r_load_index", 282, 4, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_load_index
    };
    const InstructionData constexpr data_r_refcnt_inc = {
      "r_refcnt_inc", 283, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_refcnt_inc
    };
    const InstructionData constexpr data_r_refcnt_dec = {
      "r_refcnt_dec", 284, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::r_refcnt_dec
    };
  }

  struct Instructions {
    static const instructions::InstructionData data[];

    static const instructions::InstructionData& instruction_data(int index) {
      return data[index];
    }
  };
}

#endif

#ifndef RBX_INSTRUCTIONS_DATA_HPP
#define RBX_INSTRUCTIONS_DATA_HPP

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
      "ret", 70, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::ret
    };
    const InstructionData constexpr data_rotate = {
      "rotate", 71, 2, 0, 1, 0, 0, 0, (1 * 1), (0 * 1), (0 * 0),
      rubinius::interpreter::rotate
    };
    const InstructionData constexpr data_run_exception = {
      "run_exception", 72, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::run_exception
    };
    const InstructionData constexpr data_send_method = {
      "send_method", 73, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_method
    };
    const InstructionData constexpr data_send_stack = {
      "send_stack", 74, 3, 1, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_stack
    };
    const InstructionData constexpr data_send_stack_with_block = {
      "send_stack_with_block", 75, 3, 2, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_stack_with_block
    };
    const InstructionData constexpr data_send_stack_with_splat = {
      "send_stack_with_splat", 76, 3, 3, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_stack_with_splat
    };
    const InstructionData constexpr data_send_super_stack_with_block = {
      "send_super_stack_with_block", 77, 3, 1, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_super_stack_with_block
    };
    const InstructionData constexpr data_send_super_stack_with_splat = {
      "send_super_stack_with_splat", 78, 3, 2, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_super_stack_with_splat
    };
    const InstructionData constexpr data_send_vcall = {
      "send_vcall", 79, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::send_vcall
    };
    const InstructionData constexpr data_set_call_flags = {
      "set_call_flags", 80, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_call_flags
    };
    const InstructionData constexpr data_set_const = {
      "set_const", 81, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_const
    };
    const InstructionData constexpr data_set_const_at = {
      "set_const_at", 82, 2, 2, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_const_at
    };
    const InstructionData constexpr data_set_ivar = {
      "set_ivar", 83, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_ivar
    };
    const InstructionData constexpr data_set_local = {
      "set_local", 84, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_local
    };
    const InstructionData constexpr data_set_local_depth = {
      "set_local_depth", 85, 3, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_local_depth
    };
    const InstructionData constexpr data_set_stack_local = {
      "set_stack_local", 86, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::set_stack_local
    };
    const InstructionData constexpr data_setup_unwind = {
      "setup_unwind", 87, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::setup_unwind
    };
    const InstructionData constexpr data_shift_array = {
      "shift_array", 88, 1, 1, 0, 0, 0, 2, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::shift_array
    };
    const InstructionData constexpr data_store_my_field = {
      "store_my_field", 89, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::store_my_field
    };
    const InstructionData constexpr data_string_append = {
      "string_append", 90, 1, 2, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::string_append
    };
    const InstructionData constexpr data_string_build = {
      "string_build", 91, 2, 0, 1, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::string_build
    };
    const InstructionData constexpr data_string_dup = {
      "string_dup", 92, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::string_dup
    };
    const InstructionData constexpr data_swap = {
      "swap", 93, 1, 2, 0, 0, 0, 2, (0 * 0), (0 * 0), (0 * 0),
      rubinius::interpreter::swap
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
  }
}

#endif

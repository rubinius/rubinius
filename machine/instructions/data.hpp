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
      const intptr_t interpreter_address;

      int consumed(intptr_t arg0, intptr_t arg1, intptr_t arg2) {
        return read + (arg0*read_arg0) + (arg1*read_arg1) + (arg2*read_arg2);
      }

      int produced(intptr_t arg0, intptr_t arg1, intptr_t arg2) {
        return write + (arg0*write_arg0) + (arg1*write_arg1) + (arg2*write_arg2);
      }

      int stack_effect(intptr_t arg0, intptr_t arg1, intptr_t arg2) {
        return produced(arg0, arg1, arg2) - consumed(arg0, arg1, arg2);
      }
    };

    const InstructionData data_noop = {
      "noop", 0, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::noop
    };
    const InstructionData data_push_nil = {
      "push_nil", 1, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_nil
    };
    const InstructionData data_push_true = {
      "push_true", 2, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_true
    };
    const InstructionData data_push_false = {
      "push_false", 3, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_false
    };
    const InstructionData data_push_int = {
      "push_int", 4, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_int
    };
    const InstructionData data_push_self = {
      "push_self", 5, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_self
    };
    const InstructionData data_push_memo = {
      "push_memo", 6, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_memo
    };
    const InstructionData data_push_literal = {
      "push_literal", 7, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_literal
    };
    const InstructionData data_goto = {
      "goto", 8, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::goto_
    };
    const InstructionData data_goto_if_false = {
      "goto_if_false", 9, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::goto_if_false
    };
    const InstructionData data_goto_if_true = {
      "goto_if_true", 10, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::goto_if_true
    };
    const InstructionData data_goto_if_nil = {
      "goto_if_nil", 11, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::goto_if_nil
    };
    const InstructionData data_goto_if_not_nil = {
      "goto_if_not_nil", 12, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::goto_if_not_nil
    };
    const InstructionData data_goto_if_undefined = {
      "goto_if_undefined", 13, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::goto_if_undefined
    };
    const InstructionData data_goto_if_not_undefined = {
      "goto_if_not_undefined", 14, 2, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::goto_if_not_undefined
    };
    const InstructionData data_goto_if_equal = {
      "goto_if_equal", 15, 2, 2, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::goto_if_equal
    };
    const InstructionData data_goto_if_not_equal = {
      "goto_if_not_equal", 16, 2, 2, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::goto_if_not_equal
    };
    const InstructionData data_ret = {
      "ret", 17, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::ret
    };
    const InstructionData data_swap = {
      "swap", 18, 1, 2, 0, 0, 0, 2, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::swap
    };
    const InstructionData data_dup = {
      "dup", 19, 1, 1, 0, 0, 0, 2, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::dup
    };
    const InstructionData data_dup_many = {
      "dup_many", 20, 2, 0, 1, 0, 0, 0, (1 * 2), (0 * 2), (0 * 0),
      (intptr_t)rubinius::interpreter::dup_many
    };
    const InstructionData data_pop = {
      "pop", 21, 1, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::pop
    };
    const InstructionData data_pop_many = {
      "pop_many", 22, 2, 0, 1, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::pop_many
    };
    const InstructionData data_rotate = {
      "rotate", 23, 2, 0, 1, 0, 0, 0, (1 * 1), (0 * 1), (0 * 0),
      (intptr_t)rubinius::interpreter::rotate
    };
    const InstructionData data_move_down = {
      "move_down", 24, 2, 0, 1, 0, 0, 0, (1 * 1), (0 * 1), (0 * 0),
      (intptr_t)rubinius::interpreter::move_down
    };
    const InstructionData data_set_local = {
      "set_local", 25, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::set_local
    };
    const InstructionData data_push_local = {
      "push_local", 26, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_local
    };
    const InstructionData data_push_local_depth = {
      "push_local_depth", 27, 3, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_local_depth
    };
    const InstructionData data_set_local_depth = {
      "set_local_depth", 28, 3, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::set_local_depth
    };
    const InstructionData data_passed_arg = {
      "passed_arg", 29, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::passed_arg
    };
    const InstructionData data_push_current_exception = {
      "push_current_exception", 30, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_current_exception
    };
    const InstructionData data_clear_exception = {
      "clear_exception", 31, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::clear_exception
    };
    const InstructionData data_push_exception_state = {
      "push_exception_state", 32, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_exception_state
    };
    const InstructionData data_restore_exception_state = {
      "restore_exception_state", 33, 1, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::restore_exception_state
    };
    const InstructionData data_raise_exc = {
      "raise_exc", 34, 1, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::raise_exc
    };
    const InstructionData data_setup_unwind = {
      "setup_unwind", 35, 3, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::setup_unwind
    };
    const InstructionData data_pop_unwind = {
      "pop_unwind", 36, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::pop_unwind
    };
    const InstructionData data_raise_return = {
      "raise_return", 37, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::raise_return
    };
    const InstructionData data_ensure_return = {
      "ensure_return", 38, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::ensure_return
    };
    const InstructionData data_raise_break = {
      "raise_break", 39, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::raise_break
    };
    const InstructionData data_reraise = {
      "reraise", 40, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::reraise
    };
    const InstructionData data_make_array = {
      "make_array", 41, 2, 0, 1, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::make_array
    };
    const InstructionData data_cast_array = {
      "cast_array", 42, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::cast_array
    };
    const InstructionData data_shift_array = {
      "shift_array", 43, 1, 1, 0, 0, 0, 2, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::shift_array
    };
    const InstructionData data_set_ivar = {
      "set_ivar", 44, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::set_ivar
    };
    const InstructionData data_push_ivar = {
      "push_ivar", 45, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_ivar
    };
    const InstructionData data_set_const = {
      "set_const", 46, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::set_const
    };
    const InstructionData data_set_const_at = {
      "set_const_at", 47, 2, 2, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::set_const_at
    };
    const InstructionData data_push_cpath_top = {
      "push_cpath_top", 48, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_cpath_top
    };
    const InstructionData data_push_const = {
      "push_const", 49, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_const
    };
    const InstructionData data_find_const = {
      "find_const", 50, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::find_const
    };
    const InstructionData data_set_call_flags = {
      "set_call_flags", 51, 2, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::set_call_flags
    };
    const InstructionData data_allow_private = {
      "allow_private", 52, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::allow_private
    };
    const InstructionData data_send_vcall = {
      "send_vcall", 53, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::send_vcall
    };
    const InstructionData data_send_method = {
      "send_method", 54, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::send_method
    };
    const InstructionData data_send_stack = {
      "send_stack", 55, 3, 1, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::send_stack
    };
    const InstructionData data_send_stack_with_block = {
      "send_stack_with_block", 56, 3, 2, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::send_stack_with_block
    };
    const InstructionData data_send_stack_with_splat = {
      "send_stack_with_splat", 57, 3, 3, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::send_stack_with_splat
    };
    const InstructionData data_send_super_stack_with_block = {
      "send_super_stack_with_block", 58, 3, 1, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::send_super_stack_with_block
    };
    const InstructionData data_send_super_stack_with_splat = {
      "send_super_stack_with_splat", 59, 3, 2, 0, 1, 0, 1, (0 * 0), (0 * 0),
      (0 * 0), (intptr_t)rubinius::interpreter::send_super_stack_with_splat
    };
    const InstructionData data_push_block = {
      "push_block", 60, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_block
    };
    const InstructionData data_passed_blockarg = {
      "passed_blockarg", 61, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::passed_blockarg
    };
    const InstructionData data_create_block = {
      "create_block", 62, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::create_block
    };
    const InstructionData data_cast_for_single_block_arg = {
      "cast_for_single_block_arg", 63, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::cast_for_single_block_arg
    };
    const InstructionData data_cast_for_multi_block_arg = {
      "cast_for_multi_block_arg", 64, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::cast_for_multi_block_arg
    };
    const InstructionData data_cast_for_splat_block_arg = {
      "cast_for_splat_block_arg", 65, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::cast_for_splat_block_arg
    };
    const InstructionData data_yield_stack = {
      "yield_stack", 66, 2, 0, 1, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::yield_stack
    };
    const InstructionData data_yield_splat = {
      "yield_splat", 67, 2, 1, 1, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::yield_splat
    };
    const InstructionData data_string_append = {
      "string_append", 68, 1, 2, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::string_append
    };
    const InstructionData data_string_build = {
      "string_build", 69, 2, 0, 1, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::string_build
    };
    const InstructionData data_string_dup = {
      "string_dup", 70, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::string_dup
    };
    const InstructionData data_push_scope = {
      "push_scope", 71, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_scope
    };
    const InstructionData data_add_scope = {
      "add_scope", 72, 1, 1, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::add_scope
    };
    const InstructionData data_push_variables = {
      "push_variables", 73, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_variables
    };
    const InstructionData data_check_interrupts = {
      "check_interrupts", 74, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::check_interrupts
    };
    const InstructionData data_yield_debugger = {
      "yield_debugger", 75, 1, 0, 0, 0, 0, 0, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::yield_debugger
    };
    const InstructionData data_check_serial = {
      "check_serial", 76, 3, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::check_serial
    };
    const InstructionData data_check_serial_private = {
      "check_serial_private", 77, 3, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::check_serial_private
    };
    const InstructionData data_push_my_field = {
      "push_my_field", 78, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_my_field
    };
    const InstructionData data_store_my_field = {
      "store_my_field", 79, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::store_my_field
    };
    const InstructionData data_kind_of = {
      "kind_of", 80, 1, 2, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::kind_of
    };
    const InstructionData data_instance_of = {
      "instance_of", 81, 1, 2, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::instance_of
    };
    const InstructionData data_push_my_offset = {
      "push_my_offset", 82, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_my_offset
    };
    const InstructionData data_zsuper = {
      "zsuper", 83, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::zsuper
    };
    const InstructionData data_push_block_arg = {
      "push_block_arg", 84, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_block_arg
    };
    const InstructionData data_push_undef = {
      "push_undef", 85, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_undef
    };
    const InstructionData data_push_stack_local = {
      "push_stack_local", 86, 2, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_stack_local
    };
    const InstructionData data_set_stack_local = {
      "set_stack_local", 87, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::set_stack_local
    };
    const InstructionData data_push_has_block = {
      "push_has_block", 88, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_has_block
    };
    const InstructionData data_push_proc = {
      "push_proc", 89, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_proc
    };
    const InstructionData data_check_frozen = {
      "check_frozen", 90, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::check_frozen
    };
    const InstructionData data_cast_multi_value = {
      "cast_multi_value", 91, 1, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::cast_multi_value
    };
    const InstructionData data_invoke_primitive = {
      "invoke_primitive", 92, 3, 0, 0, 1, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::invoke_primitive
    };
    const InstructionData data_push_rubinius = {
      "push_rubinius", 93, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_rubinius
    };
    const InstructionData data_object_to_s = {
      "object_to_s", 94, 2, 1, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::object_to_s
    };
    const InstructionData data_push_type = {
      "push_type", 95, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_type
    };
    const InstructionData data_push_mirror = {
      "push_mirror", 96, 1, 0, 0, 0, 0, 1, (0 * 0), (0 * 0), (0 * 0),
      (intptr_t)rubinius::interpreter::push_mirror
    };
  }
}

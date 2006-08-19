require 'rubygems'
require 'test/unit'
require 'cpu/instructions'
require 'cpu/runtime'
require 'bytecode/encoder'
require 'test/unit/show_code'

class TestInstructionEncoder < Test::Unit::TestCase
  
  def setup
    @encoder = Bytecode::InstructionEncoder.new  
  end
  
  def decI(str, count=1)
    str.unpack("C" + ("I" * count))
  end
  
  def enc(*args)
    @encoder.encode *args
  end
  
  def assert_is_op(op, code)
    assert_equal Bytecode::InstructionEncoder::OpCodes[code], op, "#{code} is not #{op}"
  end
  
  def assert_is_op_only(op, bytes)
    assert_equal 1, bytes.size, "More than just #{op}."
    assert_is_op op, bytes[0]
  end
  
  def assert_op(op)
    assert_is_op_only op, enc(op)
  end
  
  def test_noop
    bytes = @encoder.encode :noop
    assert bytes[0] == 0
  end
  
  def test_push_nil
    assert_op :push_nil
  end
  
  def test_push_true
    assert_op :push_true
  end
  
  def test_push_false
    assert_op :push_false
  end
  
  def test_allocate
    bytes = @encoder.encode :allocate, 4
    assert_equal 6, bytes.size
    codes = decI bytes
    assert_is_op :push_int, codes[0]
    assert_equal 4, codes[1]
    assert_is_op :allocate, bytes[5]
  end
  
  def test_set_class
    bytes = @encoder.encode :set_class
    assert_equal 1, bytes.size
    codes = decI bytes
    assert_is_op :set_class, codes[0]
  end
  
  def test_find_field
    bytes = @encoder.encode :find_field, 1
    assert_equal 5, bytes.size
    codes = decI bytes
    assert_is_op :find_field, codes[0]
    assert codes[1] == 1
  end
  
  def test_store_into
    bytes = @encoder.encode :store_into
    assert_equal 1, bytes.size
    assert_is_op :store_into, bytes[0]
  end
  
  def test_push_int
    bytes = @encoder.encode :push_int, 88
    assert_equal 5, bytes.size
    codes = decI bytes
    assert_is_op :push_int, bytes[0]
    assert_equal 88, codes[1]
  end
  
  def test_store_field
    bytes = @encoder.encode :store_field
    assert_equal 1, bytes.size
    assert_is_op :store_field, bytes[0]
  end
  
  def test_store_field_at
    bytes = @encoder.encode :store_field_at, 354
    assert_equal 6, bytes.size
    codes = decI bytes
    assert_is_op :push_int, bytes[0]
    assert_equal 354, codes[1]
    assert_is_op :store_field, bytes[5]
  end

  def test_fetch_field
    assert_op :fetch_field
  end
  
  def test_send_primitive
    bytes = @encoder.encode :send_primitive, 88
    assert_equal 5, bytes.size
    codes = decI bytes
    assert_is_op :send_primitive, bytes[0]
    assert_equal 88, codes[1]
  end
  
  def test_push_context
    assert_op :push_context
  end
  
  def assert_op_and_int(op, int)
    bytes = @encoder.encode op, int
    assert_equal 5, bytes.size
    codes = decI bytes
    assert_is_op op, bytes[0]
    assert_equal int, codes[1]
  end
  
  def assert_op_and_2ints(op, int, int2)
    bytes = @encoder.encode op, int, int2
    assert_equal 9, bytes.size
    codes = decI bytes, 2
    assert_is_op op, bytes[0]
    assert_equal int, codes[1]
    assert_equal int2, codes[2]
  end
  
  def test_push_literal
    bytes = @encoder.encode :push_literal, 2
    assert_equal 5, bytes.size
    codes = decI bytes
    assert_is_op :push_literal, bytes[0]
    assert_equal 2, codes[1]
  end
  
  def test_push_self
    assert_op :push_self
  end
  
  def test_goto
    assert_op_and_int :goto, 6
  end
  
  def test_goto_if_false
    assert_op_and_int :goto_if_false, 6
  end
  
  def test_goto_if_true
    assert_op_and_int :goto_if_true, 6
  end
  
  def test_goto_if_defined
    assert_op_and_int :goto_if_defined, 8
  end
  
  def test_swap_stack
    assert_op :swap_stack
  end
  
  def test_set_local
    assert_op_and_int :set_local, 2
  end
  
  def test_push_local
    assert_op_and_int :push_local, 2
  end
  
  def test_push_exception
    assert_op :push_exception
  end
  
  def test_make_array
    assert_op_and_int :make_array, 20
  end
  
  def test_make_hash
    assert_op_and_int :make_hash, 20
  end
  
  def test_set_ivar
    assert_op_and_int :set_ivar, 12
  end
  
  def test_push_ivar
    assert_op_and_int :push_ivar, 12
  end
  
  def test_push_const
    assert_op_and_int :push_const, 88
  end
  
  def test_set_const
    assert_op_and_int :set_const, 103
  end
  
  def test_find_const
    assert_op_and_int :find_const, 103
  end
  
  def test_attach_method
    assert_op_and_int :attach_method, 99
  end
  
  def test_open_class
    assert_op_and_int :open_class, 32423
  end
  
  def test_open_class_under
    assert_op_and_int :open_class_under, 32423
  end
  
  def test_open_module
    assert_op_and_int :open_module, 97843
  end
  
  def test_open_module
    assert_op_and_int :open_module_under, 97843
  end
  
  def test_add_method
    assert_op_and_int :add_method, 83
  end
  
  def test_dup_top
    assert_op :dup_top
  end
  
  def test_pop
    assert_op :pop
  end
  
  def test_send_method
    assert_op_and_int :send_method, 12
  end
  
  def test_send_stack
    assert_op_and_2ints :send_stack, 12, 9
  end
  
  def test_send_stack_with_block
    assert_op_and_2ints :send_stack_with_block, 12, 9
  end
  
  def test_activate_method
    assert_op_and_int :activate_method, 3
  end
  
  def test_return
    assert_op :ret
  end
  
  def test_push_block
    assert_op :push_block
  end
  
  def test_clear_exception
    assert_op :clear_exception
  end
  
  def test_soft_return
    assert_op :soft_return
  end
  
  def test_caller_return
    assert_op :caller_return
  end
  
  def test_push_array
    assert_op :push_array
  end
  
  def test_cast_array
    assert_op :cast_array
  end
  
  def test_raise_exc
    assert_op :raise_exc
  end
  
  def test_unshift_tuple
    assert_op :unshift_tuple
  end
  
  def test_cast_tuple
    assert_op :cast_tuple
  end
  
  def test_make_rest
    assert_op_and_int :make_rest, 3
  end
  
  def test_set_encloser
    assert_op :set_encloser
  end
  
  def test_push_encloser
    assert_op :push_encloser
  end
  
  def test_push_cpath_top
    assert_op :push_cpath_top
  end
  
  def test_check_argcount
    assert_op_and_2ints :check_argcount, 99, 12
  end
  
  def test_passed_arg
    assert_op_and_int :passed_arg, 33
  end
end

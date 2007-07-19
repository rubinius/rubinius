module Bytecode
class InstructionEncoder
  
  class InvalidOpCode < RuntimeError
  end

=begin
  StackChanges = {
    :noop =>        [0, 0],
    :push_nil =>    [0, 1],
    :push_true =>   [0, 1],
    :push_false =>  [0, 1],
    :allocate =>    [1, 1],
    :set_class =>   [2, 1],
    :store_field => [3, 1],
    :push_int =>    [0, 1],
    :fetch_field => [2, 1],
    :send_primitive => proc { |a| [a[1], 1] },
    :push_context => [0, 1],
    :push_literal => [0, 1],
    :push_self =>   [0, 1],
    :goto =>        [0, 0],
    :goto_if_false => [1, 0],
    :goto_if_true =>  [1, 0],
    :swap_stack =>  [2, 2],
    :set_local =>   [1, 1],
    :push_local =>  [0, 1],
    :push_exception => [0, 1],
    :make_array =>  proc { |a| [a[0], 1] },
    :set_ivar => [1, 1],
    :push_ivar => [0, 1],
    :goto_if_defined => [1, 0],
    :push_const =>  [0, 1],
    :set_const =>   [1, 1],
    :set_const_at =>  [2, 0],
    :find_const =>    [1, 1],
    :attach_method => [2, 1],
    :add_method =>    [2, 1],
    :open_class =>    [1, 1],
    :open_class_under => [2, 1],
    :open_module =>   [0, 1],
    :open_module_under => [1, 1],
    :unshift_tuple => [1, 2],
    :cast_tuple =>    [1, 1],
    :make_rest =>     nil, # a special case
    :dup_top =>       [0, 1],
    :pop =>           [1, 0],
    :ret =>           [0, 0],
    :send_method =>   [1, 1],
    :send_stack =>  proc { |a| [a[1] + 1, 1] },
    :send_stack_with_block => proc { |a| [a[1] + 2, 1] },
    :push_block =>    [0, 1],
    :clear_exception => [0, 0],
    :soft_return =>   [0, 0],
    :caller_return => [0, 0],
    :push_array =>    nil, # arg. not sure what do do with this yet
    :cast_array =>    [1, 1],
    :make_hash =>     proc { |a| [a, 1] },
    :raise_exc =>     [1, 0],
    :set_encloser =>  [1, 0],
    :push_encloser => [0, 0],
    :activate_method => nil, # again, unsure
    :push_cpath_top => [0, 1],
    :check_argcount => [0, 0],
    :passed_arg =>    [0, 1],
    :string_append => [2, 1],
    :string_dup =>    [1, 1],
    :set_args =>      [1, 0],
    :get_args =>      [0, 1],
    :send_with_arg_register => nil,
    :cast_array_for_args => nil ,
    :send_super_stack_with_block,
    :push_my_field,
    :store_my_field,
    :open_metaclass,
    :set_cache_index,
    :block_break,
    :send_super_with_arg_register,
    :meta_push_neg_1,
    :meta_push_0,
    :meta_push_1,
    :meta_push_2,
    :meta_send_stack_1,
    :meta_send_stack_2,
    :meta_send_stack_3,
    :meta_send_stack_4,
    :meta_send_op_plus,
    :meta_send_op_minus,
    :meta_send_op_equal,
    :meta_send_op_lt,
    :meta_send_op_gt,
    :meta_send_op_tequal,
    :meta_send_op_nequal,
    :push_local_depth,
    :set_local_depth,
    :create_block,
    :send_off_stack,
    :locate_method,
    :kind_of,
    :instance_of,
    :set_call_flags,
    :yield_debugger,
    :from_fp,
    :set_local_from_fp,
    :make_rest_fp,
    :allocate_stack,
    :deallocate_stack,
    :set_local_fp,
    :get_local_fp
    
    
  }
  
=end
  
  OpCodes = [
    :noop,
    :push_nil,
    :push_true,
    :push_false,
    :allocate,
    :set_class,
    :store_field,
    :push_int,
    :fetch_field,
    :send_primitive,
    :push_context,
    :push_literal,
    :push_self,
    :goto,
    :goto_if_false,
    :goto_if_true,
    :swap_stack,
    :set_local,
    :push_local,
    :push_exception,
    :make_array,
    :set_ivar,
    :push_ivar,
    :goto_if_defined,
    :push_const,
    :set_const,
    :set_const_at,
    :find_const,
    :attach_method,
    :add_method,
    :open_class,
    :open_class_under,
    :open_module,
    :open_module_under,
    :unshift_tuple,
    :cast_tuple,
    :make_rest,
    :dup_top,
    :pop,
    :ret,
    :send_method,
    :send_stack,
    :send_stack_with_block,
    :push_block,
    :clear_exception,
    :soft_return,
    :caller_return,
    :push_array,
    :cast_array,
    :make_hash,
    :raise_exc,
    :set_encloser,
    :push_encloser,
    :activate_method,
    :push_cpath_top,
    :check_argcount,
    :passed_arg,
    :string_append,
    :string_dup,
    :set_args,
    :get_args,
    :send_with_arg_register,
    :cast_array_for_args,
    :send_super_stack_with_block,
    :push_my_field,
    :store_my_field,
    :open_metaclass,
    :set_cache_index,
    :block_break,
    :send_super_with_arg_register,
    :meta_push_neg_1,
    :meta_push_0,
    :meta_push_1,
    :meta_push_2,
    :meta_send_stack_1,
    :meta_send_stack_2,
    :meta_send_stack_3,
    :meta_send_stack_4,
    :meta_send_op_plus,
    :meta_send_op_minus,
    :meta_send_op_equal,
    :meta_send_op_lt,
    :meta_send_op_gt,
    :meta_send_op_tequal,
    :meta_send_op_nequal,
    :push_local_depth,
    :set_local_depth,
    :create_block,
    :send_off_stack,
    :locate_method,
    :kind_of,
    :instance_of,
    :set_call_flags,
    :yield_debugger,
    :from_fp,
    :set_local_from_fp,
    :make_rest_fp,
    :allocate_stack,
    :deallocate_stack,
    :set_local_fp,
    :get_local_fp
  ]
  
  IntArg = [
    :find_field, :push_int, :send_primitive, :push_literal,
    :goto, :goto_if_false, :goto_if_true,
    :goto_if_defined,
    :set_local,
    :push_local,
    :make_array,
    :set_ivar,
    :push_ivar,
    :push_const,
    :set_const,
    :set_const_at,
    :find_const,
    :attach_method,
    :add_method,
    :open_class,
    :open_class_under,
    :open_module,
    :open_module_under,
    :send_method,
    :send_stack,
    :make_hash,
    :make_rest,
    :activate_method,
    :send_stack_with_block,
    :check_argcount,
    :passed_arg,
    :send_with_arg_register,
    :cast_array_for_args,
    :send_super_stack_with_block,
    :push_my_field,
    :store_my_field,
    :set_cache_index,
    :send_super_with_arg_register,
    :meta_send_stack_1,
    :meta_send_stack_2,
    :meta_send_stack_3,
    :meta_send_stack_4,
    :push_local_depth,
    :set_local_depth,
    :create_block,
    :set_call_flags,
    :from_fp,
    :set_local_from_fp,
    :make_rest_fp,
    :allocate_stack,
    :deallocate_stack,
    :set_local_fp,
    :allocate_stack,
    :deallocate_stack,
    :set_local_fp,
    :get_local_fp
  ]
  
  TwoInt = [
    :send_stack, :send_stack_with_block,
    :check_argcount,
    :send_super_stack_with_block,
    :send_primitive,
    :push_local_depth,
    :set_local_depth,
    :set_local_from_fp
  ]
  
  def instruction_width(op)
    if Fixnum === op
      op = OpCodes[op]
    end
    
    width = 4
    if IntArg.include?(op)
      width += 4
    elsif TwoInt.include?(op)
      width += 8
    end
    
    return width
  end
  
  def encode_stream(stream)
    out = ""
    stream.each do |ent|
      out << encode(*ent)
    end
    puts out.inspect if ENV["DEBUG_ASSEMBLER"]
    return out
  end
  
  def encode(kind, *args)
    
    case kind
    when :store_field_at
      idx = args.shift
      return encode(:push_int, idx) + encode(:store_field)
    when :allocate
      unless args.empty?
        idx = args.shift
        return encode(:push_int, idx) + encode(:allocate)
      end
    end
    
    opcode = OpCodes.index(kind)
    if opcode.nil?
      raise InvalidOpCode, "Unknown opcode '#{kind}'"
    end
    
    orig = args.dup
    out = process_args(kind, opcode, args)
    puts "#{kind} (#{orig.inspect}) encoded as: #{out.inspect}" if ENV["DEBUG_ASSEMBLER"]
    out
  end
  
  def process_args(kind, opcode, args)
    str = [opcode].pack("N")
    # puts "#{kind} args. (#{str.inspect})"
    
    # puts args.inspect if ENV["DEBUG_ASSEMBLER"]

    if IntArg.include?(kind)
      int = args.shift
      unless Numeric === int
        raise "#{kind} expects an integer only, got a #{int.class} (#{int.inspect})"
      end
      is = [int].pack("N")
      # puts "#{kind} has a first arg: #{int} => #{is.inspect}."
      str << is
    end
    
    if TwoInt.include?(kind)
      int = args.shift
      unless Numeric === int
        raise "#{kind} expects an integer only, got a #{int.class} (#{int.inspect})"
      end
      is = [int].pack("N")
      # puts "#{kind} has a 2nd arg: #{int} => #{is.inspect}"
      str << is
    end
    
    unless args.empty?
      raise "Unused arguments to #{kind}, #{args.inspect}"
    end
    
    return str
  end
end
end

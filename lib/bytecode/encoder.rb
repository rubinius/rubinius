module Bytecode
class InstructionEncoder
  
  class InvalidOpCode < RuntimeError
  end
  
  OpCodes = [
    :noop,
    :push_nil,
    :push_true,
    :push_false,
    :allocate,
    :set_class,
    :find_field,
    :store_into,
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
    :open_metaclass
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
    :store_my_field
  ]
  
  TwoInt = [
    :send_stack, :send_stack_with_block,
    :check_argcount,
    :send_super_stack_with_block
  ]
  
  def encode_stream(stream)
    out = ""
    stream.each do |ent|
      out << encode(*ent)
    end
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
    # puts "#{kind} (#{orig.inspect}) encoded as: #{out.inspect}"
    out
  end
  
  def process_args(kind, opcode, args)
    str = opcode.chr
    
    if IntArg.include?(kind)
      int = args.shift
      unless Numeric === int
        raise "#{kind} expects an integer only, got a #{int.class} (#{int.inspect})"
      end
      str << [int].pack("N")
    end
    
    if TwoInt.include?(kind)
      int = args.shift
      unless Numeric === int
        raise "#{kind} expects an integer only, got a #{int.class} (#{int.inspect})"
      end
      str << [int].pack("N")
    end
    
    unless args.empty?
      raise "Unused arguments to #{kind}, #{args.inspect}"
    end
    
    return str
  end
end
end

class CPU::InstructionEncoder
  
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
    :find_const,
    :attach_method,
    :add_method,
    :open_class,
    :open_module,
    :dup_top,
    :pop,
    :ret,
    :send_stack,
    :push_block,
    :clear_exception
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
    :find_const,
    :attach_method,
    :add_method,
    :open_class,
    :open_module,
    :send_stack
  ]
  
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
    
    process_args(kind, opcode, args)
  end
  
  def process_args(kind, opcode, args)
    str = opcode.chr
    
    if IntArg.include?(kind)
      str << [args[0]].pack("I")
    end
    
    return str
  end
end
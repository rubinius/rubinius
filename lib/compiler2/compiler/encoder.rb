class Compiler
class Encoder
  
  class InvalidOpCode < Compiler::Error
  end

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
    :get_local_fp,
    :is_fixnum,
    :is_symbol,
    :is_nil,
    :class,
    :equal,
    :sret,
    :set_literal,
    :passed_blockarg,
    :create_block2,
    :cast_for_single_block_arg
  ]
  
  CheckInterupts = [
    :allocate,
    :send_primitive,
    :make_array,
    :set_ivar,
    :set_const,
    :set_const_at,
    :attach_method,
    :add_method,
    :open_class,
    :open_class_under,
    :open_module,
    :open_module_under,
    :unshift_tuple,
    :cast_tuple,
    :make_rest,
    :send_method,
    :send_stack,
    :send_stack_with_block,
    :cast_array,
    :make_hash,
    :activate_method,
    :string_append,
    :string_dup,
    :send_with_arg_register,
    :cast_array_for_args,
    :send_super_stack_with_block,
    :open_metaclass,
    :send_super_with_arg_register,
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
    :set_local_depth,
    :create_block,
    :create_block2,
    :send_off_stack,
    :yield_debugger,
    :make_rest_fp
  ]
  
  Terminators = [
    :ret,
    :sret,
    :soft_return,
    :caller_return,
    :block_break,
    :raise_exc,
    :check_argcount
  ]
  
  IntArg = [
    :push_int, :send_primitive, :push_literal,
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
    :get_local_fp,
    :set_literal,
    :passed_blockarg
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
  
  InstSize = 4
  
  @instructions = {}
  @width = {}
  i = 0
  OpCodes.each do |op|
    @instructions[op] = i
    if TwoInt.include? op
      @width[op] = 2
    elsif IntArg.include? op
      @width[op] = 1
    else
      @width[op] = 0
    end
    i += 1
  end
  
  def self.instructions
    @instructions
  end
  
  def self.width
    @width
  end
    
  def decode_iseq(iseq)
    @offset = 0
    stream = []
    while @offset < iseq.size
      inst = iseq2int iseq
      op = OpCodes[inst]
      
      case Encoder.width[op]
      when 1
        stream << [op, iseq2int(iseq)]
      when 2
        stream << [op, iseq2int(iseq), iseq2int(iseq)]
      else
        stream << [op]
      end
    end
    
    return stream
  end
  
  def iseq2int(iseq)
    inst =  (iseq[@offset    ] * 16777216)
    inst += (iseq[@offset + 1] * 65536)
    inst += (iseq[@offset + 2] * 256)
    inst += (iseq[@offset + 3])
    @offset += 4
    return inst
  end
  
  def instruction?(op)
    Encoder.instructions[op]
  end
    
  def encode_stream(stream)
    sz = stream.inject(0) { |acc, ele| acc + ((ele.size - 1) * InstSize) }
    iseq = InstructionSequence.new(sz)
    @offset = 0                 
    stream.each do |ent|
      encode(ent, iseq)    # TODO: move iseq to ivar, requires @offset anyway?
    end
    
    return iseq
  end
  
  def encode(inst, into)
    # TODO: Why is the opcode lookup not here instead of bytecode.rb?
    opcode = inst.last
    
    width = Encoder.width[inst.first]
    
    unless inst.size - 2 == width
      raise Error, "Missing instruction arguments to #{inst.first} (need #{width} / got #{inst.size - 2})"
    end
    
    int2str(opcode, into)
    
    case width
    when 1
      int2str(inst[1], into)
    when 2
      int2str(inst[1], into)
      int2str(inst[2], into)
    end
    
    return into
  end
  
  def int2str(int, into)
    3.downto(0) do |i|
      into[@offset + i] = (int % 256)
      int = int / 256
    end
    
    @offset += 4
  end  
end
end

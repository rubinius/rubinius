class InstructionSequence
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
  
  IntArg = [
    :activate_method,
    :add_method,
    :allocate_stack,
    :attach_method,
    :cast_array_for_args,
    :check_argcount,
    :create_block,
    :deallocate_stack,
    :find_const,
    :from_fp,
    :get_local_fp,
    :goto,
    :goto_if_false,
    :goto_if_true,
    :goto_if_defined,
    :make_array,
    :make_hash,
    :make_rest,
    :make_rest_fp,
    :meta_send_stack_1,
    :meta_send_stack_2,
    :meta_send_stack_3,
    :meta_send_stack_4,
    :open_class,
    :open_class_under,
    :open_module,
    :open_module_under,
    :passed_arg,
    :passed_blockarg,
    :push_const,
    :push_int,
    :push_ivar,
    :push_literal,
    :push_local,
    :push_local_depth,
    :push_my_field,
    :send_method,
    :send_primitive, 
    :send_stack,
    :send_stack_with_block,
    :send_super_stack_with_block,
    :send_super_with_arg_register,
    :send_with_arg_register,
    :set_cache_index,
    :set_call_flags,
    :set_const,
    :set_const_at,
    :set_ivar,
    :set_literal,
    :set_local,
    :set_local_depth,
    :set_local_fp,
    :set_local_from_fp,
    :store_my_field
  ]
  
  TwoInt = [
    :check_argcount,
    :push_local_depth,
    :send_primitive,
    :send_stack,
    :send_stack_with_block,
    :send_super_stack_with_block,
    :set_local_depth,
    :set_local_from_fp
  ]
  
  LiteralArgs = [
    :add_method, 
    :attach_method,
    :find_const,
    :meta_send_stack_1,
    :meta_send_stack_2,
    :meta_send_stack_3,
    :meta_send_stack_4,
    :open_class,
    :open_class_under,
    :open_module,
    :open_module_under,
    :push_const,
    :push_ivar,
    :push_literal,
    :send_method,
    :send_stack,
    :send_stack_with_block,
    :send_super_stack_with_block,
    :send_super_with_arg_register,
    :send_with_arg_register,
    :set_const,
    :set_const_at,
    :set_ivar,
    :set_literal
  ]

  InstSize = 4
  
  class Instruction
    def initialize(inst, cm = nil)
      @op = inst
      if cm and LiteralArgs.include? op_code
        @op[1] = cm.literals[@op[1]]
      end
    end
    
    # Returns the symbol representing the opcode for this instruction
    def op_code
      @op[0]
    end
    
    # Returns an array of 0 to 2 arguments, depending on the opcode
    def args
      @op[1..-1]
    end
    
    def to_s
      str = op_code.to_s
      args.each do |arg|
        str << "  " << arg.inspect
      end
      str
    end
  end

  class Encoder
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
      @iseq = iseq
      @offset = 0
      stream = []
      while @offset < @iseq.size
        inst = iseq2int
        op = OpCodes[inst]
      
        case Encoder.width[op]
        when 1
          stream << [op, iseq2int]
        when 2
          stream << [op, iseq2int, iseq2int]
        else
          stream << [op]
        end
      end
    
      return stream
    end
  
    def encode_stream(stream)
      sz = stream.inject(0) { |acc, ele| acc + ((ele.size - 1) * InstSize) }
      @iseq = InstructionSequence.new(sz)
      @offset = 0                 
      stream.each do |inst|
        opcode = inst.last

        width = Encoder.width[inst.first]

        unless inst.size - 2 == width
          raise Error, "Missing instruction arguments to #{inst.first} (need #{width} / got #{inst.size - 2})"
        end

        int2str(opcode)

        case width
        when 1
          int2str(inst[1])
        when 2
          int2str(inst[1])
          int2str(inst[2])
        end
      end
    
      return @iseq
    end
  
    def iseq2int
      inst =  (@iseq[@offset    ] * 16777216)
      inst += (@iseq[@offset + 1] * 65536)
      inst += (@iseq[@offset + 2] * 256)
      inst += (@iseq[@offset + 3])
      @offset += 4
      return inst
    end
  
    def int2str(int)
      3.downto(0) do |i|
        @iseq[@offset + i] = (int % 256)
        int = int / 256
      end
    
      @offset += 4
    end
  end

  # Decodes the instruction sequence into an array of +Instruction+s
  # If a +CompiledMethod+ object is passed in, it is used to convert literal
  # references in the opcodes to the corresponding literal values in the
  # decoded output. If no +CompiledMethod+ is passed, literal references
  # are expressed as indexes into the literals tuple.
  def decode(cm = nil)
    enc = Encoder.new
    dc = enc.decode_iseq(self)
    dc.map! do |inst|
      Instruction.new(inst, cm)
    end
  end
end
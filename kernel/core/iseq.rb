# depends on: class.rb array.rb

##
# Defines all the bytecode instructions used by the VM.

class InstructionSet

  ##
  # List of Rubinius machine opcodes
  #
  # Each opcode consists of a hash identifying:
  # - The opcode symbol
  # - An array of the arguments required by the opcode, which may be of types
  #   :int, :literal, :local, :block_local, :field, :primitive, :ip,
  #   :depth, or :cache
  # - A 2 element array of codes indicating what changes the opcode makes to
  #   the stack. The first code identifies the number of operands popped from
  #   the stack, and the second the number of operands pushed back onto the
  #   stack.
  #   
  #   If the code is zero or a positive value, it is the exact number of
  #   operands pushed or popped. If the code is a negative value, it means the
  #   number of operands consumed/produced is calculated based on another
  #   value, and cannot be determined just from the opcode.
  #   
  #   Negative codes
  #   consist of 3-digits, where:
  #   - the first digit is a multiplier (normally 1, but make_hash has a value
  #     of 2);
  #   - the second digit is the where the arg to be multiplied comes from;
  #     1 = first opcode arg, 2 = second opcode arg, 3 = arg register;
  #   - the final digit is a constant number to be added to the result.
  #   
  #   The value -999 is a special value, indicating that the result cannot be
  #   calculated from the bytecode, since it is dependent on the number of
  #   items in an array that will be on the stack when the opcode is
  #   encountered.
  # - If the opcode can change the flow of execution, it will have a :flow key,
  #   followed by a value indicating whether the opcode performs a :send, a
  #   :goto, or a :return.
  # - An optional :vm_flags key whose value is an array of the vm_flags set
  #   by the opcode. These flags are used when generating the opcode logic in
  #   instructions.rb into C include files.
  #--
  # IMPORTANT: Do not change the order of opcodes! The position in this array
  # is the opcode's instuction bytecode.

  OpCodes = [
    {:opcode => :noop, :args => [], :stack => [0,0]},
    {:opcode => :push_nil, :args => [], :stack => [0,1]},
    {:opcode => :push_true, :args => [], :stack => [0,1]},
    {:opcode => :push_false, :args => [], :stack => [0,1]},
    {:opcode => :allocate, :args => [], :stack => [1,1],
      :vm_flags => [:check_interrupts]},
    {:opcode => :set_class, :args => [], :stack => [2,1]},
    {:opcode => :store_field, :args => [], :stack => [3,1]},
    {:opcode => :push_int, :args => [:int], :stack => [0,1]},
    {:opcode => :fetch_field, :args => [], :stack => [2,1]},
    {:opcode => :send_primitive, :args => [:primitive, :int], :stack => [-121,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :push_context, :args => [], :stack => [0,1]},
    {:opcode => :push_literal, :args => [:literal], :stack => [0,1]},
    {:opcode => :push_self, :args => [], :stack => [0,1]},
    {:opcode => :goto, :args => [:ip], :stack => [0,0], :flow => :goto},
    {:opcode => :goto_if_false, :args => [:ip], :stack => [1,0], :flow => :goto},
    {:opcode => :goto_if_true, :args => [:ip], :stack => [1,0], :flow => :goto},
    {:opcode => :swap_stack, :args => [], :stack => [1,1]},
    {:opcode => :set_local, :args => [:local], :stack => [1,1]},
    {:opcode => :push_local, :args => [:local], :stack => [0,1]},
    {:opcode => :push_exception, :args => [], :stack => [0,1]},
    {:opcode => :make_array, :args => [:int], :stack => [-110,1],
      :vm_flags => []},
    {:opcode => :set_ivar, :args => [:literal], :stack => [1,1],
      :vm_flags => []},
    {:opcode => :push_ivar, :args => [:literal], :stack => [0,1]},
    {:opcode => :goto_if_defined, :args => [:ip], :stack => [1,0],
      :flow => :goto},
    {:opcode => :push_const, :args => [:literal], :stack => [0,1]},
    {:opcode => :set_const, :args => [:literal], :stack => [1,1],
      :vm_flags => []},
    {:opcode => :set_const_at, :args => [:literal], :stack => [2,0],
      :vm_flags => []},
    {:opcode => :find_const, :args => [:literal], :stack => [1,1]},
    {:opcode => :attach_method, :args => [:literal], :stack => [2,1],
      :vm_flags => [:check_interrupts]},
    {:opcode => :add_method, :args => [:literal], :stack => [2,1],
      :vm_flags => [:check_interrupts]},
    {:opcode => :open_class, :args => [:literal], :stack => [1,1],
      :vm_flags => [:check_interrupts]},
    {:opcode => :open_class_under, :args => [:literal], :stack => [2,1],
      :vm_flags => [:check_interrupts]},
    {:opcode => :open_module, :args => [:literal], :stack => [0,1],
      :vm_flags => [:check_interrupts]},
    {:opcode => :open_module_under, :args => [:literal], :stack => [1,1],
      :vm_flags => [:check_interrupts]},
    {:opcode => :unshift_tuple, :args => [], :stack => [1,2],
      :vm_flags => []},
    {:opcode => :cast_tuple, :args => [], :stack => [1,1],
      :vm_flags => []},
    {:opcode => :make_rest, :args => [:int], :stack => [-110,1],
      :vm_flags => []},
    {:opcode => :dup_top, :args => [], :stack => [0,1]},
    {:opcode => :pop, :args => [], :stack => [1,0]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :send_method, :args => [:literal], :stack => [1,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :send_stack, :args => [:literal, :int], :stack => [-121,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :send_stack_with_block, :args => [:literal, :int],
      :stack => [-122,1], :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :push_block, :args => [], :stack => [0,1]},
    {:opcode => :clear_exception, :args => [], :stack => [0,0]},
    {:opcode => :soft_return, :args => [], :stack => [1,0], :flow => :return,
      :vm_flags => [:terminator]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :push_array, :args => [], :stack => [1,-999]},
    {:opcode => :cast_array, :args => [], :stack => [1,1],
      :vm_flags => []},
    {:opcode => :make_hash, :args => [:int], :stack => [-210,1],
      :vm_flags => [:check_interrupts]},
    {:opcode => :raise_exc, :args => [], :stack => [1,0], :flow => :raise,
      :vm_flags => [:terminator]},
    {:opcode => :set_encloser, :args => [], :stack => [1,0]},
    {:opcode => :push_encloser, :args => [], :stack => [0,0]},
    {:opcode => :activate_method, :args => [:int], :stack => [-115,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :push_cpath_top, :args => [], :stack => [0,1]},
    {:opcode => :check_argcount, :args => [:int, :int], :stack => [0,0],
      :vm_flags => [:terminator]},
    {:opcode => :passed_arg, :args => [:int], :stack => [0,1]},
    {:opcode => :string_append, :args => [], :stack => [2,1],
     :vm_flags => []},
    {:opcode => :string_dup, :args => [], :stack => [1,1],
      :vm_flags => []},
    {:opcode => :set_args, :args => [], :stack => [1,0]},
    {:opcode => :get_args, :args => [], :stack => [0,1]},
    {:opcode => :send_with_arg_register, :args => [:literal], :stack => [-132,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :cast_array_for_args, :args => [:int], :stack => [1,1],
      :vm_flags => []},
    {:opcode => :send_super_stack_with_block,  :args => [:literal, :int],
      :stack => [-121,1], :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :push_my_field, :args => [:field], :stack => [0,1]},
    {:opcode => :store_my_field, :args => [:field], :stack => [1,1]},
    {:opcode => :open_metaclass, :args => [], :stack => [1,1]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :send_super_with_arg_register, :args => [:literal],
      :stack => [-131,1], :flow => :send},
    {:opcode => :meta_push_neg_1, :args => [], :stack => [0,1]},
    {:opcode => :meta_push_0, :args => [], :stack => [0,1]},
    {:opcode => :meta_push_1, :args => [], :stack => [0,1]},
    {:opcode => :meta_push_2, :args => [], :stack => [0,1]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :meta_send_op_plus, :args => [], :stack => [2,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_minus, :args => [], :stack => [2,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_equal, :args => [], :stack => [2,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_lt, :args => [], :stack => [2,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_gt, :args => [], :stack => [2,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_tequal, :args => [], :stack => [2,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_nequal, :args => [], :stack => [2,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :push_local_depth, :args => [:depth, :block_local],
      :stack => [0,1]},
    {:opcode => :set_local_depth, :args => [:depth, :block_local],
      :stack => [1,1], :vm_flags => []},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :send_off_stack, :args => [], :stack => [-133,1],
      :flow => :send, :vm_flags => [:check_interrupts]},
    {:opcode => :locate_method, :args => [], :stack => [3,1]},
    {:opcode => :kind_of, :args => [], :stack => [2,1]},
    {:opcode => :instance_of, :args => [], :stack => [2,1]},
    {:opcode => :set_call_flags, :args => [:int], :stack => [0,0]},
    {:opcode => :yield_debugger, :args => [], :stack => [0,0],
      :vm_flags => [:check_interrupts]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :set_local_from_fp, :args => [:local, :int], :stack => [0,0]},
    {:opcode => :make_rest_fp, :args => [:int], :stack => [0,1],
      :vm_flags => []},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :is_fixnum, :args => [], :stack => [1,1]},
    {:opcode => :is_symbol, :args => [], :stack => [1,1]},
    {:opcode => :is_nil, :args => [], :stack => [1,1]},
    {:opcode => :class, :args => [], :stack => [1,1]},
    {:opcode => :equal, :args => [], :stack => [2,1]},
    {:opcode => :sret, :args => [], :stack => [1,0], :flow => :return,
      :vm_flags => [:terminator]},
    {:opcode => :set_literal, :args => [:literal], :stack => [0,0]},
    {:opcode => :passed_blockarg, :args => [:int], :stack => [0,1]},
    {:opcode => :create_block2, :args => [], :stack => [1,1],
      :vm_flags => []},
    {:opcode => :cast_for_single_block_arg, :args => [], :stack => [1,1]},
    {:opcode => :cast_for_multi_block_arg, :args => [], :stack => [1,1]},
    {:opcode => :unused, :args => [], :stack => [0,0]},
    {:opcode => :check_serial, :args => [:literal, :int], :stack => [1,1]},
    {:opcode => :meta_send_call, :args => [:int], :stack => [-111,1],
      :flow => :send},
    {:opcode => :rotate, :args => [:int], :stack => [0,0]}
  ]

  InstructionSize = 4

  class OpCode
    def initialize(opcode_info)
      @opcode_info = opcode_info
    end

    def opcode
      @opcode_info[:opcode]
    end

    def bytecode
      @opcode_info[:bytecode]
    end

    def arg_count
      @opcode_info[:args].size
    end

    def args
      @opcode_info[:args]
    end

    ##
    # Returns the size of the opcode (including arguments)

    def size
      @opcode_info[:args].size + 1
    end

    ##
    # Returns the width of the opcode (including arguments) in bytes

    def width
      (@opcode_info[:args].size + 1) * InstructionSize
    end

    ##
    # Returns the number of items consumed off of the stack by this opcode.
    #
    # If the value is positive, it is the exact number of items consumed.
    #
    # If the value is negative, it is a 3-digit number where:
    # - first digit is a multiplier (1 or 2)
    # - second digit is the opcode arg to be multiplied (1 or 2), or the
    #   contents of the args register (3) at that point.
    # - third digit is a constant arg count to be added to the result
    #
    # For example, the value -210 would indicate that the number of stack
    # items consumed by the opcode is 2 * the value of the first opcode arg.
    #
    # The special value -999 is reserved for cases where the number of
    # arguments consumed cannot be determined from the bytecode itself. This
    # is currently only the case with :push_array, although opcodes that use
    # the args register may also be indeterminate if used with
    # :cast_array_for_args.

    def stack_consumed
      @opcode_info[:stack].first
    end

    ##
    # Returns the number of items produced off of the stack by this opcode.
    #
    # If the value is positive, it is the exact number of items produced.
    #
    # If the value is negative, it is a 3-digit number where:
    # - first digit is a multiplier (1 or 2)
    # - second digit is the opcode arg to be multiplied
    # - third digit is a constant arg count to be added to the result
    #
    # For example, the value -110 would indicate that the number of stack
    # items produced by the opcode is 1 * the value of the first opcode arg.
    #
    # The special values -990 to -999 are reserved for cases where the number
    # of arguments produced or consumed cannot be determined from the bytecode
    # itself. This is currently only the case with :push_array and
    # :send_with_arg_register.

    def stack_produced
      @opcode_info[:stack].last
    end

    def check_interrupts?
      flags = @opcode_info[:vm_flags]
      flags and flags.include? :check_interrupts
    end

    def terminator?
      flags = @opcode_info[:vm_flags]
      flags and flags.include? :terminator
    end

    ##
    # Returns a symbol specifying the effect of the symbol on the flow of
    # execution, or nil if the instruction does not effect flow. The symbol
    # may be one of :sequential, :send, :return, :goto, or :raise.

    def flow
      @opcode_info[:flow] || :sequential
    end

    def to_s
      @opcode_info[:opcode].to_s
    end
  end

  class InvalidOpCode < RuntimeError
  end

  @opcodes = {}
  i = 0
  OpCodes.map! do |info|
    info[:bytecode] = i
    i += 1
    op = OpCode.new info
    @opcodes[op.opcode] = op
  end

  def self.[](op)
    inst = nil
    if op.kind_of? Fixnum
      inst = OpCodes[op]
    else
      inst = @opcodes[op]
    end
    raise InvalidOpCode, "Invalid opcode #{op}" if inst.nil?
    inst
  end
end

##
# A list of bytecode instructions.

class InstructionSequence

  ##
  # Encodes an array of symbols representing bytecode into an
  # InstructionSequence

  class Encoder

    ##
    # Decodes an InstructionSequence (which is essentially a an array of ints)
    # into an array whose elements are arrays of opcode symbols and 0-2 args,
    # depending on the opcode.

    def decode_iseq(iseq, symbols_only=true)
      @iseq = iseq
      @offset = 0
      stream = []

      last_good = [nil, 0]
      
      begin
        while @offset < @iseq.size
          inst = decode
          stream << inst
          op = inst.first
          last_good = [op, stream.size] unless op.opcode == :noop
        end
      rescue InstructionSet::InvalidOpCode => ex
        # Because bytearrays and iseqs are allocated in chunks of 4 or 8 bytes,
        # we can get junk at the end of the iseq
        unless last_good.first and last_good.first.flow == :return
          ex.message << " at byte #{@offset} of #{@iseq.size} [IP:#{@offset / InstructionSet::InstructionSize}]"
          raise ex
        end
      end
      # Remove any noops or other junk at the end of the iseq
      stream.slice! last_good.last, stream.size
      if symbols_only
        stream.each {|i| i[0] = i[0].opcode}
      end

      return stream
    end

    ##
    # Encodes a stream of instructions into an InstructionSequence. The stream
    # supplied must be an array of arrays, with the inner array consisting of
    # an instruction opcode (a symbol), followed by 0 to 2 integer arguments,
    # whose meaning depends on the opcode.

    def encode_stream(stream)
      sz = stream.inject(0) { |acc, ele| acc + (ele.size * InstructionSet::InstructionSize) }
      @iseq = InstructionSequence.new(sz)
      @offset = 0
      begin
        stream.each do |inst|
          encode inst
        end
      rescue Exception => e
        STDERR.puts "Unable to encode stream:"
        STDERR.puts stream.inspect
        raise e
      end

      return @iseq
    end

    ##
    # Replaces the instruction at the specified instruction pointer with the
    # supplied instruction inst, which must be an array containing the new
    # instruction opcode symbol, followed by any int args required by the
    # opcode.
    #
    # The new instruction must be the same width or smaller than the
    # instruction it replaces.

    def replace_instruction(iseq, ip, inst)
      @iseq = iseq
      @offset = start = ip * InstructionSet::InstructionSize

      old_inst = iseq2int
      old_op = InstructionSet[old_inst]
      new_op = inst.first
      new_op = InstructionSet[inst.first] unless new_op.kind_of? InstructionSet::OpCode
      @offset += old_op.arg_count * InstructionSet::InstructionSize
      old_op.size.upto(new_op.size-1) do
        next_inst = iseq2int
        unless next_inst == 0
          raise ArgumentError, "Cannot replace an instruction with a larger instruction (existing #{old_op.opcode} / new #{new_op.opcode})"
        end
      end
      @offset = start + InstructionSet::InstructionSize
      replaced = [old_op.opcode]

      1.upto(old_op.arg_count) do
        replaced << iseq2int
        @offset -= InstructionSet::InstructionSize
        int2str(0)  # Replace old args with 0
      end
      
      @offset = start
      encode inst
      replaced
    end

    ##
    # Decodes a single instruction at the specified instruction pointer
    # address.

    def decode_instruction(iseq, ip)
      @iseq = iseq
      @offset = ip * InstructionSet::InstructionSize

      decode
    end

    def decode
      inst = iseq2int
      op = InstructionSet[inst]

      case op.arg_count
      when 0
        [op]
      when 1
        [op, iseq2int]
      when 2
        [op, iseq2int, iseq2int]
      end
    end

    private :decode
    
    def encode(inst)
      opcode = inst.first
      unless opcode.kind_of? InstructionSet::OpCode
        opcode = InstructionSet[opcode]
      end

      arg_count = opcode.arg_count
      unless inst.size - 1 == arg_count
        raise ArgumentError, "Missing instruction arguments to #{inst.first} (need #{arg_count} / got #{inst.size - 1})"
      end

      begin
        int2str(opcode.bytecode)
        case arg_count
        when 1
          int2str(inst[1])
        when 2
          int2str(inst[1])
          int2str(inst[2])
        end
      rescue Object
        raise ArgumentError, "Unable to encode #{inst.inspect}"
      end
    end

    private :encode

    def iseq2int
      inst =  (@iseq[@offset    ] * 16777216)
      inst += (@iseq[@offset + 1] * 65536)
      inst += (@iseq[@offset + 2] * 256)
      inst += (@iseq[@offset + 3])
      @offset += 4
      return inst
    end

    private :iseq2int

    def int2str(int)
      3.downto(0) do |i|
        @iseq[@offset + i] = (int % 256)
        int = int / 256
      end

      @offset += 4
    end

    private :int2str
  end

  ##
  # Decodes the instruction sequence into an array of symbols

  def decode(symbols_only=true)
    enc = Encoder.new
    enc.decode_iseq(self, symbols_only)
  end
end

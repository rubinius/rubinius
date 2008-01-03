# depends on: class.rb array.rb

class InstructionSet
  # List of Rubinius machine opcodes
  # Each opcode consists of a hash identifying:
  #   - the opcode symbol,
  #   - an array of the arguments required by the opcode, which may be of types
  #     :int, :literal, :slot_local, :stack_local, :field, :primitive, :ip,
  #     :depth, or :cache
  # IMPORTANT: Do not change the order of opcodes! The position in this array
  # is the opcode's instuction bytecode.
  OpCodes = [
    {:opcode => :noop, :args => []},
    {:opcode => :push_nil, :args => []},
    {:opcode => :push_true, :args => []},
    {:opcode => :push_false, :args => []},
    {:opcode => :allocate, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :set_class, :args => []},
    {:opcode => :store_field, :args => []},
    {:opcode => :push_int, :args => [:int]},
    {:opcode => :fetch_field, :args => []},
    {:opcode => :send_primitive, :args => [:primitive, :int], :vm_flags => [:check_interrupts]},
    {:opcode => :push_context, :args => []},
    {:opcode => :push_literal, :args => [:literal]},
    {:opcode => :push_self, :args => []},
    {:opcode => :goto, :args => [:ip]},
    {:opcode => :goto_if_false, :args => [:ip]},
    {:opcode => :goto_if_true, :args => [:ip]},
    {:opcode => :swap_stack, :args => []},
    {:opcode => :set_local, :args => [:slot_local]},
    {:opcode => :push_local, :args => [:slot_local]},
    {:opcode => :push_exception, :args => []},
    {:opcode => :make_array, :args => [:int], :vm_flags => [:check_interrupts]},
    {:opcode => :set_ivar, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :push_ivar, :args => [:literal]},
    {:opcode => :goto_if_defined, :args => [:ip]},
    {:opcode => :push_const, :args => [:literal]},
    {:opcode => :set_const, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :set_const_at, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :find_const, :args => [:literal]},
    {:opcode => :attach_method, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :add_method, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :open_class, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :open_class_under, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :open_module, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :open_module_under, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :unshift_tuple, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :cast_tuple, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :make_rest, :args => [:int], :vm_flags => [:check_interrupts]},
    {:opcode => :dup_top, :args => []},
    {:opcode => :pop, :args => []},
    {:opcode => :ret, :args => [], :vm_flags => [:terminator]},
    {:opcode => :send_method, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :send_stack, :args => [:literal, :int], :vm_flags => [:check_interrupts]},
    {:opcode => :send_stack_with_block, :args => [:literal, :int], :vm_flags => [:check_interrupts]},
    {:opcode => :push_block, :args => []},
    {:opcode => :clear_exception, :args => []},
    {:opcode => :soft_return, :args => [], :vm_flags => [:terminator]},
    {:opcode => :caller_return, :args => [], :vm_flags => [:terminator]},
    {:opcode => :push_array, :args => []},
    {:opcode => :cast_array, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :make_hash, :args => [:int], :vm_flags => [:check_interrupts]},
    {:opcode => :raise_exc, :args => [], :vm_flags => [:terminator]},
    {:opcode => :set_encloser, :args => []},
    {:opcode => :push_encloser, :args => []},
    {:opcode => :activate_method, :args => [:int], :vm_flags => [:check_interrupts]},
    {:opcode => :push_cpath_top, :args => []},
    {:opcode => :check_argcount, :args => [:int, :int], :vm_flags => [:terminator]},
    {:opcode => :passed_arg, :args => [:int]},
    {:opcode => :string_append, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :string_dup, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :set_args, :args => []},
    {:opcode => :get_args, :args => []},
    {:opcode => :send_with_arg_register, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :cast_array_for_args, :args => [:int], :vm_flags => [:check_interrupts]},
    {:opcode => :send_super_stack_with_block,  :args => [:literal, :int], :vm_flags => [:check_interrupts]},
    {:opcode => :push_my_field, :args => [:field]},
    {:opcode => :store_my_field, :args => [:field]},
    {:opcode => :open_metaclass, :args => []},
    {:opcode => :set_cache_index, :args => [:cache]},
    {:opcode => :block_break, :args => [], :vm_flags => [:terminator]},
    {:opcode => :send_super_with_arg_register, :args => [:literal]},
    {:opcode => :meta_push_neg_1, :args => []},
    {:opcode => :meta_push_0, :args => []},
    {:opcode => :meta_push_1, :args => []},
    {:opcode => :meta_push_2, :args => []},
    {:opcode => :meta_send_stack_1, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_stack_2, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_stack_3, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_stack_4, :args => [:literal], :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_plus, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_minus, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_equal, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_lt, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_gt, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_tequal, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :meta_send_op_nequal, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :push_local_depth, :args => [:depth, :enclosed_local]},
    {:opcode => :set_local_depth, :args => [:depth, :enclosed_local], :vm_flags => [:check_interrupts]},
    {:opcode => :create_block, :args => [:int], :vm_flags => [:check_interrupts]},
    {:opcode => :send_off_stack, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :locate_method, :args => []},
    {:opcode => :kind_of, :args => []},
    {:opcode => :instance_of, :args => []},
    {:opcode => :set_call_flags, :args => [:int]},
    {:opcode => :yield_debugger, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :from_fp, :args => [:int]},
    {:opcode => :set_local_from_fp, :args => [:slot_local, :int]},
    {:opcode => :make_rest_fp, :args => [:int], :vm_flags => [:check_interrupts]},
    {:opcode => :allocate_stack, :args => [:int]},
    {:opcode => :deallocate_stack, :args => [:int]},
    {:opcode => :set_local_fp, :args => [:int]},
    {:opcode => :get_local_fp, :args => [:int]},
    {:opcode => :is_fixnum, :args => []},
    {:opcode => :is_symbol, :args => []},
    {:opcode => :is_nil, :args => []},
    {:opcode => :class, :args => []},
    {:opcode => :equal, :args => []},
    {:opcode => :sret, :args => [], :vm_flags => [:terminator]},
    {:opcode => :set_literal, :args => [:literal]},
    {:opcode => :passed_blockarg, :args => [:int]},
    {:opcode => :create_block2, :args => [], :vm_flags => [:check_interrupts]},
    {:opcode => :cast_for_single_block_arg, :args => []},
    {:opcode => :cast_for_multi_block_arg, :args => []}
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

    # Returns the size of the opcode (including arguments)
    def size
      @opcode_info[:args].size + 1
    end

    # Returns the width of the opcode (including arguments) in bytes
    def width
      (@opcode_info[:args].size + 1) * InstructionSize
    end

    def check_interrupts?
      flags = @opcode_info[:vm_flags]
      flags and flags.include? :check_interrupts
    end

    def terminator?
      flags = @opcode_info[:vm_flags]
      flags and flags.include? :terminator
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


class InstructionSequence

  class Encoder
    # Decodes an +InstructionSequence+ (which is essentially a an array of ints)
    # into an array whose elements are arrays of opcode symbols and 0-2 args,
    # depending on the opcode.
    def decode_iseq(iseq)
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
        # If direct-threading is not used, we can get junk at the end of the iseq
        unless last_good.first and last_good.first.terminator?
          ex.message << " at byte #{@offset} of #{@iseq.size} [IP:#{@offset / InstructionSet::InstructionSize}]"
          raise ex
        end
        stream.slice! last_good.last, stream.size
      end

      return stream
    end

    # Encodes a stream of instructions into an +InstructionSequence+. The stream
    # supplied must be an array of arrays, with the inner array consisting of an
    # instruction opcode (a symbol), followed by 0 to 2 integer arguments, whose
    # meaning depends on the opcode.
    def encode_stream(stream)
      sz = stream.inject(0) { |acc, ele| acc + (ele.size * InstructionSet::InstructionSize) }
      @iseq = InstructionSequence.new(sz)
      @offset = 0
      stream.each do |inst|
        encode inst
      end

      return @iseq
    end
    
    # Replaces the instruction at the specified instruction pointer with the
    # supplied instruction inst, which must be an array containing the new
    # instruction opcode symbol, followed by any int args required by the opcode.
    # Note: The new instruction must be the same width or smaller than the 
    # instruction it replaces.
    def replace_instruction(iseq, ip, inst)
      @iseq = iseq
      @offset = start = ip * InstructionSet::InstructionSize

      old_inst = iseq2int
      old_op = InstructionSet[old_inst]
      new_op = InstructionSet[inst.first]
      old_op.size.upto(new_op.size-1) do
        next_inst = iseq2int
        unless next_inst == 0
          raise ArgumentError, "Cannot replace an instruction with a larger instruction (existing #{old_op.opcode} / new #{new_op.opcode})"
        end
      end
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

    # Decodes a single instruction at the specified instruction pointer address
    def decode_instruction(iseq, ip)
      @iseq = iseq
      @offset = ip * InstructionSet::InstructionSize

      decode
    end

    private

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
    
    def encode(inst)
      opcode = inst.first
      unless inst.kind_of? InstructionSet::OpCode
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
  def decode
    enc = Encoder.new
    enc.decode_iseq(self)
  end
end

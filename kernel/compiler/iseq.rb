##
# Defines all the bytecode instructions used by the VM.

module Rubinius
  class InstructionSet

    class OpCode
      attr_reader :args, :arg_count, :bytecode, :opcode, :size,
                  :stack, :stack_consumed, :stack_produced, :variable_stack,
                  :position, :stack_difference

      ##
      # Returns a symbol specifying the effect of the symbol on the flow of
      # execution, or nil if the instruction does not effect flow. The symbol
      # may be one of :sequential, :send, :return, :goto, or :raise.

      attr_reader :flow

      alias_method :name, :opcode
      alias_method :width, :size

      def initialize(opcode, bytecode, params)
        @opcode         = opcode
        @bytecode       = bytecode
        @args           = params[:args]
        @arg_count      = @args.size
        @size           = @arg_count + 1
        @flow           = params[:flow] || :sequential
        @flags          = params[:vm_flags] || []

        @stack_consumed, @stack_produced = params[:stack]
        if @stack_consumed
          @variable_stack = false
          @stack_difference = @stack_produced - @stack_consumed
        else
          @variable_stack = true
          extra, @position = params[:variable_stack]
          @stack_difference = @stack_produced - extra
        end
      end

      def variable_stack?
        @variable_stack
      end

      def check_interrupts?
        @flags.include? :check_interrupts
      end

      def terminator?
        @flags.include? :terminator
      end

      def to_s
        @opcode.to_s
      end
    end


    # InstructionSet methods

    # Allocates a new ID every call.
    def self.opcode_id
      @opcode_ids ||= -1
      @opcode_ids += 1
    end

    # Returns the opcode map.
    def self.opcodes_map
      @opcodes_map ||= Rubinius::LookupTable.new
    end

    # Returns an array of OpCode instances.
    def self.opcodes
      @opcodes ||= []
    end

    # Utility method for defining the opcodes.
    def self.opcode(name, params={})
      id = opcode_id
      opcodes[id] = OpCode.new name, id, params
      opcodes_map[name] = opcodes_map[id] = id
    end

    class InvalidOpCode < RuntimeError
    end

    # Returns an opcode given its name or numeric ID.
    def self.[](name_or_id)
      opcode = opcodes[opcodes_map[name_or_id]]
      raise InvalidOpCode, "Invalid opcode #{op.inspect}" unless opcode
      opcode
    end

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
    #   Negative codes consist of 2-digits, where:
    #   - the first digit is where the arg to be multiplied comes from;
    #     1 = first opcode arg, 2 = second opcode arg, 3 = arg register;
    #   - the final digit is a constant number to be added to the result.
    #
    # - If the opcode can change the flow of execution, it will have a :flow key,
    #   followed by a value indicating whether the opcode performs a :send, a
    #   :goto, or a :return.
    # - An optional :vm_flags key whose value is an array of the vm_flags set
    #   by the opcode. These flags are used when generating the opcode logic in
    #   instructions.rb into C include files.
    #--
    # IMPORTANT: Do not change the order of opcodes! The position in this array
    # is the opcode's instuction bytecode.

    opcode :noop,                 :stack => [0, 0],   :args => []

    # pushs
    opcode :push_nil,             :stack => [0, 1],   :args => []
    opcode :push_true,            :stack => [0, 1],   :args => []
    opcode :push_false,           :stack => [0, 1],   :args => []
    opcode :push_int,             :stack => [0, 1],   :args => [:int]
    opcode :push_self,            :stack => [0, 1],   :args => []

    # literals
    opcode :set_literal,          :stack => [0, 0],   :args => [:literal]
    opcode :push_literal,         :stack => [0, 1],   :args => [:literal]

    # flow control

    opcode :goto,                 :stack => [0, 0],   :args => [:ip],
                                                      :flow => :goto
    opcode :goto_if_false,        :stack => [1, 0],   :args => [:ip],
                                                      :flow => :goto
    opcode :goto_if_true,         :stack => [1, 0],   :args => [:ip],
                                                      :flow => :goto
    opcode :goto_if_defined,      :stack => [1, 0],   :args => [:ip],
                                                      :flow => :goto
    opcode :ret,                  :stack => [0, 0],   :args => [],
                                                      :flow => :return,
                                                      :vm_flags => [:terminator]

    # stack maintainence
    opcode :swap_stack,           :stack => [1, 1],   :args => []
    opcode :dup_top,              :stack => [0, 1],   :args => []
    opcode :pop,                  :stack => [1, 0],   :args => []
    opcode :rotate,               :stack => [0, 0],   :args => [:int]
    opcode :move_down,            :stack => [0, 0],   :args => [:int]

    # locals
    opcode :set_local,            :stack => [1, 1],   :args => [:local]
    opcode :push_local,           :stack => [0, 1],   :args => [:local]
    opcode :push_local_depth,     :stack => [0, 1],   :args => [:depth, :block_local]
    opcode :set_local_depth,      :stack => [1, 1],   :args => [:depth, :block_local]
    opcode :passed_arg,           :stack => [0, 1],   :args => [:int]

    # exceptions
    opcode :push_exception,       :stack => [0, 1],   :args => []
    opcode :clear_exception,      :stack => [0, 0],   :args => []
    opcode :pop_exception,        :stack => [1, 0],   :args => []
    opcode :raise_exc,            :stack => [0, 0],   :args => [],
                                                      :flow => :raise,
                                                      :vm_flags => [:terminator]
    opcode :setup_unwind,         :stack => [0, 0],   :args => [:ip, :type]
    opcode :pop_unwind,           :stack => [0, 0],   :args => []
    opcode :raise_return,         :stack => [0, 0],   :args => []
    opcode :ensure_return,        :stack => [0, 0],   :args => []
    opcode :raise_break,          :stack => [0, 0],   :args => []
    opcode :reraise,              :stack => [0, 0],   :args => []

    # array
    opcode :make_array,           :stack => [nil, 1], :args => [:int],
                                                      :variable_stack => [0,1]
    opcode :cast_array,           :stack => [1, 1],   :args => []
    opcode :shift_array,          :stack => [1, 2],   :args => []

    # ivars
    opcode :set_ivar,             :stack => [1, 1],   :args => [:literal]
    opcode :push_ivar,            :stack => [0, 1],   :args => [:literal]

    # constants
    opcode :push_const,           :stack => [0, 1],   :args => [:literal]
    opcode :set_const,            :stack => [1, 1],   :args => [:literal]
    opcode :set_const_at,         :stack => [2, 0],   :args => [:literal]
    opcode :find_const,           :stack => [1, 1],   :args => [:literal]
    opcode :push_cpath_top,       :stack => [0, 1],   :args => []
    opcode :push_const_fast,      :stack => [0, 1],   :args => [:literal, :literal]

    # send
    opcode :set_call_flags,               :stack => [0, 0],   :args => [:int]
    opcode :allow_private,                :stack => [0, 0],   :args => []
    opcode :send_method,                  :stack => [1, 1],   :args => [:literal],
                                                              :flow => :send,
                                                              :vm_flags => [:check_interrupts]
    opcode :send_stack,                   :stack => [nil, 1], :args => [:literal, :int],
                                                              :flow => :send,
                                                              :vm_flags => [:check_interrupts],
                                                              :variable_stack => [1,2]
    opcode :send_stack_with_block,        :stack => [nil, 1], :args => [:literal, :int],
                                                              :flow => :send,
                                                              :vm_flags => [:check_interrupts],
                                                              :variable_stack => [2,2]
    opcode :send_stack_with_splat,        :stack => [nil, 1], :args => [:literal, :int],
                                                              :flow => :send,
                                                              :vm_flags => [:check_interrupts],
                                                              :variable_stack => [3,2]
    opcode :send_super_stack_with_block,  :stack => [nil, 1], :args => [:literal, :int],
                                                              :flow => :send,
                                                              :vm_flags => [:check_interrupts],
                                                              :variable_stack => [1,2]
    opcode :send_super_stack_with_splat,  :stack => [nil, 1], :args => [:literal, :int],
                                                              :flow => :send,
                                                              :variable_stack => [2,2]

    # blocks
    opcode :push_block,                   :stack => [0, 1],   :args => []
    opcode :passed_blockarg,              :stack => [0, 1],   :args => [:int]
    opcode :create_block,                 :stack => [0, 1],   :args => [:literal]
    opcode :cast_for_single_block_arg,    :stack => [0, 1],   :args => []
    opcode :cast_for_multi_block_arg,     :stack => [0, 1],   :args => []
    opcode :cast_for_splat_block_arg,     :stack => [0, 1],   :args => []
    opcode :yield_stack,                  :stack => [nil, 1], :args => [:int],
                                                              :flow => :send,
                                                              :variable_stack => [0,1]
    opcode :yield_splat,                  :stack => [nil, 1], :args => [:int],
                                                              :flow => :send,
                                                              :variable_stack => [1,1]

    # strings
    opcode :string_append,        :stack => [2, 1],   :args => []
    opcode :string_dup,           :stack => [1, 1],   :args => []

    # scope
    opcode :push_scope,           :stack => [0, 1],   :args => []
    opcode :add_scope,            :stack => [1, 0],   :args => []

    # misc
    opcode :push_variables,       :stack => [0, 1],   :args => []
    opcode :check_interrupts,     :stack => [0, 0],   :args => []
    opcode :yield_debugger,       :stack => [0, 0],   :args => [],
                                                      :vm_flags => [:check_interrupts]
    opcode :is_nil,               :stack => [1, 1],   :args => []
    opcode :check_serial,         :stack => [1, 1],   :args => [:literal, :int]
    opcode :check_serial_private, :stack => [1, 1],   :args => [:literal, :int]

    # field access                              ,
    opcode :push_my_field,        :stack => [0, 1],   :args => [:field]
    opcode :store_my_field,       :stack => [1, 1],   :args => [:field]

    # type checks                               ,
    opcode :kind_of,              :stack => [2, 1],   :args => []
    opcode :instance_of,          :stack => [2, 1],   :args => []

    # meta opcodes, used for optimization only.
    opcode :meta_push_neg_1,      :stack => [0, 1],   :args => []
    opcode :meta_push_0,          :stack => [0, 1],   :args => []
    opcode :meta_push_1,          :stack => [0, 1],   :args => []
    opcode :meta_push_2,          :stack => [0, 1],   :args => []
    opcode :meta_send_op_plus,    :stack => [2, 1],   :args => [:literal],
                                                      :flow => :send,
                                                      :vm_flags => [:check_interrupts]
    opcode :meta_send_op_minus,   :stack => [2, 1],   :args => [:literal],
                                                      :flow => :send,
                                                      :vm_flags => [:check_interrupts]
    opcode :meta_send_op_equal,   :stack => [2, 1],   :args => [:literal],
                                                      :flow => :send,
                                                      :vm_flags => [:check_interrupts]
    opcode :meta_send_op_lt,      :stack => [2, 1],   :args => [:literal],
                                                      :flow => :send,
                                                      :vm_flags => [:check_interrupts]
    opcode :meta_send_op_gt,      :stack => [2, 1],   :args => [:literal],
                                                      :flow => :send,
                                                      :vm_flags => [:check_interrupts]
    opcode :meta_send_op_tequal,  :stack => [2, 1],   :args => [:literal],
                                                      :flow => :send,
                                                      :vm_flags => [:check_interrupts]
    opcode :meta_send_call,       :stack => [nil, 1], :args => [:literal, :int],
                                                      :flow => :send,
                                                      :variable_stack => [1,2]
    opcode :push_my_offset,       :stack => [0, 1],   :args => [:field]

  end

  ##
  # A list of bytecode instructions.


  class InstructionSequence
    def initialize(size)
      @opcodes = Tuple.new(size)
      @stack_depth = 0
    end

    attr_reader :opcodes
    attr_accessor :stack_depth

    def ==(other)
      other.kind_of?(InstructionSequence) and @opcodes == other.opcodes
    end

    def []=(idx, val)
      @opcodes[idx] = val
    end

    def [](idx)
      @opcodes[idx]
    end

    def size
      @opcodes.size
    end

    ##
    # Encodes an array of symbols representing bytecode into an
    # InstructionSequence

    class Encoder

      attr_reader :stack_depth

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
            ex.message << " at byte #{@offset} of #{@iseq.size} [IP:#{@offset}]"
            raise ex
          end
        end
        # Remove any noops or other junk at the end of the iseq
        # HACK Removed for now due to splat handling bug... is this still even necessary?
        #stream.slice! last_good.last, stream.size
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
        sz = stream.inject(0) do |acc, ele|
          acc + (ele.kind_of?(Array) ? ele.size : 1)
        end

        @max_stack_depth = 0
        @stack_depth = 0
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

        @iseq.stack_depth = @max_stack_depth

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
        @offset = start = ip

        old_inst = iseq2int
        old_op = InstructionSet[old_inst]
        new_op = inst.first
        unless new_op.kind_of? InstructionSet::OpCode
          new_op = InstructionSet[inst.first]
        end
        @offset += old_op.arg_count
        old_op.size.upto(new_op.size-1) do
          next_inst = iseq2int
          unless next_inst == 0
            raise ArgumentError, "Cannot replace an instruction with a larger instruction (existing #{old_op.opcode} / new #{new_op.opcode})"
          end
        end

        @offset = start
        replaced = [old_op.opcode]
        1.upto(old_op.arg_count) do
          replaced << iseq2int
          int2iseq(0)    # Replace any old opcode args with 0 (i.e. noop)
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
        @offset = ip

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

      def iseq2int
        op = @iseq[@offset]
        @offset += 1
        return op
      end

      private :iseq2int

      def int2iseq(op)
        @iseq[@offset] = op
        @offset += 1
        return op
      end

      private :int2iseq

      def encode(inst)
        if inst.kind_of? Array
          opcode = inst.first
          unless opcode.kind_of? InstructionSet::OpCode
            opcode = InstructionSet[opcode]
          end

          arg_count = opcode.arg_count
          unless inst.size - 1 == arg_count
            raise ArgumentError, "Missing instruction arguments to #{inst.first} (need #{arg_count} / got #{inst.size - 1})"
          end
        else
          opcode = inst
          arg_count = 0
          unless opcode.kind_of? InstructionSet::OpCode
            opcode = InstructionSet[opcode]
          end
        end

        begin
          @iseq[@offset] = opcode.bytecode
          case arg_count
          when 1
            @iseq[@offset + 1] = inst[1].to_i
          when 2
            @iseq[@offset + 1] = inst[1].to_i
            @iseq[@offset + 2] = inst[2].to_i
          end
          @offset += (1 + arg_count)
        rescue => e
          raise ArgumentError, "Unable to encode #{inst.inspect}"
        end
      end

      private :encode

    end

    ##
    # Decodes the instruction sequence into an array of symbols

    def decode(symbols_only=true)
      enc = Encoder.new
      enc.decode_iseq(self, symbols_only)
    end
  end
end

##
# Defines all the bytecode instructions used by the VM.

module Rubinius
  class InstructionSet

    class OpCode
      attr_reader :args, :arg_count, :bytecode, :opcode, :size,
                  :stack, :stack_consumed, :stack_produced, :variable_stack,
                  :position, :produced_position, :stack_difference, :control_flow

      alias_method :name, :opcode
      alias_method :width, :size

      def initialize(opcode, bytecode, params)
        @opcode         = opcode
        @bytecode       = bytecode
        @args           = params[:args]
        @arg_count      = @args.size
        @size           = @arg_count + 1
        @position       = nil
        @produced_position = nil

        @stack_consumed, @stack_produced = params[:stack]
        if @stack_consumed.kind_of? Fixnum
          if @stack_produced.kind_of? Fixnum
            @variable_stack = false
            @stack_difference = @stack_produced - @stack_consumed
          else
            @variable_stack = true
            produced_extra, @produced_position, @produced_times = @stack_produced
            @stack_difference = produced_extra -  @stack_consumed
          end
        else
          @variable_stack = true
          extra, @position = @stack_consumed

          if @stack_produced.kind_of? Fixnum
            @stack_difference = @stack_produced - extra
          else
            produced_extra, @produced_position, @produced_times = @stack_produced
            @stack_difference = produced_extra - extra
          end
        end

        @control_flow = params[:control_flow]
      end

      def variable_stack?
        @variable_stack
      end

      def to_s
        @opcode.to_s
      end
    end


    # InstructionSet methods

    # Returns the opcode map.
    def self.opcodes_map
      @opcodes_map ||= Rubinius::LookupTable.new
    end

    # Returns an array of OpCode instances.
    def self.opcodes
      @opcodes ||= []
    end

    # Utility method for defining the opcodes.
    def self.opcode(id, name, params={})
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
  end

  ##
  # A list of bytecode instructions.


  class InstructionSequence
    def initialize(size)
      @opcodes = Tuple.new(size)
    end

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

    def show
      ip = 0
      decode.each do |inst|
        puts "%4s: %s" % [ip, inst.join(" ")]
        ip += inst.size
      end
    end
  end
end

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
  # Aids in decoding an instruction sequence into information
  # about each instruction
  #
  class InstructionDecoder
    def initialize(iseq)
      @iseq = iseq
    end

    ##
    # Decodes an InstructionSequence (which is essentially a an array of ints)
    # into an array whose elements are arrays of opcode symbols and 0-2 args,
    # depending on the opcode.

    def decode(symbols=false)
      stream = decode_between(0, @iseq.size)

      if symbols
        stream.each { |i| i[0] = i[0].opcode }
      end

      return stream
    end

    def decode_between(start, fin)
      offset = start

      stream = []

      while offset < fin
        inst = @iseq[offset]
        op = InstructionSet[inst]

        case op.arg_count
        when 0
          stream << [op]
        when 1
          stream << [op, @iseq[offset+1]]
        when 2
          stream << [op, @iseq[offset+1], @iseq[offset+2]]
        end

        offset += op.width
      end

      return stream
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

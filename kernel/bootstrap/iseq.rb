# -*- encoding: us-ascii -*-

module Rubinius
  class InstructionSequence
    def initialize(size)
      if size.kind_of? Tuple
        @opcodes = size
      else
        @opcodes = Tuple.new(size)
      end
    end

    attr_reader :opcodes

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
  end
end

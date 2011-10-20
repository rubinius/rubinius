Enumerator = Enumerable::Enumerator

module Enumerable
  class Enumerator
    def initialize(obj=undefined, iter=:each, *args, &block)
      if obj.equal? undefined
        raise ArgumentError, "Enumerator.new expects a block when called without arguments." unless block_given?

        obj = Generator.new(&block)
      end

      @object = obj
      @iter = iter.to_sym
      @args = args
      @generator = nil
    end

    def with_index(offset=0)
      if offset
        offset = Rubinius::Type.coerce_to offset, Integer, :to_int
      else
        offset = 0
      end

      return to_enum(:with_index, offset) unless block_given?

      each do |o|
        val = yield(o, offset)
        offset += 1
        val
      end
    end

    # A supporting class for Enumerator that allows for easy proxying to a Generator's yield.
    class Yielder
      def initialize(&block)
        raise LocalJumpError, "Expected a block to be given" unless block_given?

        @proc = block
      end

      # Yield multiple values.
      def yield(*args)
        @proc.call *args
      end

      # Chainable yield of a single value.
      def <<(value)
        self.yield value

        self
      end
    end

    # A supporting class for Enumerator that encloses iteration over a Yielder's generated data.
    class Generator
      # The block is given a yielder from which it can pass values for iteration.
      def initialize(&block)
        raise LocalJumpError, "Expected a block to be given" unless block_given?

        @proc = block
      end

      # Iterate over values given to the yielder in the Generator's proc.
      def each
        enclosed_yield = Proc.new { |*args| yield *args }

        @proc.call Yielder.new(&enclosed_yield)
      end
    end
  end
end

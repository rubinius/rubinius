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
      @lookahead = []
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

    # Returns the next object in the enumerator
    # and move the internal position forward.
    # When the position reached at the end,
    # internal position is rewound then StopIteration is raised.
    #
    # Note that enumeration sequence by next method
    # does not affect other non-external enumeration methods,
    # unless underlying iteration methods itself has side-effect, e.g. IO#each_line.
    #
    def next
      return @lookahead.shift unless @lookahead.empty?

      unless @generator
        # Allow #to_generator to return nil, indicating it has none for
        # this method.
        if @object.respond_to? :to_generator
          @generator = @object.to_generator(@iter)
        end

        if !@generator and gen = FiberGenerator
          @generator = gen.new(self)
        else
          @generator = ThreadGenerator.new(self, @object, @iter, @args)
        end
      end

      begin
        return @generator.next if @generator.next?
      rescue StopIteration
      end

      raise StopIteration, "iteration reached end"
    end

    def peek
      return @lookahead.first unless @lookahead.empty?
      item = self.next
      @lookahead << item
      item
    end

    def peek_values
      Array(self.peek)
    end

    # Rewinds the enumeration sequence by the next method.
    #
    # If the enclosed object responds to a "rewind" method, it is called.
    #
    def rewind
      @object.rewind if @object.respond_to? :rewind
      @generator.rewind if @generator
      @lookahead = []
      self
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

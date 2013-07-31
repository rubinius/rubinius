# -*- encoding: us-ascii -*-

Enumerator = Enumerable::Enumerator

module Enumerable
  class Enumerator
    def initialize(obj=undefined, iter=:each, *args, &block)
      if undefined.equal? obj
        raise ArgumentError, "Enumerator.new expects a block when called without arguments." unless block_given?

        obj = Generator.new(&block)
      end

      @object = obj
      @iter = iter.to_sym
      @args = args
      @generator = nil
      @lookahead = []

      self
    end
    private :initialize

    def with_index(offset=0)
      if offset
        offset = Rubinius::Type.coerce_to offset, Integer, :to_int
      else
        offset = 0
      end

      return to_enum(:with_index, offset) unless block_given?

      each do
        o = Rubinius.single_block_arg
        val = yield(o, offset)
        offset += 1
        val
      end
    end

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

      exception = StopIteration.new "iteration reached end"
      Rubinius.privately do
        exception.result = @generator.result
      end

      raise exception
    end

    def next_values
      Array(self.next)
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

    def rewind
      @object.rewind if @object.respond_to? :rewind
      @generator.rewind if @generator
      @lookahead = []
      self
    end

    class Yielder
      def initialize(&block)
        raise LocalJumpError, "Expected a block to be given" unless block_given?

        @proc = block

        self
      end
      private :initialize

      def yield(*args)
        @proc.call *args
      end

      def <<(value)
        self.yield value

        self
      end
    end

    class Generator
      include Enumerable
      def initialize(&block)
        raise LocalJumpError, "Expected a block to be given" unless block_given?

        @proc = block

        self
      end
      private :initialize

      def each
        enclosed_yield = Proc.new { |*args| yield *args }

        @proc.call Yielder.new(&enclosed_yield)
      end
    end
  end
end

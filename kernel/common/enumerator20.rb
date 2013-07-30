# -*- encoding: us-ascii -*-

module Enumerable
  class Enumerator

    def initialize(object_or_size=undefined, iter=:each, *args, &block)
      if block_given?
        if undefined.equal? object_or_size
          size = nil
        else
          size = object_or_size
        end

        object = Generator.new(&block)
      else
        if undefined.equal? object_or_size
          raise ArgumentError, "Enumerator#initialize requires a block when called without arguments"
        end

        object = object_or_size
      end

      @object = object
      @size = size
      @iter = Rubinius::Type.coerce_to iter, Symbol, :to_sym
      @args = args
      @generator = nil
      @lookahead = []

      self
    end
    private :initialize

    class Lazy < self
      def initialize(receiver, size=nil)
        raise ArgumentError, "Lazy#initialize requires a block" unless block_given?
        Rubinius.check_frozen

        super(size) do |yielder|
          receiver.each do |*args|
            yield yielder, *args
          end
        end

        self
      end
      private :initialize

      def map
        raise ArgumentError, 'Lazy#{map,collect} requires a block' unless block_given?

        Lazy.new(self, enumerator_size) do |yielder, *args|
          yielder.yield yield(*args)
        end
      end
      alias_method :collect, :map
    end

  end
end

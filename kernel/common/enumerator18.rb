# -*- encoding: us-ascii -*-

module Enumerable
  class Enumerator
    def initialize(obj, iter=:each, *args)
      @object = obj
      @iter = iter.to_sym
      @args = args
      @generator = nil

      self
    end
    private :initialize

    def next
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

      @generator.rewind

      raise StopIteration, "iteration reached end"
    end

    def rewind
      @object.rewind if @object.respond_to? :rewind
      @generator.rewind if @generator
      self
    end
  end
end

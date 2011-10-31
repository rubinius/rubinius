module Enumerable
  class Enumerator
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

    # Rewinds the enumeration sequence by the next method.
    #
    # If the enclosed object responds to a "rewind" method, it is called.
    #
    def rewind
      @object.rewind if @object.respond_to? :rewind
      @generator.rewind if @generator
      self
    end
  end
end

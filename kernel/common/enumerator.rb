# A class which provides a method `each' to be used as an Enumerable
# object.

module Enumerable
  class Enumerator
    include Enumerable

    def initialize(obj, iter=:each, *args)
      @object = obj
      @iter = iter.to_sym
      @args = args
    end

    def each(&block)
      @object.__send__(@iter, *@args, &block)
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
    # * Depends on continuations (via generator), not currently supported
    def next
      raise NotImplementedError, "no continuation support"

      require 'generator'
      @generator ||= Generator.new(self)
      raise StopIteration unless @generator.next?
      @generator.next
    end

    # Rewinds the enumeration sequence by the next method.
    #
    # If the enclosed object responds to a "rewind" method, it is called.
    #
    # * Depends on continuations (via generator), not currently supported
    def rewind
      raise NotImplementedError, "no continuation support"

      require 'generator'
      @object.rewind if @object.respond_to? :rewind
      @generator ||= Generator.new(self)
      @generator.rewind
      self
    end
  end
end

module Kernel
  def enum_for(method=:each, *args)
    Enumerable::Enumerator.new(self, method, *args)
  end

  alias_method :to_enum, :enum_for
end

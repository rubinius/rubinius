class Enumerable::Enumerator

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
    require 'generator'
    @generator ||= Generator.new(self)
    raise StopIteration unless @generator.next?
    @generator.next
  end unless method_defined? :next
  
  # Rewinds the enumeration sequence by the next method.
  # 
  # If the enclosed object responds to a "rewind" method, it is called.
  #
  # * Depends on continuations (via generator), not currently supported 
  def rewind
    @object.rewind if @object.respond_to? :rewind
    require 'generator'
    @generator ||= Generator.new(self)
    @generator.rewind
    self
  end unless method_defined? :rewind
  
  # Actually defined in Ruby 1.9 only, we define it here because it can be useful to implementers
  class Yielder
    def initialize(&block)
      @main_block = block
    end

    def each(&block)
      @final_block = block
      @main_block.call(self)
    end

    def yield(*arg)
      @final_block.call(*arg)
    end
  end
  
  def self.new_with_block(&block)
    new(Yielder.new(&block))
  end

end
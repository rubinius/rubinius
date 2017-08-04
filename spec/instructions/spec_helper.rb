require File.expand_path('../../spec_helper', __FILE__)

class InstructionSpec
  attr_accessor :name, :code

  def initialize(name, &block)
    @name = name
    @code = self.class.dynamic_method(name, &block)
    @code.tags |= Rubinius::Executable::Experimental
  end

  def run(*args, &block)
    send(@name, *args, &block)
  end
end

require File.expand_path('../../spec_helper', __FILE__)

class InstructionSpec
  attr_accessor :name, :code

  def initialize(name, &block)
    @name = name
    @code = self.class.dynamic_method(name, &block)
  end

  def run(*args)
    send(@name, *args)
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "MethodContext#stack_at" do
  before :each do
    @b = ContextSpecs::B.new
  end

  it "returns the value on the context operand stack at the specified depth" do
    @b.inspect_stack(3).should == 'First'
    @b.inspect_stack(4).should == :second
    @b.inspect_stack(5).should == 3
    @b.inspect_stack(6).should == [4,5,6]
  end

  it "throws an ArgumentError if depth is out of range" do
    lambda { @b.inspect_stack(0) }.should_not raise_error(ArgumentError)
    lambda { @b.inspect_stack(-1) }.should raise_error(ArgumentError)
    lambda { @b.inspect_stack(10) }.should raise_error(ArgumentError)
  end
end

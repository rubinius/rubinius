require File.dirname(__FILE__) + '/../../spec_helper'
describe "Proc#call" do
  it "invokes self" do
    Proc.new(def meth; "test!"; end).call.should == "test!"
  end

  it "sets self's parameters to the given values" do
    Proc.new(def meth(a, b); a + b; end).call(1, 2).should == 3
    Proc.new(def meth(*args); args; end).call(1, 2, 3, 4).should == [1, 2, 3, 4]
    Proc.new(def meth(_,*args); args; end).call(1, 2, 3).should == [2, 3]
  end

  it "raises an ArgumentError when called with too few arguments on a Proc created from a CompiledMethod" do
    lambda { Proc.new(def meth(a, b); [a, b]; end).call }.should raise_error(ArgumentError)
    lambda { Proc.new(def meth(a, b); [a, b]; end).call(1) }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError when called with too many arguments on a Proc created from a CompiledMethod" do
    lambda { Proc.new(def meth(a, b); [a, b]; end).call(1,2,3) }.should raise_error(ArgumentError)
  end

  it "treats a single Array argument as a single argument when called on a Proc created from a CompiledMethod" do
    Proc.new(def meth(a); [a]; end).call([1, 2]).should == [[1, 2]]
    lambda { Proc.new(def meth(a, b); [a, b]; end).call([1, 2]) }.should raise_error(ArgumentError)
  end
end

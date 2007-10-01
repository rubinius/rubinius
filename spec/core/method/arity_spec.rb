require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Method#arity" do
  before(:each) do
    @m = MethodSpecs::Methods.new
  end

  it "returns an indication of the number of arguments accepted by a method." do
    @m.method(:zero).arity.should == 0
    @m.method(:one).arity.should == 1
    @m.method(:two).arity.should == 2
  end

  it "if a variable number of arguments, return -(n-1), where n is the number of required arguments" do
    @m.method(:neg_one).arity.should == -1
    @m.method(:neg_two).arity.should == -2
    @m.method(:neg_three).arity.should == -3
  end
end

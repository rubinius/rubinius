require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Method#unbind" do
  before(:each) do
    @um = MethodSpecs::B.new.method(:baz).unbind
  end


  it "returns an UnboundMethod" do
    @um.kind_of?(UnboundMethod).should == true
  end

  it "should handle unbinding and binding of the method properly" do
    MethodSpecs::B.module_eval "def baz; false end"

    @um.bind(MethodSpecs::B.new).call(1, 2).should == MethodSpecs::B
    @um.bind(MethodSpecs::C.new).call(1, 2).should == MethodSpecs::C

    lambda {
      @um.bind(MethodSpecs::A.new).call(1, 2).should == MethodSpecs::A
    }.should raise_error(TypeError)
  end
end

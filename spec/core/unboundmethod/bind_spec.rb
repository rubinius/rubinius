require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "UnboundMethod#bind" do
  it "should handle unbinding and binding of the method properly" do
    um1 = UnboundMethodSpecs::B.new.method(:baz).unbind
    um2 = UnboundMethodSpecs::B.instance_method(:baz)

    [um1, um2].each do |um|
      UnboundMethodSpecs::B.module_eval "def baz; false end"

      um.bind(UnboundMethodSpecs::B.new).call(1, 2).should == UnboundMethodSpecs::B
      um.bind(UnboundMethodSpecs::C.new).call(1, 2).should == UnboundMethodSpecs::C

      lambda {
        um.bind(UnboundMethodSpecs::A.new).call(1, 2).should == UnboundMethodSpecs::A
      }.should raise_error(TypeError)
    end
  end
end

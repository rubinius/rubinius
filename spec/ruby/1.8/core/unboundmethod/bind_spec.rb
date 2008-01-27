require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "UnboundMethod#bind" do
  before :each do
    @um1 = UnboundMethodSpecs::B.new.method(:baz).unbind
    @um2 = UnboundMethodSpecs::B.instance_method(:baz)
    @um3 = UnboundMethodSpecs::Methods.new.method(:foo).unbind
  end

  it "handles unbinding and binding of the method properly" do
    [@um1, @um2].each do |um|
      UnboundMethodSpecs::B.module_eval "def baz; false end"

      um.bind(UnboundMethodSpecs::B.new).call(1, 2).should == UnboundMethodSpecs::B
      um.bind(UnboundMethodSpecs::C.new).call(1, 2).should == UnboundMethodSpecs::C
    end
  end

  it "should raise a TypeError if the object being bound to is not a kind_of? the class it originally was unbound from" do
    lambda {
      @um1.bind(UnboundMethodSpecs::A.new)
    }.should raise_error(TypeError, "bind argument must be an instance of UnboundMethodSpecs::B")

    lambda {
      @um3.bind(UnboundMethodSpecs::A.new)
    }.should raise_error(TypeError, "bind argument must be an instance of UnboundMethodSpecs::Methods")
  end
end

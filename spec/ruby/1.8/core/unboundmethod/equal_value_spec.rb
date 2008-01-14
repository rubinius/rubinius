require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "UnboundMethod#==" do
  before(:each) do
    @um1 = UnboundMethodSpecs::Methods.instance_method(:foo)
    @um2 = UnboundMethodSpecs::Methods.new.method(:foo).unbind
    
    @method_one = UnboundMethodSpecs::Methods.instance_method(:one)
    @method_two = UnboundMethodSpecs::Methods.instance_method(:two)
  end

  it "returns true if UnboundMethod's are the same" do
    (@um1 == @um1).should == true
    (@um1 == @um2).should == true
  end
  
  it "returns false if UnboundMethod's are not the same" do
    (@method_one == @method_two).should == false
    (@method_two == @method_one).should == false
  end

  it "returns true on aliased methods which are unbound" do
    alias_um1 = UnboundMethodSpecs::Methods.instance_method(:bar)
    alias_um2 = UnboundMethodSpecs::Methods.new.method(:bar).unbind

    (@um1 == alias_um1 && @um1 == alias_um2).should == true
  end
end

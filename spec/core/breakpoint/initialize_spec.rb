require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Breakpoint#initalize" do

  before do
    @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
  end

  it "converts a missing instruction pointer argument to 0" do
    bp = Breakpoint.new(@cm, nil) {}
    bp.ip.should == 0
    bp = Breakpoint.new(@cm) {}
    bp.ip.should == 0
  end

  it "throws an ArgumentError if the IP is out of range" do
    lambda { Breakpoint.new(@cm, -1) {} }.should raise_error(ArgumentError)
    lambda { Breakpoint.new(@cm, 1000) {} }.should raise_error(ArgumentError)
    lambda { Breakpoint.new(@cm, 10) {} }.should_not raise_error(ArgumentError)
  end

  it "throws an ArgumentError if the IP is not the address of an instruction" do
    lambda { Breakpoint.new(@cm, 14) {} }.should raise_error(ArgumentError)
  end

  it "throws an ArgumentError if no block is given" do
    lambda { Breakpoint.new(@cm) }.should raise_error(ArgumentError)
  end

  it "does not modify the compiled method instruction sequence" do
    pre = @cm.bytecodes.decode
    bp = Breakpoint.new(@cm, nil) {}
    bp.enabled?.should == false
    @cm.bytecodes.decode.should == pre
  end
end

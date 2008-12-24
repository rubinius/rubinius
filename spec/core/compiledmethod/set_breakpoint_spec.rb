require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/breakpoint_classes'

describe "CompiledMethod#set_breakpoint" do
  before :each do
    @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
  end

  it "sets a breakpoint flag at the location specified" do
    @cm.breakpoint?(0).should == false
    @cm.set_breakpoint(0)
    @cm.breakpoint?(0).should == true
  end

  it "raises an exception if an invalid IP is specified" do
    lambda { @cm.set_breakpoint(2) }.should raise_error(ArgumentError)
  end
end

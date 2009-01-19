require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/breakpoint_classes'

describe "CompiledMethod#clear_breakpoint" do
  before :each do
    @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
    @cm.compile
  end

  after :each do
    Rubinius::VM.debug_channel = nil
  end

  it "clears a breakpoint flag at the location specified" do
    @cm.breakpoint?(0).should == false
    @cm.set_breakpoint(0)
    @cm.breakpoint?(0).should == true
    @cm.clear_breakpoint(0)
    @cm.breakpoint?(0).should == false
  end

  it "raises an exception if an invalid IP is specified" do
    lambda { @cm.clear_breakpoint(1) }.should raise_error(ArgumentError)
  end

  it "causes execution to no longer be suspended when the breakpoint location is hit" do
    dbg = BreakpointSpecs::Debugger.new
    target = BreakpointSpecs::Debuggee.new
    target.counter.should == 5
    target.count
    target.counter.should == 15
    breakpoint_hit = false
    dbg.wait_for_breakpoint do
      breakpoint_hit = true
      target.counter.should == 25
    end
    target.count
    cm = BreakpointSpecs::Debuggee.instance_method(:count).compiled_method
    cm.set_breakpoint(0)
    target.count
    breakpoint_hit.should == true
    target.counter.should == 35

    cm.clear_breakpoint(0)
    breakpoint_hit = false
    target.count
    breakpoint_hit.should == false
    target.counter.should == 45
  end
end

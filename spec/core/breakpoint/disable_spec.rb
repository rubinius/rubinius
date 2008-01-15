require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Breakpoint#disable" do
  before :each do
    @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
    @cm.bytecodes = BreakpointSpecs::Debuggee.orig_bytecodes.dup

    @orig = @cm.bytecodes.decode[4]
    @bp = Breakpoint.new(@cm,8) {}  # The fifth instruction has an IP of 8
    @bp.enable
  end

  it "removes the yield_debugger instruction at the location specified by @ip" do
    dc = @cm.bytecodes.decode
    dc[4].first.opcode.should == :yield_debugger
    @bp.disable
    dc = @cm.bytecodes.decode
    dc[4].first.opcode.should_not == :yield_debugger
    dc[4].should == @orig
  end

  it "sets the enabled flag to false" do
    @bp.enabled?.should == true
    @bp.disable
    @bp.enabled?.should == false
  end
end
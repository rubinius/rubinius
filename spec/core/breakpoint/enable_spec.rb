require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Breakpoint#enable" do
  before :each do
    @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
    @cm.bytecodes = BreakpointSpecs::Debuggee.orig_bytecodes.dup
  end

  it "inserts a yield_debugger instruction at the location specified by @ip" do
    dc = @cm.bytecodes.decode
    dc[0].first.opcode.should_not == :yield_debugger
    bp = Breakpoint.new(@cm) {}
    bp.enable
    dc = @cm.bytecodes.decode
    dc[0].first.opcode.should == :yield_debugger
    bp.disable

    dc[4].first.opcode.should_not == :yield_debugger
    bp = Breakpoint.new(@cm,8) {}  # The fifth instruction has an IP of 8
    bp.enable
    dc = @cm.bytecodes.decode
    dc[4].first.opcode.should == :yield_debugger
  end

  it "sets the enabled flag to true" do
    bp = Breakpoint.new(@cm,8) {}
    bp.enabled?.should == false
    bp.enable
    bp.enabled?.should == true
  end
end
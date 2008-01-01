require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Breakpoint#disable" do
  before do
    @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
    @orig = @cm.bytecodes.decode[8]
    @bp = Breakpoint.new(@cm,8) {}
    @bp.enable
  end

  it "removes the yield_debugger instruction at the location specified by @ip" do
    dc = @cm.bytecodes.decode
    dc[8].first.should == :yield_debugger
    @bp.disable
    dc = @cm.bytecodes.decode
    dc[8].first.should_not == :yield_debugger
    dc[8].should == @orig
  end

  it "sets the enabled flag to false" do
    @bp.enabled?.should == true
    @bp.disable
    @bp.enabled?.should == false
  end
end
require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Breakpoint#disable" do
  before :each do
    @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
    @cm.iseq = BreakpointSpecs::Debuggee.orig_bytecodes.dup

    @orig = @cm.iseq.decode[4]
    @bp = GlobalBreakpoint.new(@cm,8) {}  # The fifth instruction has an IP of 8
    @bp.install
  end

  it "removes the yield_debugger instruction at the location specified by @ip" do
    dc = @cm.iseq.decode
    dc[4].first.should == :yield_debugger
    @bp.remove
    dc = @cm.iseq.decode
    dc[4].first.should_not == :yield_debugger
    dc[4].should == @orig
  end

  it "sets the enabled flag to false" do
    @bp.installed?.should == true
    @bp.remove
    @bp.installed?.should == false
  end
end
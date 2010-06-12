require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Breakpoint#enable" do
  before :each do
    @cm = BreakpointSpecs::Debuggee.instance_method(:simple_method).compiled_method
    @cm.iseq = BreakpointSpecs::Debuggee.orig_bytecodes.dup
  end

  it "inserts a yield_debugger instruction at the location specified by @ip" do
    dc = @cm.iseq.decode
    dc[0].first.should_not == :yield_debugger
    bp = GlobalBreakpoint.new(@cm) {}
    bp.install
    dc = @cm.iseq.decode
    dc[0].first.should == :yield_debugger
    bp.remove

    dc[4].first.should_not == :yield_debugger
    bp = GlobalBreakpoint.new(@cm,8) {}  # The fifth instruction has an IP of 8
    bp.install
    dc = @cm.iseq.decode
    dc[4].first.should == :yield_debugger
  end

  it "sets the enabled flag to true" do
    bp = GlobalBreakpoint.new(@cm,8) {}
    bp.installed?.should == false
    bp.install
    bp.installed?.should == true
  end
end
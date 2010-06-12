require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "StepBreakpoint#calculate_step_breakpoint" do

  before :all do
    @cm = BreakpointSpecs::Debuggee.instance_method(:step_method).compiled_method
  end

  def create_bp(selector, ip)
    @ctxt = ctxt = BreakpointSpecs::ContextStub.new(@cm, ip)
    @task = BreakpointSpecs::TaskStub.new(@ctxt)
    @step_bp = StepBreakpoint.new(@task, selector) {}
    @step_bp.instance_eval { @context = ctxt; @last_method = @cm; @last_ip = ip }
  end

  it "given a valid target IP within the method, returns the target IP" do
    create_bp({:ip => 12}, 0)
    @step_bp.calculate_target_breakpoint.should == 12
  end

  it "given a valid target line within the method, returns the first IP on that line" do
    create_bp({:line => 20}, 0)
    @step_bp.calculate_target_breakpoint.should == 8
  end

  it "given a valid line outside the current method, ???" do
    fail("Figure out what this should do")
  end

  it "given an invalid line for the source file, it raises an ArgumentError" do
    create_bp({:line => 0}, 0)
    lambda{ @step_bp.calculate_target_breakpoint }.should raise_error(ArgumentError)
    create_bp({:line => 9999}, 0)
    lambda{ @step_bp.calculate_target_breakpoint }.should raise_error(ArgumentError)
  end

  it "given an IP that is invalid, but within the valid range, it returns the next valid IP after" do
    create_bp({:ip => 15}, 0)
    @step_bp.calculate_target_breakpoint.should == 16
  end

  it "given an IP that is outside the valid range, it raises an ArgumentError" do
    create_bp({:ip => -1}, 0)
    lambda{ @step_bp.calculate_target_breakpoint }.should raise_error(ArgumentError)
    create_bp({:ip => 9999}, 0)
    lambda{ @step_bp.calculate_target_breakpoint }.should raise_error(ArgumentError)
  end

  it "given a step-out from a top-level context, it raises an ArgumentError" do
    create_bp({:out => 1}, 0)
    @ctxt.should_receive(:sender).and_return(nil)
    lambda{ @step_bp.calculate_target_breakpoint }.should raise_error(ArgumentError)
  end
end

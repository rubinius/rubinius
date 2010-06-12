require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "StepBreakpoint#calculate_next_breakpoint" do

  before :all do
    @cm = BreakpointSpecs::Debuggee.instance_method(:step_method).compiled_method
    @block_cm = @cm.literals.at(0)
    @if_cm = BreakpointSpecs::Debuggee.instance_method(:if_method).compiled_method
  end

  def create_bp(cm, selector, ip)
    @ctxt = ctxt = BreakpointSpecs::ContextStub.new(cm, ip)
    @task = BreakpointSpecs::TaskStub.new(@ctxt)
    @step_bp = StepBreakpoint.new(@task, selector) {}
    @step_bp.instance_eval { @context = ctxt; @last_method = cm; @last_ip = ip }
  end

  it "given an step ip with no intervening flow opcodes, returns the IP at the specified increment" do
    create_bp(@cm, {:step_by => :ip, :steps => 1}, 0)
    @step_bp.calculate_next_breakpoint.should == 3
    create_bp(@cm, {:step_by => :ip, :steps => 1}, 5)
    @step_bp.calculate_next_breakpoint.should == 7
    create_bp(@cm, {:step_by => :ip, :steps => 7}, 5)
    @step_bp.calculate_next_breakpoint.should == 16
  end

  it "given step line with no intervening flow opcodes, returns the first IP at the specified increment" do
    create_bp(@cm, {:step_by => :line, :steps => 2}, 5)
    @step_bp.calculate_next_breakpoint.should == 12
  end

  it "given a step ip count with an intervening send opcode, breaks at send, at start of method, and when steps reach 0" do
    create_bp(@cm, {:step_by => :ip, :step_type => :in, :steps => 8}, 14)
    @step_bp.calculate_next_breakpoint.should == 21
    @step_bp.steps.should == 3
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 21
    @step_bp.calculate_next_breakpoint.should == 0
    @step_bp.steps.should == 2
    @step_bp.break_type.should == :context_change
    @ctxt.method = @block_cm
    @ctxt.ip = 0
    @step_bp.calculate_next_breakpoint.should == 4
    @step_bp.break_type.should == :opcode_replacement
    @step_bp.steps.should == 0
  end

  it "given a step line count with an intervening send opcode, breaks at send, at start of method, and when steps reach 0" do
    create_bp(@cm, {:step_by => :line, :step_type => :in, :steps => 1}, 23)
    @step_bp.calculate_next_breakpoint.should == 26
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 26
    @step_bp.calculate_next_breakpoint.should == 0
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :context_change
    @ctxt.method = @block_cm
    @ctxt.ip = 0
    @step_bp.calculate_next_breakpoint.should == 5
    @step_bp.break_type.should == :opcode_replacement
    @step_bp.steps.should == 0
  end

  it "given a next ip count with an intervening send opcode, steps right over the send opcode" do
    create_bp(@cm, {:step_by => :ip, :step_type => :next, :steps => 8}, 14)
    @step_bp.calculate_next_breakpoint.should == 26
    @step_bp.steps.should == 0
    @step_bp.break_type.should == :opcode_replacement
  end

  it "given a next line count with an intervening send opcode, steps right over the send opcode" do
    create_bp(@block_cm, {:step_by => :line, :step_type => :next, :steps => 2}, 4)
    @step_bp.calculate_next_breakpoint.should == 19
    @step_bp.steps.should == 0
    @step_bp.break_type.should == :opcode_replacement
  end

  it "given a step ip count starting from a goto opcode, counts from target until steps reach 0" do
    create_bp(@cm, {:step_by => :ip, :steps => 2}, 29)
    @step_bp.calculate_next_breakpoint.should == 50
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 50
    @step_bp.calculate_next_breakpoint.should == 51
    @step_bp.steps.should == 0
    @step_bp.break_type.should == :opcode_replacement
  end

  it "given a step line count starting from a goto opcode, counts from target until steps reach 0" do
    create_bp(@cm, {:step_by => :line, :steps => 1}, 29)
    @step_bp.calculate_next_breakpoint.should == 50
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 50
    @step_bp.calculate_next_breakpoint.should == 51
    @step_bp.steps.should == 0
    @step_bp.break_type.should == :opcode_replacement
  end

  it "given a next ip count with an intervening goto opcode, breaks at goto, then counts from target until steps reach 0" do
    create_bp(@cm, {:step_by => :ip, :steps => 3}, 26)
    @step_bp.calculate_next_breakpoint.should == 29
    @step_bp.steps.should == 2
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 29
    @step_bp.calculate_next_breakpoint.should == 50
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 50
    @step_bp.calculate_next_breakpoint.should == 51
    @step_bp.steps.should == 0
    @step_bp.break_type.should == :opcode_replacement
  end

  it "given a next line count with an intervening goto opcode, breaks at goto, then counts from target until steps reach 0" do
    create_bp(@cm, {:step_by => :line, :steps => 1}, 26)
    @step_bp.calculate_next_breakpoint.should == 29
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 29
    @step_bp.calculate_next_breakpoint.should == 50
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 50
    @step_bp.calculate_next_breakpoint.should == 51
    @step_bp.steps.should == 0
    @step_bp.break_type.should == :opcode_replacement

    # Test case where stepping over goto and condition is false
    create_bp(@if_cm, {:step_by => :line, :steps =>1}, 3)
    @step_bp.calculate_next_breakpoint.should == 4
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 4
    @task.stack_value = false
    @step_bp.calculate_next_breakpoint.should == 11
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 11
    @step_bp.calculate_next_breakpoint.should == 11
    @step_bp.steps.should == 0
    @step_bp.break_type.should == :opcode_replacement
  end

  it "given a next ip count with an intervening conditional goto, breaks at goto and evaluates condition, then continues as appropriate" do
    create_bp(@cm, {:step_by => :ip, :steps => 3}, 39)
    @step_bp.calculate_next_breakpoint.should == 42
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 42
    @task.stack_value = true
    @step_bp.calculate_next_breakpoint.should == 44
    @step_bp.steps.should == 0
    @step_bp.break_type.should == :opcode_replacement

    create_bp(@cm, {:step_by => :ip, :steps => 3}, 39)
    @step_bp.calculate_next_breakpoint.should == 42
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 42
    @task.stack_value = false
    @step_bp.calculate_next_breakpoint.should == 48
    @step_bp.steps.should == 0
    @step_bp.break_type.should == :opcode_replacement
  end

  it "given a step ip count with an intervening return opcode, breaks at return, after call site, and when steps reach 0" do
    create_bp(@block_cm, {:step_by => :ip, :steps => 4}, 38)
    @ctxt.sender = sender = BreakpointSpecs::ContextStub.new(@cm, 29)
    @step_bp.calculate_next_breakpoint.should == 39
    @step_bp.steps.should == 3
    @step_bp.context.should == @ctxt
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 39
    @step_bp.calculate_next_breakpoint.should == 29
    @step_bp.context.should == sender
    @step_bp.steps.should == 2
    @step_bp.break_type.should == :opcode_replacement
  end

  it "given a step line count with an intervening return opcode, breaks at return, after call site, and when steps reach 0" do
    create_bp(@block_cm, {:step_by => :line, :steps => 1}, 38)
    @ctxt.sender = sender = BreakpointSpecs::ContextStub.new(@cm, 29)
    @step_bp.calculate_next_breakpoint.should == 39
    @step_bp.steps.should == 1
    @step_bp.context.should == @ctxt
    @step_bp.break_type.should == :opcode_replacement
    @ctxt.ip = 39
    @step_bp.calculate_next_breakpoint.should == 29
    @step_bp.context.should == sender
    @step_bp.steps.should == 1
    @step_bp.break_type.should == :opcode_replacement
    @step_bp.calculate_next_breakpoint.should == 29
    @step_bp.context.should == sender
    @step_bp.steps.should == 0
    @step_bp.break_type.should == :opcode_replacement
  end
end

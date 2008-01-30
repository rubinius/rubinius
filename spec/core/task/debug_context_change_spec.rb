require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Task#debug_context_change=" do
  before do
    @a = TaskSpecs::A.new
    @listener = TaskSpecs::Listener.new
  end

  after do
    Rubinius::VM.debug_channel = nil
  end

  it "sets the debug_context_change flag as specified" do
    task = Task.current
    task.debug_context_change.should_not == true
    task.debug_context_change = true
    f = task.debug_context_change
    task.debug_context_change = false
    f.should == true
    task.debug_context_change = false
  end
end

describe "Task#debug_context_change" do
  before do
    @a = TaskSpecs::A.new
    @listener = TaskSpecs::Listener.new
  end

  after do
    Rubinius::VM.debug_channel = nil
  end

  it "returns the current state of the debug_context_change flag" do
    Task.current.debug_context_change.should_not == true
    t = nil
    f = nil
    @listener.wait_for_breakpoint do |task|
      task.debug_context_change = true
      t = task.debug_context_change
      task.debug_context_change = false
      f = task.debug_context_change
    end
    @a.yield_debugger
    t.should == true
    f.should == false
  end
end

describe "Task#debug_context_change behavior" do
  before do
    @a = TaskSpecs::A.new
    @listener = TaskSpecs::Listener.new
  end

  after do
    Rubinius::VM.debug_channel = nil
  end


  it "yields to the debugger immediately following a method send" do
    @listener.wait_for_breakpoint
    @a.call_mthd false
    @listener.get_breakpoint_method.should == nil

    @a.call_mthd true
    @listener.get_breakpoint_method.should == :simple_method
  end

  it "yields to the debugger immediately following a block send" do
    @listener.wait_for_breakpoint
    @a.call_block(false) {}
    @listener.get_breakpoint_method.should == nil

    @a.call_block(true) {}
    @listener.get_breakpoint_method.should == :__block__
  end

  it "yields to the debugger immediately following a method return" do
    @listener.wait_for_breakpoint
    @a.mthd_return false
    @listener.get_breakpoint_method.should == nil

    @a.mthd_return true
    @listener.get_breakpoint_method.should == :mthd_return
  end

  it "yields to the debugger immediately following a block return" do
    @listener.wait_for_breakpoint
    @a.block_return false
    @listener.get_breakpoint_method.should == nil

    @a.block_return true
    @listener.get_breakpoint_method.should == :each
  end

  it "yields to the debugger at the point an exception is raised" do
    @listener.wait_for_breakpoint
    lambda { @a.raise_exc false }.should raise_error(RuntimeError)
    @listener.get_breakpoint_method.should == nil

    lambda { @a.raise_exc true }.should raise_error(RuntimeError)
    @listener.get_breakpoint_method.should == :raise_exc
  end
end
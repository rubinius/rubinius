require File.expand_path('../spec_helper.rb', __FILE__)

describe "Debugger#quit!" do
  before :each do
    @dbg = Debugger.instance
    @cm = DebuggerSpecs::Debuggee.instance_method(:some_method).compiled_method
  end

  it "removes all breakpoints" do
    @dbg.set_breakpoint @cm, 0
    @dbg.breakpoints.size.should == 1
    @dbg.quit!
    @dbg.breakpoints.size.should == 0
  end

  it "clears the VM debug channel" do
    @dbg.quit!
    Rubinius::VM.debug_channel.should == nil
  end

  it "clears the Debugger singleton instance" do
    Debugger.instance_eval { @instance.should_not == nil }
    @dbg.quit!
    Debugger.instance_eval { @instance.should == nil }
  end

  it "interrupts the Debugger thread if it is sleeping" do
    @dbg.instance_eval { @breakpoint_listener.status.should == "sleep" }
    @dbg.quit!
    @dbg.instance_eval { @breakpoint_listener.status.should == false }
  end
end
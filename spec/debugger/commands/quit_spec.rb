require File.dirname(__FILE__) + "/../spec_helper.rb"

describe "Quit#execute" do
  before :each do
    @dbg = mock('Debugger')
    @quit = Debugger::Quit.new
  end

  it "sets the quit flag on Debugger so the debugger knows to exit" do
    @dbg.should_receive(:quit!)
    @quit.execute(@dbg, nil)
  end
end
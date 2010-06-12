require File.expand_path('../../spec_helper.rb', __FILE__)
require 'debugger/command'

describe "Quit#execute" do
  before :each do
    @dbg = mock('Debugger')
    @interface = mock('Interface')
    @quit = Debugger::Quit.new
  end

  it "sets the quit flag on Debugger so the debugger knows to exit" do
    @dbg.should_receive(:quit!)
    @interface.should_receive(:done!)
    @quit.execute(@dbg, @interface, nil)
  end
end
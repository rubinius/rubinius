require File.expand_path('../../spec_helper.rb', __FILE__)
require 'debugger/interface'

describe "Help#execute" do
  before :each do
    @interface = Debugger::CmdLineInterface.new
    @interface.load_commands
  end

  it "returns an array of commands and descriptions" do
    hlp = Debugger::Help.new
    out = hlp.execute nil, @interface, [nil,nil]
    out.class.should == Debugger::Output
  end
end
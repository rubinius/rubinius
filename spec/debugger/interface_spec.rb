require File.dirname(__FILE__) + "/spec_helper"
require 'debugger/interface'
require 'stringio'

describe "Debugger::CmdLineInterface#handle_exception" do
  before :each do
    @out, @err = StringIO.new, StringIO.new
    @cl = Debugger::CmdLineInterface.new(@out, @err)
    @dbg = mock 'Debugger'
  end
  
  it "omits stack frames above Debugger::Interface#process_command" do
    @cl.eval_context = MethodContext.current
    @cl.process_command @dbg, 'blah'
    @err.string.should_not =~ /process_command/
  end
end

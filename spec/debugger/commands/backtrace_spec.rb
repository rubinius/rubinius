require File.expand_path('../../spec_helper.rb', __FILE__)
require 'debugger/interface'

describe "ShowBacktrace#execute" do
  before :each do
    @dbg = mock('Debugger')
    @interface = mock('Interface')
    @bt = Debugger::ShowBacktrace.new
  end

  it "generates a backtrace from the debug context" do
    ctxt = MethodContext.current
    @interface.should_receive(:debug_context).and_return ctxt
    @interface.should_receive(:eval_context).at_least(:once).and_return ctxt
    @bt.execute(@dbg, @interface, nil).to_s.should =~ /backtrace_spec/
  end

  it "outputs a line marker alongside the eval context" do
    ctxt = MethodContext.current
    eval_ctxt = ctxt.sender
    @interface.should_receive(:debug_context).and_return ctxt
    @interface.should_receive(:eval_context).at_least(:once).and_return eval_ctxt
    out = @bt.execute(@dbg, @interface, nil)
    out.to_s.should =~ /backtrace_spec/
    out.to_s.should =~ /=>.*eval\.rb/
  end
end

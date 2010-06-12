require File.expand_path('../../spec_helper.rb', __FILE__)
require 'debugger/interface'

describe "UpFrame#execute" do
  before :each do
    @dbg = mock('Debugger')
    @interface = mock('Interface')
    @up = Debugger::UpFrame.new
  end

  it "moves the eval context up the specified number of frames" do
    ctxt = MethodContext.current
    @interface.should_receive(:eval_context).and_return ctxt
    @interface.should_receive(:debug_context).and_return ctxt
    @interface.should_receive(:eval_context=).with(ctxt.sender.sender)
    @up.execute(@dbg, @interface, @up.command_regexp.match("up 2"))
  end

  it "does not move the eval context past the top-most frame" do
    ctxt = MethodContext.current
    @interface.should_receive(:eval_context).and_return ctxt
    @interface.should_receive(:debug_context).and_return ctxt
    @interface.should_receive(:eval_context=).with(ctxt.context_stack.last)
    @up.execute(@dbg, @interface, @up.command_regexp.match("up 200"))
  end
end

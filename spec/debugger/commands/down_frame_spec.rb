require File.expand_path('../../spec_helper.rb', __FILE__)
require 'debugger/interface'

describe "DownFrame#execute" do
  before :each do
    @dbg = mock('Debugger')
    @interface = mock('Interface')
    @up = Debugger::DownFrame.new
  end

  it "moves the eval context down the specified number of frames" do
    ctxt = MethodContext.current
    @interface.should_receive(:eval_context).at_least(:once).and_return ctxt.sender.sender.sender
    @interface.should_receive(:debug_context).at_least(:once).and_return ctxt
    @interface.should_receive(:eval_context=).with(ctxt.sender)
    @up.execute(@dbg, @interface, @up.command_regexp.match("down 2"))
  end

  it "does not move the eval context past the current frame" do
    ctxt = MethodContext.current
    @interface.should_receive(:eval_context).at_least(:once).and_return ctxt.sender
    @interface.should_receive(:debug_context).at_least(:once).and_return ctxt
    @interface.should_receive(:eval_context=).with(ctxt)
    @up.execute(@dbg, @interface, @up.command_regexp.match("down 2"))
  end
end

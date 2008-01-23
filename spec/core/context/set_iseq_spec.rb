require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "MethodContext#set_iseq" do
  before do
    @cm = ContextSpecs::A.ret_7_cm
    @cm.bytecodes = ContextSpecs::A.orig_bytecodes.dup
    @cm.compile
    @a = ContextSpecs::A.new
    @listener = ContextSpecs::Listener.new
  end

  after do
    Rubinius::VM.debug_channel = nil
  end

  it "changes the iseq for the receiving context only" do
    @listener.wait_for_breakpoint
    @a.ret_7.should == 7
    # Setting the iseq for a context changes that context's actions
    @listener.wait_for_breakpoint do |ctxt|
      ctxt.set_iseq @a.change_ret(5)
    end
    @a.ret_7.should == 5
    # Another context for the same method is unaffected
    @listener.wait_for_breakpoint
    @a.ret_7.should == 7
  end

  it "insulates the receiving context from changes to the compiled method" do
    @listener.wait_for_breakpoint
    @a.ret_7.should == 7
    # Recompilation normally impacts any contexts using the method,
    # but should not if set_iseq has been used
    @listener.wait_for_breakpoint do |ctxt|
      ctxt.set_iseq @a.change_ret(3)
      @cm.bytecodes = @a.change_ret(8)
      @cm.compile
    end
    @a.ret_7.should == 3
    # Other contexts will pick up the re-compiled change
    @listener.wait_for_breakpoint
    @a.ret_7.should == 8
  end

  it "changes to the iseq persist across calls to blocks" do
    @listener.wait_for_breakpoint
    @a.ret_7.should == 7
    @a.called_block?.should == nil
    # Recompilation normally impacts any contexts using the method,
    # but should not if set_iseq has been used
    @listener.wait_for_breakpoint do |ctxt|
      ctxt.set_iseq @a.change_ret(3)
    end
    @a.ret_7(true, false).should == 3
    @a.called_block?.should == true
  end

  it "changes to the iseq persist across calls to other methods" do
    @listener.wait_for_breakpoint
    @a.ret_7.should == 7
    @a.called_from_ret_7?.should == nil
    # Recompilation normally impacts any contexts using the method,
    # but should not if set_iseq has been used
    @listener.wait_for_breakpoint do |ctxt|
      ctxt.set_iseq @a.change_ret(9)
    end
    @a.ret_7(false, true).should == 9
    @a.called_from_ret_7?.should == true
  end

end
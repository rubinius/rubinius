require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "MethodContext#reload_method" do
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

  it "modifying the CompiledMethod bytecodes without compiling does not effect the context" do
    @listener.wait_for_breakpoint
    @a.ret_7.should == 7
    @listener.wait_for_breakpoint do |ctxt|
      @cm.bytecodes = @a.change_ret(5)
    end
    @a.ret_7.should == 7
  end

  it "modifying the CompiledMethod bytecodes and compiling updates the context" do
    @listener.wait_for_breakpoint
    @a.ret_7.should == 7
    @listener.wait_for_breakpoint do |ctxt|
      @cm.bytecodes = @a.change_ret(5)
      @cm.compile
    end
    @a.ret_7.should == 5
  end

  it "reloads the compiled instruction sequence from the context's CompiledMethod" do
    @listener.wait_for_breakpoint
    @a.ret_7.should == 7
    @listener.wait_for_breakpoint do |ctxt|
      ctxt.set_iseq @a.change_ret(5)
    end
    @a.ret_7.should == 5
    @listener.wait_for_breakpoint
    @a.ret_7.should == 7
    @listener.wait_for_breakpoint do |ctxt|
      ctxt.set_iseq @a.change_ret(5)
      ctxt.reload_method
    end
    @a.ret_7.should == 7
  end
end
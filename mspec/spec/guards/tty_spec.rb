require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/guards/tty'

describe Object, "#with_tty" do
  before :each do
    ScratchPad.clear
  end

  it "yields if STDOUT is a TTY" do
    STDOUT.should_receive(:tty?).and_return(true)
    with_tty { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield
  end

  it "does not yield if STDOUT is not a TTY" do
    STDOUT.should_receive(:tty?).and_return(false)
    with_tty { ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield
  end
end

require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/guards/extensions'

describe Object, "#extended_on" do
  before :each do
    @guard = ExtensionsGuard.new
    ExtensionsGuard.stub!(:new).and_return(@guard)
    ScratchPad.clear
  end

  it "does not yield when #implementation? and #platform? return false" do
    @guard.stub!(:implementation?).and_return(false)
    @guard.stub!(:platform?).and_return(false)
    extended_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield
  end

  it "yields when #implementation? or #platform? return true" do
    @guard.stub!(:implementation?).and_return(true)
    @guard.stub!(:platform?).and_return(false)
    extended_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield

    @guard.stub!(:implementation?).and_return(false)
    @guard.stub!(:platform?).and_return(true)
    extended_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield
  end

  it "yields when #implementation? and #platform? return true" do
    @guard.stub!(:implementation?).and_return(true)
    @guard.stub!(:platform?).and_return(true)
    extended_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield
  end
end

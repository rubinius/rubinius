require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/guards/support'

describe Object, "#not_supported_on" do
  before :each do
    @guard = SupportedGuard.new
    SupportedGuard.stub!(:new).and_return(@guard)
    ScratchPad.clear
  end

  it "does not yield when #implementation? returns true" do
    @guard.stub!(:implementation?).and_return(true)
    not_supported_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield
  end

  it "yields when #implementation? returns false" do
    @guard.stub!(:implementation?).and_return(false)
    not_supported_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield
  end
end

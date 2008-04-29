require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/guards/compliance'

describe Object, "#compliant_on" do
  before :each do
    @guard = ComplianceGuard.new
    ComplianceGuard.stub!(:new).and_return(@guard)
    ScratchPad.clear
  end

  it "does not yield when #implementation? and #platform? return false" do
    @guard.stub!(:implementation?).and_return(false)
    @guard.stub!(:platform?).and_return(false)
    compliant_on(:rbx) { @ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield
  end

  it "yields when #implementation? or #platform? return true" do
    @guard.stub!(:implementation?).and_return(true)
    @guard.stub!(:platform?).and_return(false)
    compliant_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield

    @guard.stub!(:implementation?).and_return(false)
    @guard.stub!(:platform?).and_return(true)
    compliant_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield
  end

  it "yields when #implementation? and #platform? return true" do
    @guard.stub!(:implementation?).and_return(true)
    @guard.stub!(:platform?).and_return(true)
    compliant_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield
  end
end

describe Object, "#not_compliant_on" do
  before :each do
    @guard = ComplianceGuard.new
    ComplianceGuard.stub!(:new).and_return(@guard)
    ScratchPad.clear
  end

  it "does not yield when #implementation? and #platform? return true" do
    @guard.stub!(:implementation?).and_return(true)
    @guard.stub!(:platform?).and_return(true)
    not_compliant_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield
  end

  it "does not yield when #implementation? or #platform? return true" do
    @guard.stub!(:implementation?).and_return(true)
    @guard.stub!(:platform?).and_return(false)
    not_compliant_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield

    @guard.stub!(:implementation?).and_return(false)
    @guard.stub!(:platform?).and_return(true)
    not_compliant_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield
  end

  it "yields when #implementation? and #platform? return false" do
    @guard.stub!(:implementation?).and_return(false)
    @guard.stub!(:platform?).and_return(false)
    not_compliant_on(:rbx) { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield
  end
end

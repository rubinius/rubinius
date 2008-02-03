require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/guards/extensions'

describe Object, "#extended_on" do
  before :each do
    @guard = ExtensionsGuard.new
    ExtensionsGuard.stub!(:new).and_return(@guard)
  end
  
  it "does not yield when #implementation? and #platform? return false" do
    @guard.stub!(:implementation?).and_return(false)
    @guard.stub!(:platform?).and_return(false)
    extended_on(:rbx) { @record = :yield }
    @record.should_not == :yield
  end
  
  it "yields when #implementation? or #platform? return true" do
    @guard.stub!(:implementation?).and_return(true)
    @guard.stub!(:platform?).and_return(false)
    extended_on(:rbx) { @record = :yield }
    @record.should == :yield

    @guard.stub!(:implementation?).and_return(false)
    @guard.stub!(:platform?).and_return(true)
    extended_on(:rbx) { @record = :yield }
    @record.should == :yield
  end
  
  it "yields when #implementation? and #platform? return true" do
    @guard.stub!(:implementation?).and_return(true)
    @guard.stub!(:platform?).and_return(true)
    extended_on(:rbx) { @record = :yield }
    @record.should == :yield
  end
end

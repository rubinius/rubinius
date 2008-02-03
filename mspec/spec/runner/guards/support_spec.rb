require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/guards/support'

describe Object, "#not_supported_on" do
  before :each do
    @guard = SupportedGuard.new
    SupportedGuard.stub!(:new).and_return(@guard)
  end
  
  it "does not yield when #implementation? returns true" do
    @guard.stub!(:implementation?).and_return(true)
    not_supported_on(:rbx) { @record = :yield }
    @record.should_not == :yield
  end

  it "yields when #implementation? returns false" do
    @guard.stub!(:implementation?).and_return(false)
    not_supported_on(:rbx) { @record = :yield }
    @record.should == :yield
  end
end

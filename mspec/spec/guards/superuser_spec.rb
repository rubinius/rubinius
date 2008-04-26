require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/guards/superuser'

describe Object, "#as_superuser" do
  before :each do
    @guard = SuperUserGuard.new
    SuperUserGuard.stub!(:new).and_return(@guard)
    ScratchPad.clear
  end

  it "does not yield when Process.euid is not 0" do
    Process.stub!(:euid).and_return(501)
    as_superuser { ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield
  end

  it "yields when Process.euid is 0" do
    Process.stub!(:euid).and_return(0)
    as_superuser { ScratchPad.record :yield }
    ScratchPad.recorded.should == :yield
  end
end

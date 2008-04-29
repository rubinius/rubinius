require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/guards/quarantine'

describe QuarantineGuard, "#match?" do
  it "returns false" do
    QuarantineGuard.new.match?.should == false
  end
end

describe Object, "#quarantine!" do
  before :each do
    ScratchPad.clear
  end

  it "does not yield" do
    quarantine! { ScratchPad.record :yield }
    ScratchPad.recorded.should_not == :yield
  end
end

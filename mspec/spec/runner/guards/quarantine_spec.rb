require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/guards/quarantine'

describe QuarantineGuard, "#match?" do
  it "returns false" do
    QuarantineGuard.new.match?.should == false
  end
end

describe Object, "#quarantine!" do
  it "does not yield" do
    quarantine! { @record = :yield }
    @record.should_not == :yield
  end
end

require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/guards/quarantine'

describe QuarantineGuard, "#match?" do
  it "returns false" do
    QuarantineGuard.new.match?.should == false
  end
end

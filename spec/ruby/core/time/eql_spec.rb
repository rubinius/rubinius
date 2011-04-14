require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)

describe "Time#eql?" do
  it "returns true iff time is equal in seconds and usecs to other time" do
    Time.at(100, 100).should eql(Time.at(100, 100))
    Time.at(100, 100).should_not eql(Time.at(100, 99))
    Time.at(100, 100).should_not eql(Time.at(99, 100))
  end

  it "returns false when given a non-time value" do
    Time.at(100, 100).should_not eql("100")
    Time.at(100, 100).should_not eql(100)
    Time.at(100, 100).should_not eql(100.1)
  end
end

require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)

describe "Time#usec" do
  it "returns the microseconds for time" do
    Time.at(0).usec.should == 0
    (Time.at(1.1) + 0.9).usec.should == 0
    (Time.at(1.1) - 0.2).usec.should == 900000
  end

  ruby_version_is "1.9" do
    it "returns the microseconds for time created by Time#local" do
      Time.local(1,2,3,4,5,Rational(6.78)).usec.should == 780000
    end
  end
end

require File.expand_path('../../../spec_helper', __FILE__)

describe "Time#to_r" do
  ruby_version_is "1.9" do
    it "returns the rational number of seconds + usecs since the epoch" do
      Time.at(1.1).to_r.should == 1.1.to_r
    end

    it "returns the numerator of the rational number when the denominator is 1" do
      Time.at(2).to_r.should == 2
    end
  end
end

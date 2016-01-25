require File.expand_path('../../../spec_helper', __FILE__)

describe "Rational#positive?" do
  it "returns true if self is greater than 0" do
    Rational(1, 2).positive?.should == true
  end

  it "returns false if self is 0" do
    Rational(0).positive?.should == false
  end

  it "returns false if self is less than 0" do
    Rational(-3, 4).positive?.should == false
  end
end

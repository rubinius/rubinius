require File.expand_path('../../../spec_helper', __FILE__)

describe "Rational#negative?" do
  it "returns true if self is less than 0" do
    Rational(-3, 4).negative?.should == true
  end

  it "returns false if self is 0" do
    Rational(0).negative?.should == false
  end

  it "returns false if self is greater than 0" do
    Rational(1, 2).negative?.should == false
  end
end

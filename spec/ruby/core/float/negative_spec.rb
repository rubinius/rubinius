require File.expand_path('../../../spec_helper', __FILE__)

describe "Float#negative?" do
  it "returns true if self is less than 0.0" do
    numbers = [
      -1.0,
      -(0.0.next_float),
      -Float::INFINITY
    ]

    numbers.each do |number|
      number.negative?.should == true
    end
  end

  it "returns false if self is greater than or equal to 0.0" do
    numbers = [
      +1.0,
      +0.0,
      -0.0,
      +(0.0.next_float),
      +Float::INFINITY,
      Float::NAN
    ]

    numbers.each do |number|
      number.negative?.should == false
    end
  end
end

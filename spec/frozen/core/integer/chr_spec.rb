require File.dirname(__FILE__) + '/../../spec_helper'

describe "Integer#chr" do
  it "returns a string containing the ASCII character represented by self" do
    [82.chr, 117.chr, 98.chr, 105.chr, 110.chr, 105.chr, 117.chr, 115.chr, 
     32.chr, 
     114.chr, 111.chr, 99.chr, 107.chr, 115.chr].should == 
      ["R", "u", "b", "i", "n", "i", "u", "s", " ", "r", "o", "c", "k", "s"]
  end

  it "returns a new String for each call" do
    82.chr.should_not equal(82.chr)
  end
  
  it "raises a RangeError if self is out of the ASCII character range" do
    lambda { -1.chr }.should raise_error(RangeError)
    lambda { -300.chr }.should raise_error(RangeError)
    lambda { (-bignum_value).chr }.should raise_error(RangeError)

    lambda { 300.chr }.should raise_error(RangeError)
    lambda { bignum_value.chr }.should raise_error(RangeError)
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#^" do
  it "returns self bitwise EXCLUSIVE OR other" do
    (3 ^ 5).should == 6
    (-2 ^ -255).should == 255
  end
  
  it "coerces fixnum and return self bitwise EXCLUSIVE OR other" do
    (-7 ^ 15.2).should == -10
    (5 ^ 0xffffffff).should == 4294967290
  end
end

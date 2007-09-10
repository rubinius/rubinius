require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#>=" do
  it "returns true if self is greater than or equal to other" do
    (-50 >= -50).should == true
    (14 >= 2).should == true
    (900 >= 901).should == false
  end

  it "coerces fixnum and return true if self is greater than or equal to other" do
    (-50 >= -50).should == true
    (14 >= 2.5).should == true
    (900 >= 0xffffffff).should == false
  end
end

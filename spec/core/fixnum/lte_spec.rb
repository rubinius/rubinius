require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#<=" do
  it "returns true if self is less than or equal to other" do
    (2 <= 2).should == true
  end

  it "coerces fixnum and return true if self is less than or equal to other" do
    (7 <= 5.4).should == false
    (11 <= 0xffffffff).should == true
  end
end

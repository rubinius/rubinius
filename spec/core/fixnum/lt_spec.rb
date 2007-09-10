require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#<" do
  it "returns true if self is less than other" do
    (-1 < 0).should == true
  end
  
  it "coerces fixnum and return true if self is less than other" do
    (3 < -5.2).should == false
    (9 < 0xffffffff).should == true
  end
end

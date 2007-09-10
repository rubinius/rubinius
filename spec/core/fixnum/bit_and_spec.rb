require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#&" do
  it "returns self bitwise AND other" do
    (2010 & 5).should == 0
    (65535 & 1).should == 1
  end
  
  it "coerces fixnum and return self bitwise AND other" do
    (256 & 16).should == 0
    (0xffff & 0xffffffff).should == 65535
  end
end

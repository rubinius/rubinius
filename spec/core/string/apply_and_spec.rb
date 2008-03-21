require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#apply_and!" do
  it "applies the operation self[i] &&= other[i]" do
    a = "\000\000\001\001"
    b = "\000\001\000\001"
    a.apply_and! b
    a.should == "\000\000\000\001"
  end

  it "does not exceed the shorter of the two strings" do
    a = "\001\000\001"
    b = "\001"
    a.apply_and! b
    a.should == "\001\000\001"

    a = "\001"
    b = "\001\000\001"
    a.apply_and! b
    a.should == "\001"
  end
end

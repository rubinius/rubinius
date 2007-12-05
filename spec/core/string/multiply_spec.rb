require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#*" do
  it "returns a new string containing count copies of self" do
    ("cool" * 0).should == ""
    ("cool" * 1).should == "cool"
    ("cool" * 3).should == "coolcoolcool"
  end
  
  it "tries to convert the given argument to an integer using to_int" do
    ("cool" * 3.1).should == "coolcoolcool"
    ("a" * 3.999).should == "aaa"
    
    a = Object.new
    def a.to_int() 4; end
    ("a" * a).should == "aaaa"
    
    a = Object.new
    a.should_receive(:respond_to?).with(:to_int).and_return(true)
    a.should_receive(:method_missing).with(:to_int).and_return(4)
    ("a" * a).should == "aaaa"    
  end
  
  it "raises an ArgumentError when given integer is negative" do
    lambda { "cool" * -3    }.should raise_error(ArgumentError)
    lambda { "cool" * -3.14 }.should raise_error(ArgumentError)
  end
  
  it "raises a RangeError when given integer is a Bignum" do
    lambda { "cool" * 9999999999 }.should raise_error(RangeError)
  end
  
  it "returns subclass instances" do
    (MyString.new("cool") * 0).class.should == MyString
    (MyString.new("cool") * 1).class.should == MyString
    (MyString.new("cool") * 2).class.should == MyString
  end
  
  it "always taints the result when self is tainted" do
    ["", "OK", MyString.new(""), MyString.new("OK")].each do |str|
      str.taint

      [0, 1, 2].each do |arg|
        (str * arg).tainted?.should == true
      end
    end
  end
end

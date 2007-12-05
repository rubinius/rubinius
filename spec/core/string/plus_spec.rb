require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#+" do
  it "returns a new string containing the given string concatenated to self" do
    ("" + "").should == ""
    ("" + "Hello").should == "Hello"
    ("Hello" + "").should == "Hello"
    ("Ruby !" + "= Rubinius").should == "Ruby != Rubinius"
  end
  
  it "converts its argument to a string using to_str" do
    c = Object.new
    def c.to_str() "aaa" end
    
    ("a" + c).should == "aaaa"

    c = Object.new
    c.should_receive(:respond_to?).with(:to_str).any_number_of_times.and_return(true)
    c.should_receive(:method_missing).with(:to_str).and_return("aaa")
    ("a" + c).should == "aaaa"
  end
  
  it "doesn't return subclass instances" do
    (MyString.new("hello") + "").class.should == String
    (MyString.new("hello") + "foo").class.should == String
    (MyString.new("hello") + MyString.new("foo")).class.should == String
    (MyString.new("hello") + MyString.new("")).class.should == String
    (MyString.new("") + MyString.new("")).class.should == String
    ("hello" + MyString.new("foo")).class.should == String
    ("hello" + MyString.new("")).class.should == String
  end
  
  it "always taints the result when self or other is tainted" do
    strs = ["", "OK", MyString.new(""), MyString.new("OK")]
    strs += strs.map { |s| s.dup.taint }
    
    strs.each do |str|
      strs.each do |other|
        (str + other).tainted?.should == (str.tainted? | other.tainted?)
      end
    end
  end
end

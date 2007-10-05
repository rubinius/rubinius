require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#insert with index, other" do
  it "inserts other before the character at the given index" do
    "abcd".insert(0, 'X').should == "Xabcd"
    "abcd".insert(3, 'X').should == "abcXd"
    "abcd".insert(4, 'X').should == "abcdX"
  end
  
  it "modifies self in place" do
    a = "abcd"
    a.insert(4, 'X').should == "abcdX"
    a.should == "abcdX"
  end
  
  it "inserts after the given character on an negative count" do
    "abcd".insert(-5, 'X').should == "Xabcd"
    "abcd".insert(-3, 'X').should == "abXcd"
    "abcd".insert(-1, 'X').should == "abcdX"
  end
  
  it "raises an IndexError if the index is beyond string" do
    should_raise(IndexError) { "abcd".insert(5, 'X') }
    should_raise(IndexError) { "abcd".insert(-6, 'X') }
  end
  
  it "converts index to an integer using to_int" do
    other = Object.new
    def other.to_int() -3 end
    "abcd".insert(other, "XYZ").should == "abXYZcd"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => -3)
    "abcd".insert(obj, "XYZ").should == "abXYZcd"
  end
  
  it "converts other to a string using to_str" do
    other = Object.new
    def other.to_str() "XYZ" end
    "abcd".insert(-3, other).should == "abXYZcd"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :count => :any, :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "X")
    "abcd".insert(-3, obj).should == "abXcd"
  end

  it "taints self if string to insert is tainted" do
    str = "abcd"
    str.insert(0, "T".taint).tainted?.should == true

    str = "abcd"
    other = Object.new
    def other.to_str() "T".taint end
    str.insert(0, other).tainted?.should == true
  end
  
  it "raises a TypeError if other can't be converted to string" do
    should_raise(TypeError) { "abcd".insert(-6, ?e) }
    should_raise(TypeError) { "abcd".insert(-6, :sym) }
    should_raise(TypeError) { "abcd".insert(-6, Object.new) }
  end
  
  it "raises a TypeError if self is frozen" do
    str = "abcd".freeze
    should_raise(TypeError) { str.insert(4, '') }
    should_raise(TypeError) { str.insert(4, 'X') }
  end
end

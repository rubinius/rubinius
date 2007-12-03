require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#*" do
  it "is equivalent to self.join(str) when passed a string" do
    ([ 1, 2, 3 ] * ",").should == [1, 2, 3].join(",")
  end

  it "handles recursive arrays like #join" do
    x = []
    x << x
    (x * ":").should == x.join(":")

    x = []
    y = []
    y << 9 << x << 8 << y << 7
    x << 1 << x << 2 << y << 3
    (x * ":").should == x.join(":")
  end

  it "calls to_str on its argument" do
    obj = Object.new
    def obj.to_str() "x" end
    ([1, 2, 3] * obj).should == [1, 2, 3].join("x")
    
    obj = Object.new
    obj.should_receive(:respond_to?, :count => :any, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "x")
    ([1, 2, 3] * obj).should == [1, 2, 3].join("x")
  end
  
  it "concatenates n copies of the array when passed an integer" do
    ([ 1, 2, 3 ] * 0).should == []
    ([ 1, 2, 3 ] * 1).should == [1, 2, 3]
    ([ 1, 2, 3 ] * 3).should == [1, 2, 3, 1, 2, 3, 1, 2, 3]
    ([] * 10).should == []
  end
  
  it "raises ArgumentError when passed a negative integer" do
    lambda { [ 1, 2, 3 ] * -1 }.should raise_error(ArgumentError)
    lambda { [] * -1 }.should raise_error(ArgumentError)
  end
  
  it "calls to_int on its argument" do
    obj = Object.new
    def obj.to_int() 2 end

    ([1, 2, 3] * obj).should == [1, 2, 3] * obj.to_int
    
    obj = Object.new
    obj.should_receive(:respond_to?, :count => :any, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
    ([1, 2, 3] * obj).should == [1, 2, 3] * 2
  end

  it "calls to_str on its argument before to_int" do
    obj = Object.new
    def obj.to_int() 2 end
    def obj.to_str() "x" end
    ([1, 2, 3] * obj).should == [1, 2, 3] * obj.to_str
  end

  it "returns subclass instance with Array subclasses" do
    (MyArray[1, 2, 3] * 0).class.should == MyArray
    (MyArray[1, 2, 3] * 1).class.should == MyArray
    (MyArray[1, 2, 3] * 2).class.should == MyArray
  end

  it "raises TypeError if the argument can neither be converted to a string nor an integer" do
    lambda { [1, 2] * Object.new }.should raise_error(TypeError)
  end  
end

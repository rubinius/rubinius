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
    obj = mock('x')
    def obj.to_str() "x" end
    ([1, 2, 3] * obj).should == [1, 2, 3].join("x")
  end

  it "checks whether the passed argument responds to #to_str" do
    obj = mock('x')
    obj.should_receive(:respond_to?).with(:to_str).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_str).and_return("x")
    ([1, 2, 3] * obj).should == [1, 2, 3].join("x")
  end

  it "concatenates n copies of the array when passed an integer" do
    ([ 1, 2, 3 ] * 0).should == []
    ([ 1, 2, 3 ] * 1).should == [1, 2, 3]
    ([ 1, 2, 3 ] * 3).should == [1, 2, 3, 1, 2, 3, 1, 2, 3]
    ([] * 10).should == []
  end
  
  it "raises an ArgumentError when passed a negative integer" do
    lambda { [ 1, 2, 3 ] * -1 }.should raise_error(ArgumentError)
    lambda { [] * -1 }.should raise_error(ArgumentError)
  end
  
  it "calls to_int on its argument" do
    obj = mock('2')
    def obj.to_int() 2 end
    ([1, 2, 3] * obj).should == [1, 2, 3] * obj.to_int
  end

  it "checks whether the passed argument responds to #to_int" do
    obj = mock('2')
    obj.should_receive(:respond_to?).with(:to_str).any_number_of_times.and_return(false)
    obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_int).and_return(2)
    ([1, 2, 3] * obj).should == [1, 2, 3] * 2
  end

  it "calls to_str on its argument before to_int" do
    obj = mock('2')
    def obj.to_int() 2 end
    def obj.to_str() "x" end
    ([1, 2, 3] * obj).should == [1, 2, 3] * obj.to_str
  end

  it "returns subclass instance with Array subclasses" do
    (ArraySpecs::MyArray[1, 2, 3] * 0).class.should == ArraySpecs::MyArray
    (ArraySpecs::MyArray[1, 2, 3] * 1).class.should == ArraySpecs::MyArray
    (ArraySpecs::MyArray[1, 2, 3] * 2).class.should == ArraySpecs::MyArray
  end

  it "raises a TypeError if the argument can neither be converted to a string nor an integer" do
    lambda { [1, 2] * mock('str') }.should raise_error(TypeError)
  end  
end

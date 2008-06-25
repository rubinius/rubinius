require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#first" do
  it "returns the first element" do
    %w{a b c}.first.should == 'a'
    [nil].first.should == nil
  end
  
  it "returns nil if self is empty" do
    [].first.should == nil
  end
  
  it "returns the first count elements" do
    [true, false, true, nil, false].first(2).should == [true, false]
  end
  
  it "returns an empty array when passed count on an empty array" do
    [].first(0).should == []
    [].first(1).should == []
  end
  
  it "returns an empty array when passed count == 0" do
    [1, 2, 3, 4, 5].first(0).should == []
  end
  
  it "returns an array containing the first element when passed count == 1" do
    [1, 2, 3, 4, 5].first(1).should == [1]
  end
  
  it "raises an ArgumentError when count is negative" do
    lambda { [1, 2].first(-1) }.should raise_error(ArgumentError)
  end
  
  it "returns the entire array when count > length" do
    [1, 2, 3, 4, 5, 9].first(10).should == [1, 2, 3, 4, 5, 9]
  end

  it "tries to convert the passed argument to an Integer using #to_int" do
    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(2)
    [1, 2, 3, 4, 5].first(obj).should == [1, 2]
  end
  
  it "checks whether the passed argument responds to #to_int" do
    obj = mock('method_missing to_int')
    obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_int).and_return(2)
    [1, 2, 3, 4, 5].first(obj).should == [1, 2]
  end

  it "does not return subclass instance when passed count on Array subclasses" do
    ArraySpecs::MyArray[].first(0).class.should == Array
    ArraySpecs::MyArray[].first(2).class.should == Array
    ArraySpecs::MyArray[1, 2, 3].first(0).class.should == Array
    ArraySpecs::MyArray[1, 2, 3].first(1).class.should == Array
    ArraySpecs::MyArray[1, 2, 3].first(2).class.should == Array
  end
end

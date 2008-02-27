require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#last" do
  it "returns the last element" do
    [1, 1, 1, 1, 2].last.should == 2
  end
  
  it "returns nil if self is empty" do
    [].last.should == nil
  end
  
  it "returns the last count elements" do
    [1, 2, 3, 4, 5, 9].last(3).should == [4, 5, 9]
  end

  it "returns an empty array when passed a count on an empty array" do
    [].last(0).should == []
    [].last(1).should == []
  end
  
  it "returns an empty array when count == 0" do
    [1, 2, 3, 4, 5].last(0).should == []
  end
  
  it "raises an ArgumentError when count is negative" do
    lambda { [1, 2].last(-1) }.should raise_error(ArgumentError)
  end
  
  it "returns the entire array when count > length" do
    [1, 2, 3, 4, 5, 9].last(10).should == [1, 2, 3, 4, 5, 9]
  end

  it "uses to_int to convert its argument" do
    o = mock('2')
    lambda { [1, 2, 3].last o }.should raise_error(TypeError)

    def o.to_int(); 2; end

    [1, 2, 3].last(o).should == [2, 3]
  end

  it "does not return subclass instance on Array subclasses" do
    ArraySpecs::MyArray[].last(0).class.should == Array
    ArraySpecs::MyArray[].last(2).class.should == Array
    ArraySpecs::MyArray[1, 2, 3].last(0).class.should == Array
    ArraySpecs::MyArray[1, 2, 3].last(1).class.should == Array
    ArraySpecs::MyArray[1, 2, 3].last(2).class.should == Array
  end
end

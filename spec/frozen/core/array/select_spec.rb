require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#select" do
  it "returns a new array of elements for which block is true" do
    [1, 3, 4, 5, 6, 9].select { |i| i % ((i + 1) / 2) == 0}.should == [1, 4, 6]
  end

  it "does not return subclass instance on Array subclasses" do
    ArraySpecs::MyArray[1, 2, 3].select { true }.class.should == Array
  end

  it "properly handles recursive arrays" do
    empty = ArraySpecs.empty_recursive_array
    empty.select { true }.should == empty
    empty.select { false }.should == []

    array = ArraySpecs.recursive_array
    array.select { true }.should == [1, 'two', 3.0, array, array, array, array, array]
    array.select { false }.should == []
  end

end

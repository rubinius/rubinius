require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#partition" do
  it "returns two arrays" do
    [].partition {}.should == [[], []]
  end
  
  it "returns in the left array values for which the block evaluates to true" do
    ary = [0, 1, 2, 3, 4, 5]

    ary.partition { |i| true }.should == [ary, []]
    ary.partition { |i| 5 }.should == [ary, []]
    ary.partition { |i| false }.should == [[], ary]
    ary.partition { |i| nil }.should == [[], ary]
    ary.partition { |i| i % 2 == 0 }.should == [[0, 2, 4], [1, 3, 5]]
    ary.partition { |i| i / 3 == 0 }.should == [[0, 1, 2], [3, 4, 5]]
  end
  
  it "does not return subclass instances on Array subclasses" do
    result = ArraySpecs::MyArray[1, 2, 3].partition { |x| x % 2 == 0 }
    result.class.should == Array
    result[0].class.should == Array
    result[1].class.should == Array
  end
end

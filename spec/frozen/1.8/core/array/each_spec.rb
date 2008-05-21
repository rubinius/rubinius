require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#each" do
  it "yields each element to the block" do
    a = []
    x = [1, 2, 3]
    x.each { |item| a << item }.should equal(x)
    a.should == [1, 2, 3]
  end
  
  it "yields each element to the block even if the array is changed during iteration" do
    a = [1, 2, 3, 4, 5]
    b = []
    a.each {|x| b << x; a << x+5 if (x%2).zero? }
    b.should == [1, 2, 3, 4, 5, 7, 9]
  end
  
  it "yields only elements that are still in the array" do
    a = [0, 1, 2, 3, 4]
    b = []
    a.each {|x| b << x; a.pop if (x%2).zero? }
    b.should == [0, 1, 2]
  end    

  it "yields elements based on an internal index" do
    a = [0, 1, 2, 3, 4]
    b = []
    a.each {|x| b << x; a.shift if (x%2).zero? }
    b.should == [0, 2, 4]
  end    

  it "yields each element to a block that takes multiple arguments" do
    a = [[1, 2], :a, [3, 4]]
    b = []
    
    a.each { |x, y| b << x }
    b.should == [1, :a, 3]

    b = []
    a.each { |x, y| b << y }
    b.should == [2, nil, 4]
  end
end

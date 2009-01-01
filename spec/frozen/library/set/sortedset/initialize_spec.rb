require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'

describe "SortedSet#initialize" do
  it "is private" do
    SortedSet[].private_methods.should include("initialize")
  end
  
  it "adds all elements of the passed Enumerable to self" do
    s = SortedSet.new([1, 2, 3])
    s.size.should eql(3)
    s.should include(1)
    s.should include(2)
    s.should include(3)
  end
  
  it "preprocesses all elements by a passed block before adding to self" do
    s = SortedSet.new([1, 2, 3]) { |x| x * x }
    s.size.should eql(3)
    s.should include(1)
    s.should include(4)
    s.should include(9)
  end
end

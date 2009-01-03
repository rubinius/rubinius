require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'

describe "SortedSet#initialize_copy" do
  before(:each) do
    @set = SortedSet[1, 2, 3]
  end
  
  it "is private" do
    @set.private_methods.should include("initialize_copy")
  end
  
  it "replaces all elements of self with the elements of the passed SortedSet" do
    other = SortedSet["1", "2", "3"]
    @set.send(:initialize_copy, other)
    @set.should == SortedSet["1", "2", "3"]
  end
end

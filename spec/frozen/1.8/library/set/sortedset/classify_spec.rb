require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'

describe "SortedSet#classify" do
  before(:each) do
    @set = SortedSet["one", "two", "three", "four"]
  end

  it "yields each Object in self in sorted order" do
    res = []
    @set.classify { |x| res << x }
    res.should == ["one", "two", "three", "four"].sort
  end

  it "raises a LocalJumpError when passed no block" do
    lambda { @set.classify }.should raise_error(LocalJumpError)
  end

  it "classifies the Objects in self based on the block's return value" do
    classified = @set.classify { |x| x.length }
    classified.should == { 3 => SortedSet["one", "two"], 4 => SortedSet["four"], 5 => SortedSet["three"] }
  end
end

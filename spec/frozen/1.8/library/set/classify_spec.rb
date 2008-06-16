require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#classify" do
  before(:each) do
    @set = Set["one", "two", "three", "four"]
  end

  it "yields each Object in self" do
    res = []
    @set.classify { |x| res << x }
    res.sort.should == ["one", "two", "three", "four"].sort
  end

  it "raises a LocalJumpError when passed no block" do
    lambda { @set.classify }.should raise_error(LocalJumpError)
  end

  it "classifies the Objects in self based on the block's return value" do
    classified = @set.classify { |x| x.length }
    classified.should == { 3 => Set["one", "two"], 4 => Set["four"], 5 => Set["three"] }
  end
end

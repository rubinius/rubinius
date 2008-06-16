require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#delete_if" do
  before(:each) do
    @set = Set["one", "two", "three"]
  end

  it "yields every element of self" do
    ret = []
    @set.delete_if { |x| ret << x }
    ret.sort.should == ["one", "two", "three"].sort
  end

  it "deletes every element from self for which the passed block returns true" do
    @set.delete_if { |x| x.size == 3 }
    @set.size.should eql(1)

    @set.should_not include("one")
    @set.should_not include("two")
    @set.should include("three")
  end

  it "returns self" do
    @set.delete_if { |x| x }.should equal(@set)
  end

  it "raises a LocalJumpError when not passed a block" do
    lambda { @set.delete_if }.should raise_error(LocalJumpError)
  end
end

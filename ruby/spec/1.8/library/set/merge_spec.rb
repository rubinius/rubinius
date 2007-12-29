require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#merge" do
  before :each do
    @set = Set[:a, :b]
  end
  
  it "adds the elements of other to the set and returns self" do
    @set.merge(Set[:b, :c, :d]).should == @set
    @set.should == Set[:a, :b, :c, :d]
  end
  
  it "accepts any enumerable as other" do
    @set.merge([:a, :b, :c]).should == Set[:a, :b, :c]
  end
end

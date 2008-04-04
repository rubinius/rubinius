require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#proper_subset?" do

  before :each do
    @set = Set[1, 2, 3, 4]
    @subset = Set[1, 2, 3]
    @empty_set = Set[]
  end

  it "returns true if a subset" do
    @subset.proper_subset?(@set).should == true
  end
  
  it "returns false when compared to itself" do
    @set.proper_subset?(@set).should == false
  end
  
  it "returns true when comparing set to empty set" do
    @empty_set.proper_subset?(@set).should == true
  end
  
  it "returns false when comparing empty set to itself" do
    @empty_set.proper_subset?(@empty_set).should == false
  end
  
end

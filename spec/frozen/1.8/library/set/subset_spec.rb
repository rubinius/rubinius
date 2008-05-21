require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#subset?" do

  before :each do
    @set = Set[1, 2, 3, 4]
    @subset = Set[1, 2, 3]
    @empty_set = Set[]
  end

  it "returns true if a subset of a set" do
    @subset.subset?(@set).should == true
  end
  
  it "returns true when compared to itself" do
    @set.subset?(@set).should == true
  end
  
  it "returns true when comparing empty set to a set" do
    @empty_set.subset?(@set).should == true
  end
  
  it "returns true when comparing empty set to itself" do
    @empty_set.subset?(@empty_set).should == true
  end
  
end

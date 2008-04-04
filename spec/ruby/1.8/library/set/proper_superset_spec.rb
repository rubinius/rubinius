require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#superset?" do

  before :each do
    @set = Set[1, 2, 3, 4]
    @superset = Set[1, 2, 3, 4, 5]
    @empty_set = Set[]
  end

  it "returns true if a proper superset" do
    @superset.proper_superset?(@set).should == true
  end
  
  it "returns false when compared to itself" do
    @set.proper_superset?(@set).should == false
  end
  
  it "returns false when non-empty set compared to an empty set" do
    @empty_set.proper_superset?(@set).should == false
  end
  
  it "returns false then empty set compared to itself" do
    @empty_set.proper_superset?(@empty_set).should == false
  end
  
end

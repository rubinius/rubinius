require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#superset" do

  before :each do
    @set = Set[1, 2, 3, 4]
    @superset = Set[1, 2, 3, 4, 5]
  end

  it "returns true if a superset" do
    @superset.superset?(@set).should == true
  end
  
  it "returns true when compared to itself" do
    @set.superset?(@set).should == true
  end
  
end

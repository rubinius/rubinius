require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Comparable#<=>" do
  it "is provided" do
    ComparableSpecs::Weird.new(0).respond_to?(:<=>).should == true
  end
  
  it "returns 0 if other is equal" do
    (ComparableSpecs::Weird.new(-1) <=> ComparableSpecs::Weird.new(-2)).should == 0
  end
  
  it "returns 1 if other is greater" do
    (ComparableSpecs::Weird.new(-1) <=> ComparableSpecs::Weird.new(0)).should == 1
  end
  
  it "returns -1 if other is lesser" do
    (ComparableSpecs::Weird.new(1) <=> ComparableSpecs::Weird.new(-1)).should == -1
  end
end

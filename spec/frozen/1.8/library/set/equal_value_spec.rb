require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#==" do
  it "returns true when the passed Object is a Set and self and the Object contain the same elements" do
    Set[] == Set[]
    Set[1, 2, 3].should == Set[1, 2, 3]
    Set["1", "2", "3"].should == Set["1", "2", "3"]
    
    Set[1, 2, 3].should_not == Set[1.0, 2, 3]
    Set[1, 2, 3].should_not == [1, 2, 3]
  end
end

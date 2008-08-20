require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#initialize_copy" do
  before(:each) do
    @set = Set[1, 2, 3]
  end
  
  it "is private" do
    @set.private_methods.should include("initialize_copy")
  end
  
  it "replaces all elements of self with the elements of the passed Set" do
    other = Set["1", "2", "3"]
    @set.send(:initialize_copy, other)
    @set.should == Set["1", "2", "3"]
  end
end

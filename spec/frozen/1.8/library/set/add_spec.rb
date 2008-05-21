require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/add'
require 'set'

describe "Set#add" do
  it_behaves_like :set_add, :add
end

describe "Set#add?" do
  before :each do
    @set = Set.new
  end
  
  it "adds the object to the set and returns self" do
    @set.add?("cat").should == Set["cat"]
  end
  
  it "returns nil if the object is already in the set" do
    @set.add?("cat")
    @set.add?("cat").should == nil
  end
end

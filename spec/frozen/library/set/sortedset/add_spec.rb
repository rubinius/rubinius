require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/add'

describe "SortedSet#add" do
  it_behaves_like :sorted_set_add, :add
end

describe "SortedSet#add?" do
  before :each do
    @set = SortedSet.new
  end

  it "adds the passed Object to self" do
    @set.add?("cat")
    @set.should include("cat")
  end

  it "returns self when the Object has not yet been added to self" do
    @set.add?("cat").should equal(@set)
  end

  it "returns nil when the Object has already been added to self" do
    @set.add?("cat")
    @set.add?("cat").should be_nil
  end
end

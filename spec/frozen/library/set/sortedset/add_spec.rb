require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/shared/add'

describe "SortedSet#add" do
  it_behaves_like :sorted_set_add, :add

  ruby_bug "redmine #118", "1.9.1" do
    it "takes only values which responds <=>" do
      lambda { SortedSet[Object.new].add(Object.new) }.should raise_error(ArgumentError)

      obj = mock('x')
      lambda { SortedSet[obj].add(:aaa) }.should raise_error(ArgumentError)
      def obj.<=>(o) 1 end
      lambda { SortedSet[obj].add(:aaa) }.should_not raise_error(ArgumentError)
    end
  end
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

require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'

describe "SortedSet#flatten" do
  ruby_bug "http://redmine.ruby-lang.org/projects/ruby-18/issues/show?id=117", "1.8.7" do
    it "returns a copy of self with each included SortedSet flattened" do
      set = SortedSet[1, 2, SortedSet[3, 4, SortedSet[5, 6, SortedSet[7, 8]]], 9, 10]
      flattened_set = set.flatten
    
      flattened_set.should_not equal(set)
      flattened_set.should == SortedSet[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    end
  end
end

describe "SortedSet#flatten!" do
  ruby_bug "http://redmine.ruby-lang.org/projects/ruby-18/issues/show?id=117", "1.8.7" do
    it "flattens self" do
      set = SortedSet[1, 2, SortedSet[3, 4, SortedSet[5, 6, SortedSet[7, 8]]], 9, 10]
      set.flatten!
      set.should == SortedSet[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    end
  
    it "returns self when self was modified" do
      set = SortedSet[1, 2, SortedSet[3, 4]]
      set.flatten!.should equal(set)
    end
  end
  
  it "returns nil when self was not modified" do
    set = SortedSet[1, 2, 3, 4]
    set.flatten!.should be_nil
  end
end
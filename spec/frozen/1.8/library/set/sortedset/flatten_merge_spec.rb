require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'

describe "SortedSet#flatten_merge" do
  it "is protected" do
    SortedSet[].protected_methods.should include("flatten_merge")
  end
  
  ruby_bug "http://redmine.ruby-lang.org/projects/ruby-18/issues/show?id=117", "1.8.7" do
    it "flattens the passed SortedSet and merges it into self" do
      set1 = SortedSet[1, 2]
      set2 = SortedSet[3, 4, SortedSet[5, 6]]
    
      set1.send(:flatten_merge, set2).should == SortedSet[1, 2, 3, 4, 5, 6]
    end
  
    it "raises an ArgumentError when trying to flatten a recursive SortedSet" do
      set1 = SortedSet[1, 2, 3]
      set2 = SortedSet[5, 6, 7]
      set2 << set2
    
      lambda { set1.send(:flatten_merge, set2) }.should raise_error(ArgumentError)
    end
  end
end
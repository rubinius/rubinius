require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/../shared/collect'

describe "SortedSet#map!" do
  it_behaves_like :set_collect_bang, SortedSet, :map!

  it "yields each Object in sorted order" do
    res = []
    set = SortedSet["one", "two", "three", "four"]
    set.map! { |x| res << x }
    res.should == ["four", "one", "three", "two"]

    res = []
    set = SortedSet[4, 5, 1, 2]
    set.map! { |x| res << x }
    res.should == [1, 2, 4, 5]
  end
end

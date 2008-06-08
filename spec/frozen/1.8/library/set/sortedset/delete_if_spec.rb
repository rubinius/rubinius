require File.dirname(__FILE__) + '/../../../spec_helper'
require 'set'
require File.dirname(__FILE__) + '/../shared/delete_if'

describe "SortedSet#delete_if" do
  it_behaves_like :set_delete_if, SortedSet

  # BUG: SortedSet#delete_if does not yield in sorted order.
  #
  # it "yields each Object in sorted order" do
  #   res = []
  #   set = SortedSet["one", "two", "three", "four"]
  #   set.delete_if { |x| res << x }
  #   res.should == ["four", "one", "three", "two"]
  # 
  #   res = []
  #   set = SortedSet[4, 5, 1, 2]
  #   set.delete_if { |x| res << x }
  #   res.should == [1, 2, 4, 5]
  # end
end

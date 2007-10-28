require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Enumerable#sort_by" do
  it "should sort the object by the given attribute" do
    a = SortByDummy.new("fooo")
    b = SortByDummy.new("bar")

    ar = [a, b].sort_by { |d| d.s }
    ar.should == [b, a]
  end
end

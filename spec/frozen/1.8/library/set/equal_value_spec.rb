require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#==" do
  it "returns true if the sets are equal" do
    s1 = Set[:a, :b, :c]
    s2 = Set[:a, :b, :c]
    s1.should == s2
    s1 << :d
    s1.should_not == s2
  end
end

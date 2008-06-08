require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#inspect" do
  it "returns a String representation of self" do
    Set[].inspect.should be_kind_of(String)
    Set[nil, false, true].inspect.should be_kind_of(String)
    Set[1, 2, 3].inspect.should be_kind_of(String)
    Set["1", "2", "3"].inspect.should be_kind_of(String)
    Set[:a, "b", Set[?c]].inspect.should be_kind_of(String)
  end
end

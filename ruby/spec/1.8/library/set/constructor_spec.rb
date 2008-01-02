require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set.[]" do
  it "creates a new Set" do
    set = Set[1, 2, 3]
    other = Set.new
    other << 1 << 2 << 3
    set.should == other
  end
end

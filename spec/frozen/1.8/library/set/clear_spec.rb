require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#clear" do
  it "removes all the elements and returns self" do
    saved = set = Set[:a, :b, :c]
    set.clear.should == saved
    set.empty?.should == true
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#delete" do
  it "deletes the object and returns self" do
    saved = set = Set[:a, :b, :c]
    set.delete(:a).should == saved
    set.should == Set[:b, :c]
  end
end

describe "Set#delete?" do
  it "deletes the object and returns self" do
    saved = set = Set[:a, :b, :c]
    set.delete?(:b).should == saved
    set.should == Set[:a, :c]
  end
  
  it "returns nil if the element is not in the set" do
    Set[:a, :b, :c].delete?(:d).should == nil
  end
end

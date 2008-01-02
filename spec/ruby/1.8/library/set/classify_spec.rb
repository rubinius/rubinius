require File.dirname(__FILE__) + '/../../spec_helper'
require 'set'

describe "Set#classify" do
  it "returns a Hash" do
    Set["a", "bb"].classify { |x| x.length }.should be_kind_of(Hash)
  end
  
  it "classifies the set by the return value of the block" do
    set = Set["one", "two", "three", "four"]
    c = set.classify { |x| x.length }
    c.keys.should include(3, 4, 5)
    c[3].should == Set["one", "two"]
    c[4].should == Set["four"]
    c[5].should == Set["three"]
  end
end

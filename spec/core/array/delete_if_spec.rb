require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#delete_if" do
  it "removes each element for which block returns true" do
    a = [ "a", "b", "c" ] 
    a.delete_if { |x| x >= "b" }.equal?(a).should == true
    a.should == ["a"]
  end
  
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { @frozen_array.delete_if {} }
  end
end

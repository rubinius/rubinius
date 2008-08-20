require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#delete_if" do
  it "removes each element for which block returns true" do
    a = [ "a", "b", "c" ] 
    a.delete_if { |x| x >= "b" }.should equal(a)
    a.should == ["a"]
  end

  compliant_on :ruby, :jruby do
    it "raises a TypeError on a frozen array" do
      lambda { ArraySpecs.frozen_array.delete_if {} }.should raise_error(TypeError)
    end
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'

describe "FalseClass#^" do
  def false_xor(other)
    false ^ other
  end
  
  it "returns false if other is nil or false, otherwise true" do
    false_xor(false).should == false
    false_xor(true).should == true
    false_xor(nil).should == false
    false_xor("").should == true
    false_xor(Object.new).should == true
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'

describe "TrueClass#^" do
  def true_xor(other)
    true ^ other
  end
  
  it "returns true if other is nil or false, otherwise false" do
    true_xor(true).should == false
    true_xor(false).should == true
    true_xor(nil).should == true
    true_xor("").should == false
    true_xor(Object.new).should == false
  end
end

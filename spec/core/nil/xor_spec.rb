require File.dirname(__FILE__) + '/../../spec_helper'

describe "NilClass#^" do
  def nil_xor(other)
    nil ^ other
  end
  
  it "returns false if other is nil or false, otherwise true" do
    nil_xor(nil).should == false
    nil_xor(true).should == true
    nil_xor(false).should == false
    nil_xor("").should == true
    nil_xor(Object.new).should == true
  end
end

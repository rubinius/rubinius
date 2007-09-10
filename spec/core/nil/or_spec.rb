require File.dirname(__FILE__) + '/../../spec_helper'

describe "NilClass#|" do
  def nil_or(other)
    nil | other
  end
  
  it "returns false if other is nil or false, otherwise true" do
    nil_or(nil).should == false
    nil_or(true).should == true
    nil_or(false).should == false
    nil_or("").should == true
    nil_or(Object.new).should == true
  end
end

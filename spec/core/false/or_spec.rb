require File.dirname(__FILE__) + '/../../spec_helper'

describe "FalseClass#|" do
  def false_or(other)
    false | other
  end
  
  it "return false if other is nil or false, otherwise true" do
    false_or(false).should == false
    false_or(true).should == true
    false_or(nil).should == false
    false_or("").should == true
    false_or(Object.new).should == true
  end
end

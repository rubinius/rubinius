require File.dirname(__FILE__) + '/../../spec_helper'

describe "TrueClass#&" do
  def true_and(other)
    true & other
  end
  
  it "returns false if other is nil or false, otherwise true" do
    true_and(true).should == true
    true_and(false).should == false
    true_and(nil).should == false
    true_and("").should == true
    true_and(Object.new).should == true
  end
end

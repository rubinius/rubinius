require File.dirname(__FILE__) + '/../../spec_helper'

describe "FalseClass#&" do
  def false_and(other)
    false & other
  end
  
  it "returns false" do
    false_and(false).should == false
    false_and(true).should == false
    false_and(nil).should == false
    false_and("").should == false
    false_and(Object.new).should == false
  end
end

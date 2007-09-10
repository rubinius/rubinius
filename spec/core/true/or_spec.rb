require File.dirname(__FILE__) + '/../../spec_helper'

describe "TrueClass#|" do
  def true_or(other)
    true | other
  end
  
  it "returns true" do
    true_or(true).should == true
    true_or(false).should == true
    true_or(nil).should == true
    true_or("").should == true
    true_or(Object.new).should == true
  end
end

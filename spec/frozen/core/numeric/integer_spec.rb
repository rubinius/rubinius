require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#integer?" do
  it "returns false" do
    NumericSub.new.integer?.should == false
  end   
end 

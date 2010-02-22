require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Numeric#integer?" do
  it "returns false" do
    NumericSub.new.integer?.should == false
  end   
end 

require File.dirname(__FILE__) + '/../../spec_helper'

# Returns num if num is not zero, nil otherwise. 
# This behavior is useful when chaining comparisons:
describe "Numeric#nonzero?" do 
  it "return the vaule if number is different to 0" do 
    0.nonzero?.should == nil
    1.nonzero?.should == 1
  end  
end

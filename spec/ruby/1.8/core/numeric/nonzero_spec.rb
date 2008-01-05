require File.dirname(__FILE__) + '/../../spec_helper'

# Returns num if num is not zero, nil otherwise. 
# This behavior is useful when chaining comparisons:
describe "Numeric#nonzero?" do 
  it "returns nil if the number is zero" do
    0.nonzero?.should == nil
  end

  it "returns the value if number is different to 0" do
    1.nonzero?.should == 1
    2.nonzero?.should == 2
  end  
end

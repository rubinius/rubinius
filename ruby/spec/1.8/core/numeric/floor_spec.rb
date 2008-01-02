require File.dirname(__FILE__) + '/../../spec_helper'

# Returns the largest integer less than or equal to num. Numeric implements 
# this by converting anInteger to a Float and invoking Float#floor.
describe "Numeric#floor" do 
  it "return the largest integer less than or equal to num (integer)" do 
    0.floor.should == 0
    100.floor.should == 100
    -100.floor.should == -100 
  end   
  
  it "return the largest integer less than or equal to num (two complement)" do 
    2147483648.floor.should == 2147483648
    -2147483648.floor.should == -2147483648
    9223372036854775808.floor.should == 9223372036854775808
    -9223372036854775808.floor.should == -9223372036854775808
  end   
end

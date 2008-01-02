require File.dirname(__FILE__) + '/../../spec_helper'

# Returns the smallest Integer greater than or equal to num. 
# Class Numeric achieves this by converting itself to a Float then invoking Float#ceil.
describe "Numeric#ceil" do
  it "ceil to integer" do
    0.ceil.should == 0
    100.ceil.should == 100
    -100.ceil.should == -100
  end
  
  it "should ceil to float" do 
    0.ceil.should == 0.0
    34.56.ceil.should == 35
    -34.56.ceil.should == -34
  end
  
  it "ceil twos complement" do 
    2147483648.ceil.should == 2147483648
    -2147483648.ceil.should == -2147483648
    9223372036854775808.ceil.should == 9223372036854775808
    -9223372036854775808.ceil.should == -9223372036854775808  
  end
end

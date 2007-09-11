require File.dirname(__FILE__) + '/../../spec_helper'

# Returns the absolute value of num.
describe "Numeric#abs" do  
  it "return the abs (integers) " do      
    0.abs.should == 0 
    100.abs.should  == 100
    -100.abs.should   == 100
  end
  
  it "return the abs (floats) " do 
    0.0.abs.should == 0.0
    34.56.abs.should == 34.56
    -34.56.abs.should == 34.56 
  end
  
  it "return the abs (two complement)" do
    2147483648.abs.should == 2147483648
    -2147483648.abs.should == 2147483648
    9223372036854775808.abs.should == 9223372036854775808
    -9223372036854775808.abs.should == 9223372036854775808    
  end 
end

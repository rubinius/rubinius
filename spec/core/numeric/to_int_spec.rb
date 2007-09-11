require File.dirname(__FILE__) + '/../../spec_helper'

describe "Numeric#to_int" do  
  it "return the integer (integers)" do 
    0.to_int.should == 0
    100.to_int.should  == 100
    -100.to_int.should == -100
  end  
  
  it "return the integer part (float)" do 
    34.56.to_int.should == 34 
    -34.56.to_int.should == -34
  end  
  
  it "return the integer part (two complement)" do    
    2147483648.to_int.should == 2147483648
    -2147483648.to_int.should == -2147483648
    9223372036854775808.to_int.should == 9223372036854775808
    -9223372036854775808.to_int.should == -9223372036854775808
  end   
end

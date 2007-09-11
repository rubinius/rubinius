require File.dirname(__FILE__) + '/../../spec_helper'

# Returns true if num is an Integer (including Fixnum and Bignum).
describe "Numeric#integer?" do  
  it "retrun true if the num is an integer?" do 
    0.integer?.should == true
    100.integer?.should  == true
    -100.integer?.should  == true
    34.56.integer?.should == false
    -34.56.integer?.should == false
    2147483648.integer?.should == true
    -2147483648.integer?.should == true
    9223372036854775808.integer?.should == true
    -9223372036854775808.integer?.should == true
  end   
end 

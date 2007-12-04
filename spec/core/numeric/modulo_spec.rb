require File.dirname(__FILE__) + '/../../spec_helper'

describe "Numeric#modulo" do 
  it " zero modulo x should be 0 (integer) " do 
    0.modulo(100).should == 0
    0.modulo(-100).should == 0
  end
  
  it " zero modulo x should be 0 (float) " do 
    0.modulo(34.56).should == 0
    0.modulo(-34.56).should == 0
  end
  
  it " zero modulo x should be 0 (bignum) " do 
    0.modulo(2147483648).should == 0
    0.modulo(-2147483648).should == 0 
  end
  
  it "y modulo x should be z (integer - integer) " do 
    100.modulo(100).should == 0
    100.modulo(-100).should == 0
    -100.modulo(100).should == 0
    -100.modulo(-100).should == 0
  end
  
  it "y modulo x should be z (integer - float) " do 
    100.modulo(34.56).should be_close(30.88,TOLERANCE) 
    100.modulo(-34.56).should be_close(-3.68000000000001,TOLERANCE)
    -100.modulo(34.56).should be_close(3.68000000000001 ,TOLERANCE)
    -100.modulo(-34.56).should be_close(-30.88,TOLERANCE)
  end
  
  it "y modulo x should be z (integer - bignum) " do 
    100.modulo(2147483648).should == 100 
    100.modulo(-2147483648).should == -2147483548
    -100.modulo(-2147483648).should == -100 
    -100.modulo(2147483648).should == 2147483548
  end  

  it "modulo x should be z  (floats and integers)"do 
    34.56.modulo(100).should be_close(34.56,TOLERANCE)
    34.56.modulo(-100).should be_close(-65.44,TOLERANCE)
    -34.56.modulo(-100).should be_close(-34.56,TOLERANCE)    
    -34.56.modulo(100).should be_close(65.44,TOLERANCE)    
  end
  
  it "modulo x should be z  (float - float)"do     
    34.56.modulo(34.56).should be_close(0.0,TOLERANCE)
    34.56.modulo(-34.56).should be_close(0.0,TOLERANCE)
    -34.56.modulo(-34.56).should be_close(0.0,TOLERANCE)
    -34.56.modulo(34.56).should be_close(0.0,TOLERANCE)
  end
  
  it "modulo x should be z  (float - bignum)"do     
    34.56.modulo(2147483648).should be_close(34.56,TOLERANCE)
    34.56.modulo(-2147483648).should be_close(-2147483613.44 ,TOLERANCE)
    -34.56.modulo(2147483648).should be_close(2147483613.44 ,TOLERANCE) 
  end

  it "y modulo x should be z (bignum -integer) "do 
    2147483648.modulo(100).should == 48
    2147483648.modulo(-100).should == -52    
    -2147483648.modulo(100).should == 52
    -2147483648.modulo(-100).should == -48
  end
  
  it "y modulo x should be z (bignum - float) "do 
    2147483648.modulo(34.56).should be_close(1.27999985871492, TOLERANCE)
    -2147483648.modulo(34.56).should be_close(33.2800001412851 , TOLERANCE)
    2147483648.modulo(-34.56).should be_close(-33.2800001412851 , TOLERANCE)
    -2147483648.modulo(-34.56).should be_close(-1.27999985871492, TOLERANCE)
  end
  
  it "y modulo x should be z (bignum - bignum) "do   
    2147483648.modulo(2147483648).should == 0
    -2147483648.modulo(2147483648).should == 0
    2147483648.modulo(-2147483648).should == 0
    -2147483648.modulo(-2147483648).should == 0 
  end 
  
  it " should NOT raise ZeroDivisionError if other is zero and is a Float" do
    1.modulo(0.0).to_s.should == 'NaN'
    1.modulo(0.0).to_s.should == 'NaN'
  end
  
  it " should raise an Exception when divide by 0 (non float)" do    
    lambda { 13.modulo(0) }.should raise_error(ZeroDivisionError)
  end
end

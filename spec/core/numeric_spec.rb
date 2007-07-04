require File.dirname(__FILE__) + '/../spec_helper'

TOLERANCE = 0.00003

# TODO <=> angle arg  conj conjugate   
# im imag image  polar real  singleton_method_added    

describe "Numeric#-@" do 
  before(:each) do 
    @num_zero  = 0
    @num_pos   = 100
    @num_neg   = -100
    @num_posf  = 34.56
    @num_negf  = -34.56
    @num_twop  = 2147483648
    @num_twon  = -2147483648
    @num_twopb = 9223372036854775808
    @num_twonb = -9223372036854775808    
    @den_bigint = 2**31
    @num_bigint_n = -2**31
  end  
  
  it " should return the same value with opposite sign (integers)" do 
    @num_zero.send(:-@).should == 0
    @num_pos.send(:-@).should == -100
    @num_neg.send(:-@).should == 100
    
  end  

  it " should return the same value with opposite sign (floats)" do 
    @num_posf.send(:-@).should == -34.56
    @num_negf.send(:-@).should == 34.56
  end   

  it " should return the same value with opposite sign (two complement)" do 
    @num_twop.send(:-@).should == -2147483648
    @num_twon.send(:-@).should == 2147483648
    @num_twopb.send(:-@).should == -9223372036854775808
    @num_twonb.send(:-@).should == 9223372036854775808
  end  
  
  after(:each) do 
    @num_zero  = nil
    @num_pos   = nil
    @num_neg   = nil
    @num_posf  = nil
    @num_negf  = nil
    @num_twop  = nil
    @num_twon  = nil
    @num_twopb = nil
    @num_twonb = nil 
    @den_bigint = nil
    @num_bigint_n = nil
  end
end

describe "Numeric#+@" do 
  before(:each) do 
    @num_zero  = 0
    @num_pos   = 100
    @num_neg   = -100
    @num_posf  = 34.56
    @num_negf  = -34.56
    @num_twop  = 2147483648
    @num_twon  = -2147483648
    @num_twopb = 9223372036854775808
    @num_twonb = -9223372036854775808    
    @den_bigint = 2**31
    @num_bigint_n = -2**31
  end  
  
  it " should return the same value with opposite sign (integers)" do 
    @num_zero.send(:+@).should == 0
    @num_pos.send(:+@).should == 100
    @num_neg.send(:+@).should == -100
    
  end  

  it " should return the same value with opposite sign (floats)" do 
    @num_posf.send(:+@).should == 34.56
    @num_negf.send(:+@).should == -34.56
  end  

  it " should return the same value with opposite sign (floats)" do 
    @den_bigint.send(:+@).should == 2**31
    @num_bigint_n.send(:+@).should == -2**31
  end  

  it " should return the same value with opposite sign (two complement)" do 
    @num_twop.send(:+@).should == 2147483648
    @num_twon.send(:+@).should == -2147483648
    @num_twopb.send(:+@).should == 9223372036854775808
    @num_twonb.send(:+@).should == -9223372036854775808
  end  
  
  after(:each) do 
    @num_zero  = nil
    @num_pos   = nil
    @num_neg   = nil
    @num_posf  = nil
    @num_negf  = nil
    @num_twop  = nil
    @num_twon  = nil
    @num_twopb = nil
    @num_twonb = nil 
    @den_bigint = nil
    @num_bigint_n = nil
  end
end

# Returns the smallest Integer greater than or equal to num. 
# Class Numeric achieves this by converting itself to a Float then invoking Float#ceil.
describe "Numeric#ceil " do 
  before(:each) do 
    @num_zero  = 0
    @num_pos   = 100
    @num_neg   = -100
    @num_posf  = 34.56
    @num_negf  = -34.56
    @num_twop  = 2147483648
    @num_twon  = -2147483648
    @num_twopb = 9223372036854775808
    @num_twonb = -9223372036854775808    
    @den_bigint = 2**31
    @num_bigint_n = -2**31
  end  
  
  it "ceil to integer " do
    @num_zero.ceil.should == 0
    @num_pos.ceil.should == 100
    @num_neg.ceil.should == -100
  end
  
  it " should ceil to float " do 
    @num_zero.ceil.should == 0.0
    @num_posf.ceil.should == 35
    @num_negf.ceil.should == -34
  end
  
  it "ceil twos complement" do 
    @num_twop.ceil.should == 2147483648
    @num_twon.ceil.should == -2147483648
    @num_twopb.ceil.should == 9223372036854775808
    @num_twonb.ceil.should == -9223372036854775808  
  end
  
  after(:each) do 
    @num_zero  = nil
    @num_pos   = nil
    @num_neg   = nil
    @num_posf  = nil
    @num_negf  = nil
    @num_twop  = nil
    @num_twon  = nil
    @num_twopb = nil
    @num_twonb = nil 
    @den_bigint = nil
    @num_bigint_n = nil  
  end
end

# Returns the absolute value of num.
describe "Numeric#abs" do 
  before(:each) do 
    @num_zero  = 0
    @zero_float = 0.0
    @num_pos   = 100
    @num_neg   = -100
    @num_posf  = 34.56
    @num_negf  = -34.56
    @num_twop  = 2147483648
    @num_twon  = -2147483648
    @num_twopb = 9223372036854775808
    @num_twonb = -9223372036854775808    
    @den_bigint = 2**31
    @num_bigint_n = -2**31
  end  
  
  it "return the abs (integers) " do      
    @num_zero.abs.should == 0 
    @num_pos.abs.should  == 100
    @num_neg.abs.should   == 100
  end
  
  it "return the abs (floats) " do 
    @zero_float.abs.should == 0.0
    @num_posf.abs.should == 34.56
    @num_negf.abs.should == 34.56 
  end
  
  it "return the abs (two complement)" do
    @num_twop.abs.should == 2147483648
    @num_twon.abs.should == 2147483648
    @num_twopb.abs.should == 9223372036854775808
    @num_twonb.abs.should == 9223372036854775808    
  end
  
  after(:each) do 
    @num_zero  = nil
    @num_pos   = nil
    @num_neg   = nil
    @num_posf  = nil
    @num_negf  = nil
    @num_twop  = nil
    @num_twon  = nil
    @num_twopb = nil
    @num_twonb = nil 
    @den_bigint = nil
    @num_bigint_n = nil  
  end
end

# Returns an array containing the quotient and modulus obtained
# by dividing num by aNumeric. If q, r = x.divmod(y), then
#    q = floor(float(x)/float(y))
#    x = q*y + r
describe "Numeric#divmod" do 
  before(:each) do 
    @num_int = 13
    @den_int = 4
    @num_flt = 13.0
    @den_flt = 4.0
    @num_bigint = 2**33
    @den_bigint = 2**32
  end
  
  it "divmod right  integers" do 
    @num_int.divmod(@den_int).should == [3,1]
    @den_int.divmod(@num_int).should == [0,4]
  end
  
  it "divmod right  integers and floats" do 
    @num_int.divmod(@den_flt).should == [3,1]
    @den_int.divmod(@num_int).should == [0,4]
  end
  
  it "divmod right the integers and floats" do 
    @num_int.divmod(@den_flt).should == [3,1]
    @den_int.divmod(@num_int).should == [0,4]
  end
  
  it "divmod right  floats" do 
    @num_flt.divmod(@den_flt).should == [3.0,1.0]
    @den_flt.divmod(@num_int).should == [0.0,4.0]
  end
  
  it " should divmod right with bignums and integers" do
    @num_bigint.divmod(@den_bigint).should == [2,0]
  end
  
  it "raise the expected exception" do
    should_raise(ArgumentError){ @num_int.divmod }
    should_raise(ZeroDivisionError){ @num_int.divmod(0) }
    should_raise(TypeError){ @num_int.divmod(nil) }
    should_raise(TypeError){ @num_int.divmod('test') }
    should_raise(TypeError){ @num_int.divmod(true) }   
  end
  
  after(:each) do 
    @num_int = nil
    @den_int = nil
    @num_flt = nil
    @den_flt = nil
    @num_bigint = nil
    @den_bigint = nil
  end
end

specify "quo should return the floating-point result of self divided by other" do
  # the to_f is required because RSpec (I'm assuming) requires 'rational'
  2.quo(2.5).to_s.should == '0.8'
  5.quo(2).to_f.to_s.should == '2.5'
  45.quo(0xffffffff).to_f.to_s.should == '1.04773789668636e-08'
end
  
specify "quo should NOT raise an exception when other is zero" do
  # 1.quo(0) should also not raise (i.e works in irb and from a file),
  # but fails here.
  1.quo(0.0).to_s.should == 'Infinity'
  1.quo(-0.0).to_s.should == '-Infinity'
end

# Equivalent to Numeric#/, but overridden in subclasses.
describe "Numeric#quo" do 
  before(:each) do 
    @num_int = 13
    @den_int = 4
    @num_flt = 13.0
    @den_flt = 4.0
    @num_bigint = 2**33
    @den_bigint = 2**32
  end
  
  it "quo right  integers" do 
    @num_int.quo(@den_int).should_be_close(3.25, TOLERANCE)
    @den_int.quo(@num_int).should_be_close( 0.307692307692308, TOLERANCE)
  end
     
  it "quo right  integers and floats" do 
    @num_int.quo(@den_flt).should_be_close(3.25, TOLERANCE)
    @den_int.quo(@num_int).should_be_close(0.307692307692308, TOLERANCE)
  end
  
  it "quo right the integers and floats" do 
    @num_int.quo(@den_flt).should_be_close(3.25, TOLERANCE)
    @den_int.quo(@num_int).should_be_close(0.307692307692308, TOLERANCE)
  end
  
  it "quo right  floats" do 
    @num_flt.quo(@den_flt).should_be_close(3.25, TOLERANCE)
    @den_flt.quo(@num_int).should_be_close(0.307692307692308, TOLERANCE)
  end
   
  it " should quo right with bignums and integers" do
    @num_bigint.quo(@den_bigint).should == 2
  end
  
  it "not raise a Exception when quo by 0" do
    @num_int.quo(0)
    @num_flt.quo(0)
    @num_bigint.quo(0)
  end
  
  it "raise the expected exception" do
    should_raise(ArgumentError){ @num_int.quo } 
    should_raise(TypeError){ @num_int.quo(nil) }
    should_raise(TypeError){ @num_int.quo('test') }
    should_raise(TypeError){ @num_int.quo(true) }   
  end
  
  after(:each) do 
    @num_int = nil
    @den_int = nil
    @num_flt = nil
    @den_flt = nil
    @num_bigint = nil
    @den_bigint = nil
  end
end
 
# Uses / to perform division, then converts the result to an integer
describe "Numeric#div" do 
  before(:each) do 
    @num_int = 13
    @den_int = 4
    @num_flt = 13.0
    @den_flt = 4.0
    @num_bigint = 2**33
    @den_bigint = 2**32
  end
  
  it "div right  integers" do 
    @num_int.div(@den_int).should == 3
    @den_int.div(@num_int).should == 0
  end
  
  it "div right  integers and floats" do 
    @num_int.div(@den_flt).should == 3
    @den_int.div(@num_int).should == 0
  end
  
  it "div right the integers and floats" do 
    @num_int.div(@den_flt).should == 3
    @den_int.div(@num_int).should == 0
  end
  
  it "div right  floats" do 
    @num_flt.div(@den_flt).should == 3
    @den_flt.div(@num_int).should == 0
  end
  
  it " should div right with bignums and integers" do
    @num_bigint.div(@den_bigint).should == 2
  end
  
  it "raise the expected exception" do
    should_raise(ArgumentError){ @num_int.div }
    should_raise(ZeroDivisionError){ @num_int.div(0) }
    should_raise(TypeError){ @num_int.div(nil) }
    should_raise(TypeError){ @num_int.div('test') }
    should_raise(TypeError){ @num_int.div(true) }   
  end
  
  after(:each) do 
    @num_int = nil
    @den_int = nil
    @num_flt = nil
    @den_flt = nil
    @num_bigint = nil
    @den_bigint = nil
  end
end


describe " Numeric#eql?" do 
  before(:each) do 
    @integer  = 1
    @float    = 1.0
    @bignum   = 4294967296
    @bigfloat = 4294967296.0
  end
  
  it "be equal (integers and floats)" do 
    @integer.eql?(@integer).should == true
    @integer.eql?(@float).should == false
    @float.eql?(@float).should == true
    @float.eql?(@integer).should == false
  end
  
  it " should be equal (bignums and floats " do
    @bignum.eql?(@bignum).should == true 
    @bignum.eql?(@bigfloat).should == false
  end
  
  it "be equal (edge cases)" do  
    0.eql?(0).should ==  true
    1.0.eql?(1.00000000000000000000).should == true
    0.eql?(0.0).should ==  false
    000000.eql?(0.0).should ==  false
    000000.eql?(000) .should ==  true
  end
  
  after(:each) do 
    @integer  = nil
    @float    = nil
    @bignum   = nil
    @bigfloat = nil
  end
end


# Returns the largest integer less than or equal to num. Numeric implements 
# this by converting anInteger to a Float and invoking Float#floor.
describe "Numeric#floor" do 
  before(:each) do 
    @num_zero  = 0
    @num_pos   = 100
    @num_neg   = -100 
    @num_twop  = 2147483648
    @num_twon  = -2147483648
    @num_twopb = 9223372036854775808
    @num_twonb = -9223372036854775808
  end  
  
  it "return the largest integer less than or equal to num (integer)" do 
    @num_zero.floor.should == 0
    @num_pos.floor.should == 100
    @num_neg.floor.should == -100
    
  end   
  
  it "return the largest integer less than or equal to num (two complement)" do 
    @num_twop.floor.should == 2147483648
    @num_twon.floor.should == -2147483648
    @num_twopb.floor.should == 9223372036854775808
    @num_twonb.floor.should == -9223372036854775808
  end  
  
  after(:each) do 
    @num_zero  = nil
    @num_pos   = nil
    @num_neg   = nil
    @num_twop  = nil
    @num_twon  = nil
    @num_twopb = nil
    @num_twonb = nil
  end
end

# Returns true if num is an Integer (including Fixnum and Bignum).
describe "Numeric#integer?" do 
  before(:each) do 
    @num_zero  = 0
    @num_pos   = 100
    @num_neg   = -100
    @num_posf  = 34.56
    @num_negf  = -34.56
    @num_twop  = 2147483648
    @num_twon  = -2147483648
    @num_twopb = 9223372036854775808
    @num_twonb = -9223372036854775808  
  end  
  
  it "retrun true if the num is an integer?" do 
    @num_zero.integer?.should == true
    @num_pos.integer?.should  == true
    @num_neg.integer?.should  == true
    @num_posf.integer?.should == false
    @num_negf.integer?.should == false
    @num_twop.integer?.should == true
    @num_twon.integer?.should == true
    @num_twopb.integer?.should == true
    @num_twonb.integer?.should == true
  end  
  
  after(:each) do     
    @num_zero  = nil
    @num_pos   = nil
    @num_neg   = nil
    @num_posf  = nil
    @num_negf  = nil
    @num_twop  = nil
    @num_twon  = nil
    @num_twopb = nil
    @num_twonb = nil
  end
end



describe "Numeric#modulo" do 
  before(:each) do 
    @num_zero  = 0
    @num_pos   = 100
    @num_neg   = -100
    @num_posf  = 34.56
    @num_negf  = -34.56
    @num_twop  = 2147483648
    @num_twon  = -2147483648
    @num_twopb = 9223372036854775808
    @num_twonb = -9223372036854775808    
    @den_bigint = 2**31
    @num_bigint = 2**31 
    @den_bigint_n = -3**31
    @num_bigint_n = -3**31
  end  
  
  it " zero modulo x should be 0 " do 
    @num_zero.modulo(@num_pos).should == 0
    @num_zero.modulo(@num_neg).should == 0
    @num_zero.modulo(@num_posf).should == 0
    @num_zero.modulo(@num_negf).should == 0
    @num_zero.modulo(@num_twop).should == 0
    @num_zero.modulo(@num_twon).should == 0
    @num_zero.modulo(@den_bigint).should == 0
    @num_zero.modulo(@num_bigint).should == 0
    @num_zero.modulo(@den_bigint_n).should == 0
    @num_zero.modulo(@num_bigint_n).should == 0
  end
  
  it "y (integer) modulo x should be z  "do 
    @num_pos.modulo(@num_pos).should == 0
    @num_neg.modulo(@num_pos).should == 0
    @num_pos.modulo(@num_neg).should == 0
    @num_neg.modulo(@num_neg).should == 0
    @num_pos.modulo(@num_posf).should_be_close(30.88,TOLERANCE) 
    @num_neg.modulo(@num_posf).should_be_close(3.68000000000001 ,TOLERANCE)
    @num_pos.modulo(@num_negf).should_be_close(-3.68000000000001,TOLERANCE)
    @num_neg.modulo(@num_negf).should_be_close(-30.88,TOLERANCE)
    @num_pos.modulo(@num_twop).should == 100 
    @num_neg.modulo(@num_twop).should == 2147483548
    @num_pos.modulo(@num_twon).should == -2147483548
    @num_neg.modulo(@num_twon).should == -100
    @num_pos.modulo(@den_bigint).should == 100
    @num_neg.modulo(@den_bigint).should == 2147483548
    @num_pos.modulo(@den_bigint_n).should == -617673396283847
    @num_neg.modulo(@den_bigint_n).should == -100    
  end  

  it "y (float) modulo x should be z  "do 
    @num_posf.modulo(@num_pos).should_be_close(34.56,TOLERANCE)
    @num_negf.modulo(@num_pos).should_be_close(65.44,TOLERANCE)
    @num_posf.modulo(@num_neg).should_be_close(-65.44,TOLERANCE)
    @num_negf.modulo(@num_neg).should_be_close(-34.56,TOLERANCE)
    @num_posf.modulo(@num_posf).should_be_close(0.0,TOLERANCE)
    @num_negf.modulo(@num_posf).should_be_close(0.0,TOLERANCE)
    @num_posf.modulo(@num_negf).should_be_close(0.0,TOLERANCE)
    @num_negf.modulo(@num_negf).should_be_close(0.0,TOLERANCE)
    @num_posf.modulo(@num_twop).should_be_close(34.56,TOLERANCE)
    @num_negf.modulo(@num_twop).should_be_close(2147483613.44 ,TOLERANCE)
    @num_posf.modulo(@num_twon).should_be_close(-2147483613.44 ,TOLERANCE)
    @num_negf.modulo(@num_twon).should_be_close(-34.56,TOLERANCE)
    @num_posf.modulo(@den_bigint).should_be_close(34.56,TOLERANCE)
    @num_negf.modulo(@den_bigint).should_be_close(2147483613.44 ,TOLERANCE)
    @num_posf.modulo(@den_bigint_n).to_s.should == -6.17673396283912e+14.to_s
    @num_negf.modulo(@den_bigint_n).should_be_close(-34.56,TOLERANCE)
  end
  
  it "y (integer) modulo x should be z  "do 
    @num_twop.modulo(@num_pos).should == 48
    @num_twon.modulo(@num_pos).should == 52
    @num_twop.modulo(@num_neg).should == -52
    @num_twon.modulo(@num_neg).should == -48
    @num_twop.modulo(@num_posf).should_be_close(1.27999985871492, TOLERANCE)
    @num_twon.modulo(@num_posf).should_be_close(33.2800001412851 , TOLERANCE)
    @num_twop.modulo(@num_negf).should_be_close(-33.2800001412851 , TOLERANCE)
    @num_twon.modulo(@num_negf).should_be_close(-1.27999985871492, TOLERANCE)
    @num_twop.modulo(@num_twop).should == 0
    @num_twon.modulo(@num_twop).should == 0
    @num_twop.modulo(@num_twon).should == 0
    @num_twon.modulo(@num_twon).should == 0
    @num_twop.modulo(@den_bigint).should == 0
    @num_twon.modulo(@den_bigint).should == 0
    @num_twop.modulo(@den_bigint_n).should == -617671248800299 
    @num_twon.modulo(@den_bigint_n).should == -2147483648
  end
  #  
  #  it " should return the same value with opposite sign (floats)" do 
  #    @num_posf.modulo(:-@).should == -34.56
  #    @num_negf.modulo(:-@).should == 34.56
  #  end  
  #
  #  it " should return the same value with opposite sign (floats)" do 
  #    @den_bigint.modulo(:-@).should == -2**31
  #    @num_bigint_n.modulo(:-@).should == 2**31
  #  end  
  #
  #  it " should return the same value with opposite sign (two complement)" do 
  #    @num_twop.modulo(:-@).should == -2147483648
  #    @num_twon.modulo(:-@).should == 2147483648
  #    @num_twopb.modulo(:-@).should == -9223372036854775808
  #    @num_twonb.modulo(:-@).should == 9223372036854775808
  #  end  
  
  it " should NOT raise ZeroDivisionError if other is zero and is a Float" do
    1.modulo(0.0).to_s.should == 'NaN'
    1.modulo(0.0).to_s.should == 'NaN'
  end
  
  it " should raise an Exception when divide by 0 (non float)" do    
    should_raise(ZeroDivisionError){ @num_int.modulo(0) }
  end
  
  after(:each) do 
    @num_zero  = nil
    @num_pos   = nil
    @num_neg   = nil
    @num_posf  = nil
    @num_negf  = nil
    @num_twop  = nil
    @num_twon  = nil
    @num_twopb = nil
    @num_twonb = nil 
    @den_bigint = nil
    @num_bigint_n = nil
  end
end


# Returns num if num is not zero, nil otherwise. 
# This behavior is useful when chaining comparisons:
describe "Numeric#nonzero?" do 
  before(:each) do 
    @zero    = 0
    @nonzero = 1
  end  
  
  it "return the vaule if number is different to 0" do 
    @zero.nonzero?.should == nil
    @nonzero.nonzero?.should == 1
  end  
  
  after(:each) do     
    @zero    = nil
    @nonzero = nil
  end
end

describe "Numeric#to_int" do 
  before(:each) do 
    @num_zero  = 0.0
    @num_pos   = 100
    @num_neg   = -100
    @num_posf  = 34.56
    @num_negf  = -34.56
    @num_twop  = 2147483648
    @num_twon  = -2147483648
    @num_twopb = 9223372036854775808.9223372036854775808
    @num_twonb = -9223372036854775808.000000000000000000
  end
  
  it "return the integer (integers)" do 
    @num_zero.to_int.should == 0
    @num_pos.to_int.should  == 100
    @num_neg.to_int.should == -100
  end  
  
  it "return the integer part (float)" do 
    @num_posf.to_int.should == 34 
    @num_negf.to_int.should == -34
  end  
  
  it "return the integer part (two complement)" do    
    @num_twop.to_int.should == 2147483648
    @num_twon.to_int.should == -2147483648
    @num_twopb.to_int.should == 9223372036854775808
    @num_twonb.to_int.should == -9223372036854775808
  end  
   
  after(:each) do     
    @num_zero  = nil
    @num_pos   = nil
    @num_neg   = nil
    @num_posf  = nil
    @num_negf  = nil
    @num_twop  = nil
    @num_twon  = nil
    @num_twopb = nil
    @num_twonb = nil
  end
end

# If num and numeric have different signs, returns mod-numeric;
# otherwise, returns mod. In both cases mod is the value num.modulo(numeric)
describe "Numeric#remainder" do 
  before(:each) do 
    @num_int = 13
    @den_int = 4
    @num_flt = 13.0
    @den_flt = 4.0    
    @num_int_n = -13
    @den_int_n = -4
    @num_flt_n = -13.0
    @den_flt_n = -4.0    
    @num_bigint = 2**33
    @den_bigint = 3**32
    @num_bigint_n = -2**31
    @den_bigint_n = -3**31
  end
  
  it "remainder the right  integers" do 
    @num_int.remainder(@den_int).should == 1
    @num_int.remainder(@den_int_n).should == 1
    @num_int_n.remainder(@den_int).should == -1
    @num_int_n.remainder(@den_int_n).should == -1
  end
  
  it "remainder right  integers and floats" do 
    @num_int.remainder(@den_flt).should == 1.0
    @num_int.remainder(@den_flt_n).should == 1.0
    @num_int_n.remainder(@den_flt).should == -1.0
    @num_int_n.remainder(@den_flt_n).should == -1.0
  end
  
  it "remainder right the integers and floats" do 
    11.5.remainder(@den_flt).should == 3.5
    11.5.remainder(@den_flt_n).should == 3.5
    -11.5.remainder(@den_flt).should == -3.5
    -11.5.remainder(@den_flt_n).should == -3.5
  end
  
  it " should remainder right with bignums and integers" do
    @num_bigint.remainder(@den_bigint).should == 8589934592
    @num_bigint.remainder(@den_bigint_n).should == 8589934592
    @num_bigint_n.remainder(@den_bigint).should == -2147483648
    @num_bigint_n.remainder(@den_bigint_n).should == -2147483648
  end
  
  it "raise the expected exception" do
    should_raise(ArgumentError){ @num_int.remainder }
    should_raise(ZeroDivisionError){ @num_int.remainder(0) }
    should_raise(TypeError){ @num_int.remainder(nil) }
    should_raise(TypeError){ @num_int.remainder('test') }
    should_raise(TypeError){ @num_int.remainder(true) }   
  end  
   
  after(:each) do 
    @num_int = nil
    @den_int = nil
    @num_flt = nil
    @den_flt = nil
    @num_bigint = nil
    @den_bigint = nil
  end
end



# Rounds num to the nearest integer
describe "Numeric#round" do 
  before(:each) do 
    @num_zero  = 0.0
    @num_pos   = 34.49999
    @num_neg   = -34.44999
    @num_posf  = 34.5001
    @num_negf  = -34.5001
    @num_twopb = 9223372036854775808.9223372036854775808
    @num_twonb = -9223372036854775808.000000000000000000
  end  
  
  it " round (down) " do 
    @num_zero.round.should == 0.0
    @num_pos.round.should == 34
    @num_neg.round.should == -34
  end  
  
  it " round (up) " do  
    @num_posf.round.should == 35
    @num_negf.round.should == -35
  end  
  
  it " round twos complement " do
    @num_twopb.round.should == 9223372036854775808 
    @num_twonb.round.should == -9223372036854775808 
  end  
  
  after(:each) do  
    @num_zero  = nil
    @num_pos   = nil
    @num_neg   = nil
    @num_posf  = nil
    @num_negf  = nil
    @num_twop  = nil
    @num_twon  = nil
    @num_twopb = nil
    @num_twonb = nil    
  end
end

describe "Numeric#truncate" do 
  before(:each) do 
    @num_zero  = 0.0
    @num_pos   = 100
    @num_neg   = -100
    @num_posf  = 34.56
    @num_negf  = -34.56
    @num_twop  = 2147483648
    @num_twon  = -2147483648
    @num_twopb = 9223372036854775808.9223372036854775808
    @num_twonb = -9223372036854775808.000000000000000000
  end  
  
  it " truncate integers " do 
    @num_zero.truncate.should == 0.0
    @num_pos.truncate.should == 100
    @num_neg.truncate.should == -100
  end  
  
  it " truncate floats " do  
    @num_posf.truncate.should == 34
    @num_negf.truncate.should == -34
  end  
  
  it " truncate two complement " do
    @num_twop.truncate.should == 2147483648
    @num_twon.truncate.should == -2147483648
    @num_twopb.truncate.should == 9223372036854775808 
    @num_twonb.truncate.should == -9223372036854775808 
  end
  
  after(:each) do     
    @num_zero  = nil
    @num_pos   = nil
    @num_neg   = nil
    @num_posf  = nil
    @num_negf  = nil
    @num_twop  = nil
    @num_twon  = nil
    @num_twopb = nil
    @num_twonb = nil  
  end
end

# Invokes block with the sequence of numbers starting at num, incremented by 
# step on each call. The loop finishes when the value to be passed to the block 
# is greater than limit (if step is positive) or less than limit (if step is negative).
# If all the arguments are integers, the loop operates using an integer counter.
describe "Numeric#step" do 
  before(:each) do 
    @step  = 2
    @stepn = -2
    @stepnf = -2
    @stepf = 2.0
    @limitf = 10.0
    @limit = 10
    @base  = 1
    @basef = 5.4
  end  
  
  it "if base < limit > step then it should iterate (base-limit)/step times (integers)" do 
    x = []
    @base.step(@limit, @step) { |i| x << i }
    x.should == [1, 3, 5, 7, 9]  
  end
  
  it "iterate one time if step is bigger than base-limit (integers)" do 
    x = []
    @base.step(@limit, 11) { |i| x<< i }
    x.should == [1]    
  end
  
  it "not iterate if base is bigger than limit and step >0 (integers)" do 
    x = []
    12.step(@limit, @step) { |i| x<< i }
    x.should == []  
  end  
  
  it "iterate backward if base is bigger than limit (integers)" do 
    x = []
    10.step(1, @stepn) { |i| x << i}
    x.should == [10, 8, 6, 4, 2]  
  end  
  
  it "not iterate if base is minor than limit and step <0 (integers)" do 
    x = []
    @base.step(@limit, @stepn) { |i| x<< i }
    x.should == []  
  end  
  
  
  it "if base < limit > step then it should iterate (base-limit)/step times (integers)" do 
    x = []
    @base.step(@limit, @step) { |i| x << i }
    x.should == [1, 3, 5, 7, 9]  
  end
  
  it "iterate one time if step is bigger than base-limit (integers)" do 
    x = []
    @base.step(@limit, 11) { |i| x<< i }
    x.should == [1]    
  end
      
  it "if base < limit > step then it should iterate (base-limit)/step times (floats)" do 
    x = []
    @basef.step(@limitf, @stepf) { |i| x << i }
    x.should == [5.4, 7.4, 9.4]  
  end
  
  it "iterate one time if step is bigger than base-limit (floats)" do 
    x = []
    @basef.step(@limitf, 11) { |i| x<< i }
    x.should == [5.4]    
  end
  
  it "not iterate if base is bigger than limit and step >0 (floats)" do 
    x = []
    12.0.step(@limitf, @stepf) { |i| x<< i }
    x.should == []  
  end  
  
  it "iterate backward if base is bigger than limit (floats)" do 
    x = []
    10.0.step(1.0, @stepnf) { |i| x << i}
    x.should == [10, 8, 6, 4, 2]  
  end  
  
  it "not iterate if base is minor than limit and step <0 (floats)" do 
    x = []
    @basef.step(@limitf, @stepnf) { |i| x<< i }
    x.should == []  
  end   
  
  it "if base < limit > step then  iterate (base-limit)/step times (floats)" do 
    x = []
    @basef.step(@limitf, @stepf) { |i| x << i }
    x.should == [5.4, 7.4, 9.4]  
  end
  
  it "raise the expected exception" do
    should_raise(ArgumentError){ @base.step }
    should_raise(ArgumentError){ @base.step(100,0) }
    should_raise(ArgumentError){ @base.step(nil) }
    should_raise(ArgumentError){ @base.step('test') }
    should_raise(ArgumentError){ @base.step(true, 123) }   
    should_raise(LocalJumpError){ @base.step(5, 3.4) }         
    should_raise(LocalJumpError){ @base.step(5.0, 2) }               
    should_raise(LocalJumpError){ @base.step(5.0, 1.0) }   
  end
  
  after(:each) do 
    
  end
end

describe "Numeric#zero?" do 
  before(:each) do 
    @zero    = 0
    @nonzero = 1
  end  
  
  it "return the vaule if number is different to 0" do 
    @zero.zero?.should == true
    @nonzero.zero?.should == false
  end  
  
  after(:each) do     
    @zero    = nil
    @nonzero = nil
  end
end

describe "Numeric#coerce" do 
  before(:each) do 
    @integer = 1
    @float   = 2.5
    class String
      def coerce(other)
        case other
        when Integer
          begin
            return other, Integer(self)
          rescue
            return Float(other), Float(self)
          end
        when Float
          return other, Float(self)
        else
          super
        end
      end
    end
    
  end  
  
  it "coerce integers" do 
    @integer.coerce(@integer).should == [1,1]
    @integer.coerce(@float).should ==[2.5,1]
  end  
  
  it "coerce floats" do 
    @float.coerce(@float).should == [2.5,2.5]
    @float.coerce(@integer).should == [1,2.5]
  end  
  
  it "coerce with 0" do 
    @integer.coerce(0).should == [0,1]
    @float.coerce(0).should == [0,2.5]
  end  
  
  it "coerce with bignums " do 
    @integer.coerce(4294967296).should == [4294967296, 1]
    @float.coerce(4294967296).should == [4294967296.0, 2.5]
  end  
 
  it "coerce strings to strings" do 
    3.should == 1 + "2"
    -1.3.should_be_close((1 - "2.3"), 0.001)
    3.5.should == 1.2 + "2.3"
    0.should == 1 - "1"
  end  
  
  it "return the vaule if number is different to 0" do 
    should_raise(ArgumentError){ @integer.coerce("test") }
    should_raise(TypeError){ @integer.coerce(nil) }
    should_raise(TypeError){ @integer.coerce(false) }      
    should_raise(ArgumentError){ @integer.coerce }
  end  
  
  after(:each) do     
    @zero    = nil
    @nonzero = nil
  end
end

warn ' Numeric have only a few specs'
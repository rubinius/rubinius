require File.dirname(__FILE__) + '/../spec_helper'

TOLERANCE = 0.00003

# TODO <=> angle arg  conj conjugate   
# im imag image  polar real  singleton_method_added    
zero  = 0
zero_float = 0.0
nonzero = 1
a_possitive_number   = 100
a_negative_number   = -100
a_possitive_float  = 34.56
a_negative_float  = -34.56
a_possitive_bignumber  = 2147483648
a_negative_bignumber  = -2147483648
a_num_twopb = 9223372036854775808
a_num_twonb = -9223372036854775808     
a_negative_num_bignumber = -2**31
a_possitive_den_bignumber = 2**31

# numerators and denominators 
a_num_int = 13
a_num_int_n = -13
a_den_int = 4
a_den_int_n = -4
a_num_flt = 13.0
a_den_flt = 4.0
a_den_flt_n = -4.0

a_num_bigint = 3**33 

describe "Numeric#-@" do    
  it " should return the same value with opposite sign (integers)" do 
    0.send(:-@).should == 0
    a_possitive_number.send(:-@).should == -100
    a_negative_number.send(:-@).should == 100 
  end  

  it " should return the same value with opposite sign (floats)" do 
    a_possitive_float.send(:-@).should == -34.56
    a_negative_float.send(:-@).should == 34.56
  end   

  it " should return the same value with opposite sign (two complement)" do 
    a_possitive_bignumber.send(:-@).should == -2147483648
    a_negative_bignumber.send(:-@).should == 2147483648
    a_num_twopb.send(:-@).should == -9223372036854775808
    a_num_twonb.send(:-@).should == 9223372036854775808
  end  
end

describe "Numeric#+@" do    
  it " should return the same value with opposite sign (integers)" do 
    0.send(:+@).should == 0
    a_possitive_number.send(:+@).should == 100
    a_negative_number.send(:+@).should == -100
    
  end  

  it " should return the same value with opposite sign (floats)" do 
    a_possitive_float.send(:+@).should == 34.56
    a_negative_float.send(:+@).should == -34.56
  end  

  it " should return the same value with opposite sign (floats)" do 
    a_possitive_den_bignumber.send(:+@).should == 2**31
    a_negative_num_bignumber.send(:+@).should == -2**31
  end  

  it " should return the same value with opposite sign (two complement)" do 
    a_possitive_bignumber.send(:+@).should == 2147483648
    a_negative_bignumber.send(:+@).should == -2147483648
    a_num_twopb.send(:+@).should == 9223372036854775808
    a_num_twonb.send(:+@).should == -9223372036854775808
  end  
end

# Returns the smallest Integer greater than or equal to num. 
# Class Numeric achieves this by converting itself to a Float then invoking Float#ceil.
describe "Numeric#ceil " do  
  it "ceil to integer " do
    0.ceil.should == 0
    a_possitive_number.ceil.should == 100
    a_negative_number.ceil.should == -100
  end
  
  it " should ceil to float " do 
    0.ceil.should == 0.0
    a_possitive_float.ceil.should == 35
    a_negative_float.ceil.should == -34
  end
  
  it "ceil twos complement" do 
    a_possitive_bignumber.ceil.should == 2147483648
    a_negative_bignumber.ceil.should == -2147483648
    a_num_twopb.ceil.should == 9223372036854775808
    a_num_twonb.ceil.should == -9223372036854775808  
  end
end

# Returns the absolute value of num.
describe "Numeric#abs" do  
  it "return the abs (integers) " do      
    0.abs.should == 0 
    a_possitive_number.abs.should  == 100
    a_negative_number.abs.should   == 100
  end
  
  it "return the abs (floats) " do 
    zero_float.abs.should == 0.0
    a_possitive_float.abs.should == 34.56
    a_negative_float.abs.should == 34.56 
  end
  
  it "return the abs (two complement)" do
    a_possitive_bignumber.abs.should == 2147483648
    a_negative_bignumber.abs.should == 2147483648
    a_num_twopb.abs.should == 9223372036854775808
    a_num_twonb.abs.should == 9223372036854775808    
  end 
end

# Returns an array containing the quotient and modulus obtained
# by dividing num by aNumeric. If q, r = x.divmod(y), then
#    q = floor(float(x)/float(y))
#    x = q*y + r
describe "Numeric#divmod" do    
  it "divmod right  integers" do 
    a_num_int.divmod(a_den_int).should == [3,1]
    a_den_int.divmod(a_num_int).should == [0,4]
  end
  
  it "divmod right  integers and floats" do 
    a_num_int.divmod(a_den_flt).should == [3,1]
    a_den_int.divmod(a_num_int).should == [0,4]
  end
  
  it "divmod right the integers and floats" do 
    a_num_int.divmod(a_den_flt).should == [3,1]
    a_den_int.divmod(a_num_int).should == [0,4]
  end
  
  it "divmod right  floats" do 
    a_num_flt.divmod(a_den_flt).should == [3.0,1.0]
    a_den_flt.divmod(a_num_int).should == [0.0,4.0]
  end
  
  it " should divmod right with bignums and integers" do
    a_num_bigint.divmod( a_possitive_number).should == [55590605665555, 23]
  end
  
  it "raise the expected exception" do
    should_raise(ArgumentError){ a_num_int.divmod }
    should_raise(ZeroDivisionError){ a_num_int.divmod(0) }
    should_raise(TypeError){ a_num_int.divmod(nil) }
    should_raise(TypeError){ a_num_int.divmod('test') }
    should_raise(TypeError){ a_num_int.divmod(true) }   
  end 
end

# Equivalent to Numeric#/, but overridden in subclasses.
describe "Numeric#quo" do  
  specify "quo should return the floating-point result of self divided by other" do
    # the to_f is required because RSpec (I'm assuming) requires 'rational'
    2.quo(2.5).to_s.should == '0.8'
    5.quo(2).to_f.to_s.should == '2.5' 
  end

  specify "quo should NOT raise an exception when other is zero" do
    # 1.quo(0) should also not raise (i.e works in irb and from a file),
    # but fails here.
    1.quo(0.0).to_s.should == 'Infinity'
    1.quo(-0.0).to_s.should == '-Infinity'
  end

  it "quo right  integers" do 
    a_num_int.quo(a_den_int).should_be_close(3.25, TOLERANCE)
    a_den_int.quo(a_num_int).should_be_close( 0.307692307692308, TOLERANCE)
  end
     
  it "quo right  integers and floats" do 
    a_num_int.quo(a_den_flt).should_be_close(3.25, TOLERANCE)
    a_den_int.quo(a_num_int).should_be_close(0.307692307692308, TOLERANCE)
  end
  
  it "quo right the integers and floats" do 
    a_num_int.quo(a_den_flt).should_be_close(3.25, TOLERANCE)
    a_den_int.quo(a_num_int).should_be_close(0.307692307692308, TOLERANCE)
  end
  
  it "quo right  floats" do 
    a_num_flt.quo(a_den_flt).should_be_close(3.25, TOLERANCE)
    a_den_flt.quo(a_num_int).should_be_close(0.307692307692308, TOLERANCE)
  end
   
  it " should quo right with bignums and integers" do
    a_possitive_bignumber.quo( a_possitive_number).to_s.should == 21474836.48.to_s
  end
  
  it "not raise a Exception when quo by 0" do
    a_num_int.quo(0)
    a_num_flt.quo(0)
    a_num_bigint.quo(0)
  end
  
  it "raise the expected exception" do
    should_raise(ArgumentError){ a_num_int.quo } 
    should_raise(TypeError){ a_num_int.quo(nil) }
    should_raise(TypeError){ a_num_int.quo('test') }
    should_raise(TypeError){ a_num_int.quo(true) }   
  end 
end
 
# Uses / to perform division, then converts the result to an integer
describe "Numeric#div" do  
  it "div right  integers" do 
    a_num_int.div(a_den_int).should == 3
    a_den_int.div(a_num_int).should == 0
  end
  
  it "div right  integers and floats" do 
    a_num_int.div(a_den_flt).should == 3
    a_den_int.div(a_num_int).should == 0
  end
  
  it "div right the integers and floats" do 
    a_num_int.div(a_den_flt).should == 3
    a_den_int.div(a_num_int).should == 0
  end
  
  it "div right  floats" do 
    a_num_flt.div(a_den_flt).should == 3
    a_den_flt.div(a_num_int).should == 0
  end
  
  it " should div right with bignums and integers" do
    a_num_bigint.div( a_possitive_number).should == 55590605665555
  end
  
  it "raise the expected exception" do
    should_raise(ArgumentError){ a_num_int.div }
    should_raise(ZeroDivisionError){ a_num_int.div(0) }
    should_raise(TypeError){ a_num_int.div(nil) }
    should_raise(TypeError){ a_num_int.div('test') }
    should_raise(TypeError){ a_num_int.div(true) }   
  end
end


describe " Numeric#eql?" do 
  before(:each) do 
    @integer  = 1
    @float    = 1.0
    @bignum   = 4294967296
    @bigfloat = 4294967296.0
  end

  after(:each) do 
    @integer  = nil
    @float    = nil
    @bignum   = nil
    @bigfloat = nil
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
end


# Returns the largest integer less than or equal to num. Numeric implements 
# this by converting anInteger to a Float and invoking Float#floor.
describe "Numeric#floor" do 
  it "return the largest integer less than or equal to num (integer)" do 
    0.floor.should == 0
    a_possitive_number.floor.should == 100
    a_negative_number.floor.should == -100 
  end   
  
  it "return the largest integer less than or equal to num (two complement)" do 
    a_possitive_bignumber.floor.should == 2147483648
    a_negative_bignumber.floor.should == -2147483648
    a_num_twopb.floor.should == 9223372036854775808
    a_num_twonb.floor.should == -9223372036854775808
  end   
end

# Returns true if num is an Integer (including Fixnum and Bignum).
describe "Numeric#integer?" do  
  it "retrun true if the num is an integer?" do 
    0.integer?.should == true
    a_possitive_number.integer?.should  == true
    a_negative_number.integer?.should  == true
    a_possitive_float.integer?.should == false
    a_negative_float.integer?.should == false
    a_possitive_bignumber.integer?.should == true
    a_negative_bignumber.integer?.should == true
    a_num_twopb.integer?.should == true
    a_num_twonb.integer?.should == true
  end   
end 

describe "Numeric#modulo" do 
  it " zero modulo x should be 0 (integer) " do 
    0.modulo(a_possitive_number).should == 0
    0.modulo(a_negative_number).should == 0
  end
  
  it " zero modulo x should be 0 (float) " do 
    0.modulo(a_possitive_float).should == 0
    0.modulo(a_negative_float).should == 0
  end
  
  it " zero modulo x should be 0 (bignum) " do 
    0.modulo(a_possitive_bignumber).should == 0
    0.modulo(a_negative_bignumber).should == 0 
  end
  
  it "y modulo x should be z (integer - integer) " do 
    a_possitive_number.modulo(a_possitive_number).should == 0
    a_possitive_number.modulo(a_negative_number).should == 0
    a_negative_number.modulo(a_possitive_number).should == 0
    a_negative_number.modulo(a_negative_number).should == 0
  end
  
  it "y modulo x should be z (integer - float) " do 
    a_possitive_number.modulo(a_possitive_float).should_be_close(30.88,TOLERANCE) 
    a_possitive_number.modulo(a_negative_float).should_be_close(-3.68000000000001,TOLERANCE)
    a_negative_number.modulo(a_possitive_float).should_be_close(3.68000000000001 ,TOLERANCE)
    a_negative_number.modulo(a_negative_float).should_be_close(-30.88,TOLERANCE)
  end
  
  it "y modulo x should be z (integer - bignum) " do 
    a_possitive_number.modulo(a_possitive_bignumber).should == 100 
    a_possitive_number.modulo(a_negative_bignumber).should == -2147483548
    a_negative_number.modulo(a_negative_bignumber).should == -100 
    a_negative_number.modulo(a_possitive_bignumber).should == 2147483548
  end  

  it "modulo x should be z  (floats and integers)"do 
    a_possitive_float.modulo(a_possitive_number).should_be_close(34.56,TOLERANCE)
    a_possitive_float.modulo(a_negative_number).should_be_close(-65.44,TOLERANCE)
    a_negative_float.modulo(a_negative_number).should_be_close(-34.56,TOLERANCE)    
    a_negative_float.modulo(a_possitive_number).should_be_close(65.44,TOLERANCE)    
  end
  
  it "modulo x should be z  (float - float)"do     
    a_possitive_float.modulo(a_possitive_float).should_be_close(0.0,TOLERANCE)
    a_possitive_float.modulo(a_negative_float).should_be_close(0.0,TOLERANCE)
    a_negative_float.modulo(a_negative_float).should_be_close(0.0,TOLERANCE)
    a_negative_float.modulo(a_possitive_float).should_be_close(0.0,TOLERANCE)
  end
  
  it "modulo x should be z  (float - bignum)"do     
    a_possitive_float.modulo(a_possitive_bignumber).should_be_close(34.56,TOLERANCE)
    a_possitive_float.modulo(a_negative_bignumber).should_be_close(-2147483613.44 ,TOLERANCE)
    a_negative_float.modulo(a_possitive_bignumber).should_be_close(2147483613.44 ,TOLERANCE) 
  end

  it "y modulo x should be z (bignum -integer) "do 
    a_possitive_bignumber.modulo(a_possitive_number).should == 48
    a_possitive_bignumber.modulo(a_negative_number).should == -52    
    a_negative_bignumber.modulo(a_possitive_number).should == 52
    a_negative_bignumber.modulo(a_negative_number).should == -48
  end
  
  it "y modulo x should be z (bignum - float) "do 
    a_possitive_bignumber.modulo(a_possitive_float).should_be_close(1.27999985871492, TOLERANCE)
    a_negative_bignumber.modulo(a_possitive_float).should_be_close(33.2800001412851 , TOLERANCE)
    a_possitive_bignumber.modulo(a_negative_float).should_be_close(-33.2800001412851 , TOLERANCE)
    a_negative_bignumber.modulo(a_negative_float).should_be_close(-1.27999985871492, TOLERANCE)
  end
  
  it "y modulo x should be z (bignum - bignum) "do   
    a_possitive_bignumber.modulo(a_possitive_bignumber).should == 0
    a_negative_bignumber.modulo(a_possitive_bignumber).should == 0
    a_possitive_bignumber.modulo(a_negative_bignumber).should == 0
    a_negative_bignumber.modulo(a_negative_bignumber).should == 0 
  end 
  
  it " should NOT raise ZeroDivisionError if other is zero and is a Float" do
    1.modulo(0.0).to_s.should == 'NaN'
    1.modulo(0.0).to_s.should == 'NaN'
  end
  
  it " should raise an Exception when divide by 0 (non float)" do    
    should_raise(ZeroDivisionError){ a_num_int.modulo(0) }
  end
end


# Returns num if num is not zero, nil otherwise. 
# This behavior is useful when chaining comparisons:
describe "Numeric#nonzero?" do 
  it "return the vaule if number is different to 0" do 
    0.nonzero?.should == nil
    1.nonzero?.should == 1
  end  
end

describe "Numeric#to_int" do  
  it "return the integer (integers)" do 
    0.to_int.should == 0
    a_possitive_number.to_int.should  == 100
    a_negative_number.to_int.should == -100
  end  
  
  it "return the integer part (float)" do 
    a_possitive_float.to_int.should == 34 
    a_negative_float.to_int.should == -34
  end  
  
  it "return the integer part (two complement)" do    
    a_possitive_bignumber.to_int.should == 2147483648
    a_negative_bignumber.to_int.should == -2147483648
    a_num_twopb.to_int.should == 9223372036854775808
    a_num_twonb.to_int.should == -9223372036854775808
  end   
end

# If num and numeric have different signs, returns mod-numeric;
# otherwise, returns mod. In both cases mod is the value num.modulo(numeric)
describe "Numeric#remainder" do    
  it "remainder the right  integers" do 
    a_num_int.remainder(a_den_int).should == 1
    a_num_int.remainder(a_den_int_n).should == 1
    a_num_int_n.remainder(a_den_int).should == -1
    a_num_int_n.remainder(a_den_int_n).should == -1
  end
  
  it "remainder right  integers and floats" do 
    a_num_int.remainder(a_den_flt).should == 1.0
    a_num_int.remainder(a_den_flt_n).should == 1.0
    a_num_int_n.remainder(a_den_flt).should == -1.0
    a_num_int_n.remainder(a_den_flt_n).should == -1.0
  end
  
  it "remainder right the integers and floats" do 
    11.5.remainder(a_den_flt).should == 3.5
    11.5.remainder(a_den_flt_n).should == 3.5
    -11.5.remainder(a_den_flt).should == -3.5
    -11.5.remainder(a_den_flt_n).should == -3.5
  end
  
  it " should remainder right with bignums and integers" do
    a_num_bigint.remainder( a_negative_number).should ==  23
    a_num_bigint.remainder( a_possitive_number).should == 23
    a_negative_num_bignumber.remainder( a_possitive_den_bignumber).should == 0
    a_negative_num_bignumber.remainder( a_possitive_den_bignumber).should == 0
  end
  
  it "raise the expected exception" do
    should_raise(ArgumentError){ a_num_int.remainder }
    should_raise(ZeroDivisionError){ a_num_int.remainder(0) }
    should_raise(TypeError){ a_num_int.remainder(nil) }
    should_raise(TypeError){ a_num_int.remainder('test') }
    should_raise(TypeError){ a_num_int.remainder(true) }   
  end   
end

# Rounds num to the nearest integer
describe "Numeric#round" do  
  it " round (down) " do 
    0.round.should == 0.0
    a_possitive_number.round.should == 100
    a_negative_number.round.should == -100
  end  
  
  it " round (up) " do  
    a_possitive_float.round.should == 35
    a_negative_float.round.should == -35
  end  
  
  it " round twos complement " do
    a_num_twopb.round.should == 9223372036854775808 
    a_num_twonb.round.should == -9223372036854775808 
  end   
end

describe "Numeric#truncate" do   
  it " truncate integers " do 
    0.truncate.should == 0.0
    a_possitive_number.truncate.should == 100
    a_negative_number.truncate.should == -100
  end  
  
  it " truncate floats " do  
    a_possitive_float.truncate.should == 34
    a_negative_float.truncate.should == -34
  end  
  
  it " truncate two complement " do
    a_possitive_bignumber.truncate.should == 2147483648
    a_negative_bignumber.truncate.should == -2147483648
    a_num_twopb.truncate.should == 9223372036854775808 
    a_num_twonb.truncate.should == -9223372036854775808 
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
end

describe "Numeric#zero?" do   
  it "return the vaule if number is different to 0" do 
    0.zero?.should == true
    1.zero?.should == false
  end     
end

describe "Numeric#coerce" do   
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
    
  before(:each) do 
    @integer = 1
    @float   = 2.5    
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
end

warn ' Numeric have only a few specs'
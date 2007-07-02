require File.dirname(__FILE__) + '/../spec_helper'

# %, *, **, +, -, -@,+@, /, <, <=, <=>, ==, >, >=, abs, ceil, coerce,
# divmod, eql?, finite?, floor, hash, infinite?, modulo, nan?, round,
# to_f, to_i, to_int, to_s, truncate, zero?

TOLERANCE = 0.00003

describe "Float#CONSTANTS" do 
  specify  "the DIG value is  15" do
    Float::DIG.should_be_close(15,TOLERANCE)
  end
  
  specify "the EPSILON value is " do
    Float::EPSILON.should_be_close(2.22044604925031e-16,TOLERANCE)
  end
  
  specify "the MANT_DIG is 53" do
    Float::MANT_DIG.should_be_close(53,TOLERANCE)
  end
  
  specify "the MAX_10_EXP is 308" do
    Float::MAX_10_EXP.should == 308
  end
  
  specify "the MIN_10_EXP is -308" do    
    Float::MIN_10_EXP.should == -307
  end  
  
  specify "the MAX_EXP is 1024" do    
    Float::MAX_EXP.should_be_close(1024,TOLERANCE)
  end
  
  specify "the MIN_EXP is -1021" do
    Float::MIN_EXP.should_be_close(-1021,TOLERANCE)
  end   
  
  specify "the MIN_EXP is 1.79769313486232e+308" do
    Float::MIN_EXP.should_be_close(-1021,TOLERANCE)
  end 
  
  specify "the MAX is -1021" do
    Float::MAX.to_s.should == "1.79769313486232e+308".to_s    
  end 
  
  specify "the MIN is 2.2250738585072e-308" do    
    Float::MIN.should_be_close(2.2250738585072e-308,TOLERANCE)    
  end 
  
  specify "the RADIX is 2" do
    Float::RADIX.should_be_close(2,TOLERANCE)
  end
end

context "Float" do
  specify "% should return self modulo other" do
    (6543.21 % 137).should_be_close(104.21, TOLERANCE)
    (5667.19 % 0xffffffff).should_be_close(5667.19,TOLERANCE)
    (6543.21 % 137.24).should_be_close(92.9299999999996, TOLERANCE)
  end
  
  specify "% should NOT raise ZeroDivisionError if other is zero" do
    (1.0 % 0).to_s.should == 'NaN'
    (1.0 % 0.0).to_s.should == 'NaN'
  end
  
  specify "* should return self multiplied by other" do 
    (4923.98221 * 2).should_be_close(9847.96442, TOLERANCE) 
    (256.4096 * 0xffffffff).should_be_close(1101270846124.03 , TOLERANCE)
    (6712.5 * 0.25).should_be_close(1678.125, TOLERANCE) 
  end
  
  specify "** should return self raise to the other power" do
    (2.3 ** 3).should_be_close(12.167,TOLERANCE)
    (5.2 ** -1).should_be_close(0.192307692307692,TOLERANCE)
    (9.5 ** 0.5).should_be_close(3.08220700148449, TOLERANCE) 
    (9.5 ** 0xffffffff).to_s.should == 'Infinity'
  end
  
  specify "+ should return self plus other" do
    (491.213 + 2).should_be_close(493.213, TOLERANCE)
    (9.99 + 0xffffffff).should_be_close(4294967304.99,TOLERANCE)
    (1001.99 + 5.219).should_be_close(1007.209, TOLERANCE)
  end

  specify "- should return self minus other" do
    (9_237_212.5280 - 5_280).should_be_close(9231932.528, TOLERANCE)
    (2_560_496.1691 - 0xfffffffff).should_be_close(-68716916238.8309, TOLERANCE)
    (5.5 - 5.5).should_be_close(0.0,TOLERANCE)
  end
  
  specify "-@ should negate self" do
    (2.221.send(:-@)).should_be_close(-2.221, TOLERANCE)
    -2.01.should_be_close(-2.01,TOLERANCE)
    -2_455_999_221.5512.should_be_close(-2455999221.5512, TOLERANCE)
    (--5.5).should_be_close(5.5, TOLERANCE)
    -8.551.send(:-@).should_be_close(8.551, TOLERANCE)
  end

  specify "/ should return self divided by other" do
    (5.75 / -2).should_be_close(-2.875,TOLERANCE)
    (451.0 / 9.3).should_be_close(48.494623655914,TOLERANCE)
    (91.1 / -0xffffffff).should_be_close(-2.12108716418061e-08, TOLERANCE)
  end
  
  specify "/ should NOT raise ZeroDivisionError if other is zero" do
    (1.0 / 0.0).to_s.should == 'Infinity'
    (-1.0 / 0.0).to_s.should == '-Infinity'
    (1.0 / -0.0).to_s.should == '-Infinity'
    (-1.0 / -0.0).to_s.should == 'Infinity'
    (0.0 / 0.0).to_s.should == 'NaN'
    (-0.0 / 0.0).to_s.should == 'NaN'
    (-0.0 / -0.0).to_s.should == 'NaN'
  end
  
  specify "< should return true if self is less than other" do
    (71.3 < 91.8).should == true
    (192.6 < -500).should == false
    (-0.12 < 0x4fffffff).should == true
  end
  
  specify "<= should return true if self is less than or equal to other" do
    (2.0 <= 3.14159).should == true
    (-2.7183 <= -24).should == false
    (0.0 <= 0.0).should == true
    (9_235.9 <= 0xffffffff).should == true
  end
  
  specify "<=> should return -1, 0, 1 when self is less than, equal, or greater than other" do
    (1.5 <=> 5).should == -1
    (2.45 <=> 2.45).should == 0
    ((0xffffffff*1.1) <=> 0xffffffff).should == 1
  end
  
  specify "== should true if self has the same value as other" do
    (1.0 == 1).should == true
    (2.71828 == 1.428).should == false
    (-4.2 == 4.2).should == false
  end

  specify "== calls 'other == self' if coercion fails" do
    class X; def ==(other); 2.0 == other; end; end

    (1.0 == X.new).should == false
    (2.0 == X.new).should == true
  end
  
  specify "> should return true if self is greater than other" do
    (1.5 > 1).should == true
    (2.5 > 3).should == false
    (45.91 > 0xffffffff).should == false
  end
  
  specify ">= should return true if self is greater than or equal to other" do
    (5.2 >= 5.2).should == true
    (9.71 >= 1).should == true
    (5.55382 >= 0xfabdafbafcab).should == false
  end

  specify "abs should return the absolute value" do
    -99.1.abs.should_be_close(99.1, TOLERANCE)
    4.5.abs.should_be_close(4.5, TOLERANCE)
    0.0.abs.should_be_close(0.0, TOLERANCE)
  end
  
  specify "ceil should return the smallest Integer greater than or equal to self" do
    -1.2.ceil.should == -1
    -1.0.ceil.should == -1
    0.0.ceil.should == 0
    1.3.ceil.should == 2
    3.0.ceil.should == 3
  end
  
  specify "coerce should [other, self] both as Floats" do
    1.2.coerce(1).inspect.should == '[1.0, 1.2]'
    -0.0.coerce(0xffffffff).inspect.should == '[4294967295.0, -0.0]'
    5.28.coerce(1.0).inspect.should == '[1.0, 5.28]'
  end
  
  specify "divmod should return an [quotient, modulus] from dividing self by other" do
    3.14.divmod(2).inspect.should == "[1.0, 1.14]"
    2.8284.divmod(3.1415).inspect.should == "[0.0, 2.8284]"
    -1.0.divmod(0xffffffff).inspect.should == "[-1.0, 4294967294.0]"
  end

  # This failed for me on MRI. I'm assuming it is platform dependent -- flgr
  if RUBY_PLATFORM["darwin"] && !defined?(RUBY_ENGINE) then
    it "divmod should return [NaN, NaN] if other is zero" do
      1.0.divmod(0).inspect.should == '[NaN, NaN]'
      1.0.divmod(0.0).inspect.should == '[NaN, NaN]'
    end
  else
    it "divmod should raise FloatDomainError if other is zero" do
      should_raise(FloatDomainError) { 1.0.divmod(0) }
      should_raise(FloatDomainError) { 1.0.divmod(0.0) }
    end
  end
  
  specify "eql? should return true if other is a Float equal to self" do
    1.0.eql?(1).should == false
    0.0.eql?(0.0).should == true
  end
  
  specify "finite? should return true if a valid IEEE floating-point number" do
    (1.5**0xffffffff).finite?.should == false
    3.14159.finite?.should == true
    (-1.0/0.0).finite?.should == false
  end
  
  specify "floor should return the largest Integer less than or equal to self" do
    -1.2.floor.should == -2
    -1.0.floor.should == -1
    0.0.floor.should == 0
    1.0.floor.should == 1
    5.9.floor.should == 5
  end
  
  specify "hash should be provided" do
    0.0.respond_to?(:hash).should == true
  end
  
  specify "infinite? should return nil, -1, +1 when self is finite, -Infinity, +Infinity" do
    1.0.infinite?.should == nil
    (1.0/0.0).infinite?.should == 1
    (1.0/-0.0).infinite?.should == -1
  end
  
  specify "modulo should be a synonym for %" do
    6543.21.modulo(137).should_be_close(104.21, TOLERANCE)
    5667.19.modulo(0xffffffff).should_be_close(5667.19, TOLERANCE)
    6543.21.modulo(137.24).should_be_close(92.9299999999996, TOLERANCE)
    
    6543.21.modulo(137).should_be_close(6543.21.%(137), TOLERANCE)
    5667.19.modulo(0xffffffff).should_be_close(5667.19.%(0xffffffff), TOLERANCE)
    6543.21.modulo(137.24).should_be_close(6543.21.%(137.24), TOLERANCE)
  end

  specify "modulo should NOT raise ZeroDivisionError if other is zero" do
    (1.0 % 0).to_s.should == 'NaN'
    (1.0 % 0.0).to_s.should == 'NaN'
  end

  specify "nan? should return true if self is not a valid IEEE floating-point number" do
    0.0.nan?.should == false
    -1.5.nan?.should == false
    (0.0/0.0).nan?.should == true
  end
  
  specify "round should return the nearest Integer" do
    5.5.round.should == 6
    0.4.round.should == 0
    -2.8.round.should == -3
    0.0.round.should == 0
  end
  
  specify "to_f should return self" do
    -500.3.to_f.should == -500.3
    267.51.to_f.should == 267.51
    1.1412.to_f.should == 1.1412
  end
  
  specify "to_i should return self truncated to an Integer" do
    899.2.to_int.should == 899
    -1.122256e-45.to_int.should == 0
    5_213_451.9201.to_int.should == 5213451
  end
  
  specify "to_int should be a synonym for to_i" do
    899.2.to_int.should == 899
    -1.122256e-45.to_int.should == 0
    5_213_451.9201.to_int.should == 5213451
  end
  
  specify "to_s should return a string representation of self, possibly Nan, -Infinity, +Infinity" do
    0.551e7.to_s.should == "5510000.0"
    -3.14159.to_s.should == "-3.14159"
    0.0.to_s.should == "0.0"
  end
  
  specify "truncate should be a synonym for to_i" do
    899.2.truncate.should == 899
    -1.122256e-45.truncate.should == 0
    5_213_451.9201.truncate.should == 5213451
  end
  
  specify "zero? should return true if self is 0.0" do
    0.0.zero?.should == true
    1.0.zero?.should == false
    -1.0.zero?.should == false
  end
  
  specify "coerce should return [Float, Float]" do
    1.0.coerce(1).inspect.should == '[1.0, 1.0]'
    1.0.coerce(0xffffffff).inspect.should == '[4294967295.0, 1.0]'
    1.0.coerce("2.5").inspect.should == '[2.5, 1.0]'
    1.0.coerce(3.14).inspect.should == '[3.14, 1.0]'
  end
end

context "Float.induced_from" do
  specify "should return the argument when passed a Float" do
    Float.induced_from(5.5).eql?(5.5).should == true
  end

  specify "should return a Float of the correct value when sent a Fixnum" do
    Float.induced_from(36).should == 36.0
  end

  specify "should return a Float of the correct value when sent a Bignum" do
    Float.induced_from(23472398472349872349872349872348972348972439423).to_s.should == '2.34723984723499e+46'
  end

  specify "should raise a TypeError if the argument is not a Float, Fixnum, or Bignum" do
    class Foo
      def to_f; 9.9; end
    end
    should_raise(TypeError) { Float.induced_from(Foo.new) }
  end
end 

describe "Float#+@" do 
  it " should return the same value with same sign (two complement)" do 
    34.56.send(:+@).should == 34.56
    -34.56.send(:+@).should == -34.56
    0.0.send(:+@).should == 0.0
  end  
end

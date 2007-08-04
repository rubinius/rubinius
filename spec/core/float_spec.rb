require File.dirname(__FILE__) + '/../spec_helper'

# %, *, **, +, -, -@,+@, /, <, <=, <=>, ==, >, >=, abs, ceil, coerce,
# divmod, eql?, finite?, floor, hash, infinite?, modulo, nan?, round,
# to_f, to_i, to_int, to_s, truncate, zero?

TOLERANCE = 0.0003

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
  
  specify "the MAX is 1.79769313486232e+308" do
    Float::MAX.to_s.should == "1.79769313486232e+308".to_s    
  end 
  
  specify "the MIN is 2.2250738585072e-308" do    
    Float::MIN.should_be_close(2.2250738585072e-308,TOLERANCE)    
  end 
  
  specify "the RADIX is 2" do
    Float::RADIX.should_be_close(2,TOLERANCE)
  end
end

describe "Float.induced_from" do
  it "returns the argument when passed a Float" do
    Float.induced_from(5.5).eql?(5.5).should == true
  end

  it "returns a Float of the correct value when sent a Fixnum" do
    Float.induced_from(36).should == 36.0
  end

  it "returns a Float of the correct value when sent a Bignum" do
    Float.induced_from(23472398472349872349872349872348972348972439423).to_s.should == '2.34723984723499e+46'
  end

  it "raises a TypeError if the argument is not a Float, Fixnum, or Bignum" do
    class Foo
      def to_f; 9.9; end
    end
    should_raise(TypeError) { Float.induced_from(Foo.new) }
  end
end 

float_modulo = shared "Float#% | Float#modulo" do |cmd|
  describe "Float##{cmd}" do
    it "returns self modulo other" do
      6543.21.send(cmd, 137).should_be_close(104.21, TOLERANCE)
      5667.19.send(cmd, 0xffffffff).should_be_close(5667.19, TOLERANCE)
      6543.21.send(cmd, 137.24).should_be_close(92.9299999999996, TOLERANCE)

      6543.21.send(cmd, 137).should_be_close(6543.21.%(137), TOLERANCE)
      5667.19.send(cmd, 0xffffffff).should_be_close(5667.19.%(0xffffffff), TOLERANCE)
      6543.21.send(cmd, 137.24).should_be_close(6543.21.%(137.24), TOLERANCE)
    end

    it "does NOT raise ZeroDivisionError if other is zero" do
      1.0.send(cmd, 0).to_s.should == 'NaN'
      1.0.send(cmd, 0.0).to_s.should == 'NaN'
    end
  end
end

describe "Float#%" do
  it_behaves_like(float_modulo, :%)
end

describe "Float#*" do
  it "returns self multiplied by other" do 
    (4923.98221 * 2).should_be_close(9847.96442, TOLERANCE) 
    (6712.5 * 0.25).should_be_close(1678.125, TOLERANCE) 
    (256.4096 * 0xffffffff).to_s.should == 1101270846124.03.to_s
  end
end

describe "Float#**" do
  it "returns self raise to the other power" do
    (2.3 ** 3).should_be_close(12.167,TOLERANCE)
    (5.2 ** -1).should_be_close(0.192307692307692,TOLERANCE)
    (9.5 ** 0.5).should_be_close(3.08220700148449, TOLERANCE) 
    (9.5 ** 0xffffffff).to_s.should == 'Infinity'
  end
end

describe "Float#+" do
  it "returns self plus other" do
    (491.213 + 2).should_be_close(493.213, TOLERANCE)
    (9.99 + 0xffffffff).should_be_close(4294967304.99,TOLERANCE)
    (1001.99 + 5.219).should_be_close(1007.209, TOLERANCE)
  end
end

describe "Float#+@" do 
  it "returns the same value with same sign (twos complement)" do 
    34.56.send(:+@).should == 34.56
    -34.56.send(:+@).should == -34.56
    0.0.send(:+@).should == 0.0
  end  
end

describe "Float#-" do
  it "returns self minus other" do
    (9_237_212.5280 - 5_280).should_be_close(9231932.528, TOLERANCE)
    (2_560_496.1691 - 0xfffffffff).should_be_close(-68716916238.8309, TOLERANCE)
    (5.5 - 5.5).should_be_close(0.0,TOLERANCE)
  end
end

describe "Float#-@" do
  it "negates self" do
    (2.221.send(:-@)).should_be_close(-2.221, TOLERANCE)
    -2.01.should_be_close(-2.01,TOLERANCE)
    -2_455_999_221.5512.should_be_close(-2455999221.5512, TOLERANCE)
    (--5.5).should_be_close(5.5, TOLERANCE)
    -8.551.send(:-@).should_be_close(8.551, TOLERANCE)
  end
end

describe "Float#/" do
  it "returns self divided by other" do
    (5.75 / -2).should_be_close(-2.875,TOLERANCE)
    (451.0 / 9.3).should_be_close(48.494623655914,TOLERANCE)
    (91.1 / -0xffffffff).should_be_close(-2.12108716418061e-08, TOLERANCE)
  end
  
  it "does NOT raise ZeroDivisionError if other is zero" do
    (1.0 / 0.0).to_s.should == 'Infinity'
    (-1.0 / 0.0).to_s.should == '-Infinity'
    (1.0 / -0.0).to_s.should == '-Infinity'
    (-1.0 / -0.0).to_s.should == 'Infinity'
    (0.0 / 0.0).to_s.should == 'NaN'
    (-0.0 / 0.0).to_s.should == 'NaN'
    (-0.0 / -0.0).to_s.should == 'NaN'
  end
end

describe "Float#<" do
  it "returns true if self is less than other" do
    (71.3 < 91.8).should == true
    (192.6 < -500).should == false
    (-0.12 < 0x4fffffff).should == true
  end
end

describe "Float#<=" do
  it "returns true if self is less than or equal to other" do
    (2.0 <= 3.14159).should == true
    (-2.7183 <= -24).should == false
    (0.0 <= 0.0).should == true
    (9_235.9 <= 0xffffffff).should == true
  end
end

describe "Float#<=>" do
  it "returns -1, 0, 1 when self is less than, equal, or greater than other" do
    (1.5 <=> 5).should == -1
    (2.45 <=> 2.45).should == 0
    ((0xffffffff*1.1) <=> 0xffffffff).should == 1
  end
end

describe "Float#==" do
  it "returns true if self has the same value as other" do
    (1.0 == 1).should == true
    (2.71828 == 1.428).should == false
    (-4.2 == 4.2).should == false
  end

  it "calls 'other == self' if coercion fails" do
    class X; def ==(other); 2.0 == other; end; end

    (1.0 == X.new).should == false
    (2.0 == X.new).should == true
  end
end

describe "Float#>" do
  it "returns true if self is greater than other" do
    (1.5 > 1).should == true
    (2.5 > 3).should == false
    (45.91 > 0xffffffff).should == false
  end
end

describe "Float#>=" do
  it "returns true if self is greater than or equal to other" do
    (5.2 >= 5.2).should == true
    (9.71 >= 1).should == true
    (5.55382 >= 0xfabdafbafcab).should == false
  end
end

describe "Float#abs" do
  it "returns the absolute value" do
    -99.1.abs.should_be_close(99.1, TOLERANCE)
    4.5.abs.should_be_close(4.5, TOLERANCE)
    0.0.abs.should_be_close(0.0, TOLERANCE)
  end
  
end

describe "Float#ceil" do
  it "returns the smallest Integer greater than or equal to self" do
    -1.2.ceil.should == -1
    -1.0.ceil.should == -1
    0.0.ceil.should == 0
    1.3.ceil.should == 2
    3.0.ceil.should == 3
  end
end

describe "Float#coerce" do
  it "returns [other, self] both as Floats" do
    1.2.coerce(1).inspect.should == '[1.0, 1.2]'
    -0.0.coerce(0xffffffff).inspect.should == '[4294967295.0, -0.0]'
    5.28.coerce(1.0).inspect.should == '[1.0, 5.28]'
    1.0.coerce(1).inspect.should == '[1.0, 1.0]'
    1.0.coerce(0xffffffff).inspect.should == '[4294967295.0, 1.0]'
    1.0.coerce("2.5").inspect.should == '[2.5, 1.0]'
    1.0.coerce(3.14).inspect.should == '[3.14, 1.0]'
  end
end

describe "Float#divmod" do
  it "returns an [quotient, modulus] from dividing self by other" do
    3.14.divmod(2).inspect.should == "[1, 1.14]"
    2.8284.divmod(3.1415).inspect.should == "[0, 2.8284]"
    -1.0.divmod(0xffffffff).inspect.should == "[-1, 4294967294.0]"
  end

  # This failed for me on MRI. I'm assuming it is platform dependent -- flgr
  if RUBY_PLATFORM["darwin"] && !defined?(RUBY_ENGINE) then
    it "returns [NaN, NaN] if other is zero" do
      1.0.divmod(0).inspect.should == '[NaN, NaN]'
      1.0.divmod(0.0).inspect.should == '[NaN, NaN]'
    end
  else
    it "raises FloatDomainError if other is zero" do
      should_raise(FloatDomainError) { 1.0.divmod(0) }
      should_raise(FloatDomainError) { 1.0.divmod(0.0) }
    end
  end
end

describe "Float#eql?" do
  it "returns true if other is a Float equal to self" do
    1.0.eql?(1).should == false
    0.0.eql?(0.0).should == true
  end
end

describe "Float#finite?" do
  it "returns true if a valid IEEE floating-point number" do
    (1.5**0xffffffff).finite?.should == false
    3.14159.finite?.should == true
    (-1.0/0.0).finite?.should == false
  end
end

describe "Float#floor" do
  it "returns the largest Integer less than or equal to self" do
    -1.2.floor.should == -2
    -1.0.floor.should == -1
    0.0.floor.should == 0
    1.0.floor.should == 1
    5.9.floor.should == 5
  end
end

describe "Float#hash" do
  it "is provided" do
    0.0.respond_to?(:hash).should == true
  end
end

describe "Float#infinite?" do
  it "returns nil, -1, +1 when self is finite, -Infinity, +Infinity" do
    1.0.infinite?.should == nil
    (1.0/0.0).infinite?.should == 1
    (1.0/-0.0).infinite?.should == -1
  end
end

describe "Float#modulo" do
  it_behaves_like(float_modulo, :modulo)
end

describe "Float#nan?" do
  it "returns true if self is not a valid IEEE floating-point number" do
    0.0.nan?.should == false
    -1.5.nan?.should == false
    (0.0/0.0).nan?.should == true
  end
end

describe "Float#round" do
  it "returns the nearest Integer" do
    5.5.round.should == 6
    0.4.round.should == 0
    -2.8.round.should == -3
    0.0.round.should == 0
  end
end

describe "Float#to_f" do
  it "returns self" do
    -500.3.to_f.should == -500.3
    267.51.to_f.should == 267.51
    1.1412.to_f.should == 1.1412
  end
end

float_to_i = shared "Float#to_i | Float#to_int | Float#truncate" do |cmd|
  describe "Float##{cmd}" do
    it "returns self truncated to an Integer" do
      899.2.send(cmd).should == 899
      -1.122256e-45.send(cmd).should == 0
      5_213_451.9201.send(cmd).should == 5213451
    end
  end
end

describe "Float#to_i" do
  it_behaves_like(float_to_i, :to_i)
end

describe "Float#" do
  it_behaves_like(float_to_i, :to_int)
end

describe "Float#to_s" do
  it "returns a string representation of self, possibly Nan, -Infinity, +Infinity" do
    0.551e7.to_s.should == "5510000.0"
    -3.14159.to_s.should == "-3.14159"
    0.0.to_s.should == "0.0"
  end  
end

describe "Float#" do
  it_behaves_like(float_to_i, :truncate)
end

describe "Float#zero?" do
  it "returns true if self is 0.0" do
    0.0.zero?.should == true
    1.0.zero?.should == false
    -1.0.zero?.should == false
  end
end

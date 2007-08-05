require File.dirname(__FILE__) + '/../spec_helper'

# %, &, *, **, **, +, -, -, /, <<, <=>, ==, >>, [], ^, abs, coerce,
# div, divmod, eql?, hash, modulo, power!, quo, quo, rdiv, remainder,
# rpower, size, to_f, to_s, |, ~

# the smallest bignum in MRI is a different value
# than the smallest bignum in Rubinius, however,
# rubinius(smallest_bignum) <= MRI(smallest_bignum)
# so these specs are written such that they should
# pass on either target.

module BignumHelper
  def self.sbm(plus=0)
    0x40000000 + plus
  end
end

bignum_modulo = shared "Bignum#modulo" do |cmd|
  describe "Bignum##{cmd}" do
    it "returns self modulo other" do
      a = BignumHelper.sbm(2_222)
      a.send(cmd, 5).should == 1
      a.send(cmd, 2.22).inspect.should == '0.419999905491539'
      a.send(cmd, BignumHelper.sbm).should == 2222
    end

    it "raises ZeroDivisionError if other is zero and not a Float" do
      should_raise(ZeroDivisionError) { BignumHelper.sbm.send(cmd, 0) }
    end

    it "% should NOT raise ZeroDivisionError if other is zero and is a Float" do
      a = BignumHelper.sbm(5_221)
      b = BignumHelper.sbm(25)
      a.send(cmd, 0.0).to_s.should == 'NaN'
      b.send(cmd, -0.0).to_s.should == 'NaN'
    end
  end
end

describe "Bignum#%" do
  it_behaves_like(bignum_modulo, :%)
end

describe "Bignum#modulo" do
  it_behaves_like(bignum_modulo, :modulo)
end

describe "Bignum#&" do
  it "returns self bitwise AND other" do
    a = BignumHelper.sbm(5)
    (a & 3.4).should == 1
    (a & 52).should == 4
    (a & BignumHelper.sbm(9921)).should == 1073741825
  end
end

describe "Bignum#*" do
  it "returns self multiplied by other" do
    a = BignumHelper.sbm(772)
    (a * 98.6).should_be_close(1.0587101996559999e+11, TOLERANCE)
    (a * 10).to_s.should == '10737425960'
    (a * (a - 40)).to_s.should == '1152923119515115376'
  end
end

describe "Bignum#**" do
  it "returns self raised to other power" do
    a = BignumHelper.sbm(47)
    (a ** 5.2).to_s.should == "9.13438731244363e+46"
    (a ** 4).should == 1329228228517658539377366716859970881
  end
end

describe "Bignum#+" do
  it "returns self plus other" do
    a = BignumHelper.sbm(76)
    (a + 4).should == 1073741904
    (a + 4.2).should == 1073741904.2
    (a + BignumHelper.sbm(3)).should == 2147483727
  end
end

describe "Bignum#-" do
  it "returns self minus other" do
    a = BignumHelper.sbm(314)
    (a - 9).should == 1073742129
    (a - 12.57).should == 1073742125.43
    (a - BignumHelper.sbm(42)).should == 272
  end
end

describe "Bignum#-@" do
  it "negates self" do
    BignumHelper.sbm.send(:-@).should == (-1073741824)
    BignumHelper.sbm(921).send(:-@).should == (-1073742745)
  end
end

bignum_divide = shared "Bignum#/" do |cmd|
  describe "Bignum##{cmd}" do
    it "returns self divided by other" do
      a = BignumHelper.sbm(88)
      a.send(cmd, 4).should_be_close(268435478, TOLERANCE)
      a.send(cmd, 16.2).should_be_close(66280364.9382716, TOLERANCE)
      a.send(cmd, BignumHelper.sbm(2)).to_s.should == '1'
    end

    it "raises ZeroDivisionError if other is zero and not a Float" do
      should_raise(ZeroDivisionError) { BignumHelper.sbm.send cmd, 0 }
    end

    it "does NOT raise ZeroDivisionError if other is zero and is a Float" do
      a = BignumHelper.sbm(5_221)
      b = BignumHelper.sbm(25)
      a.send(cmd, 0.0).to_s.should == 'Infinity'
      b.send(cmd, -0.0).to_s.should == '-Infinity'
    end
  end
end

describe "Bignum#/" do
  it_behaves_like(bignum_divide, :/)
end

describe "Bignum#div" do
  it_behaves_like(bignum_divide, :div)
end

describe "Bignum#<" do
  it "returns true if self is less than other" do
    a = BignumHelper.sbm(32)
    (a < 2).should == false
    (a < 456_912.410).should == false
    (a < BignumHelper.sbm(42)).should == true
  end
end

describe "Bignum#<=" do
  it "returns true if self is less than or equal to other" do
    a = BignumHelper.sbm(39)
    (a <= a).should == true
    (a <= 4).should == false
    (a <= 45.8).should == false
  end
end

describe "Bignum#<=>" do
  it "returns -1, 0, 1 when self is less than, equal, or greater than other" do
    a = BignumHelper.sbm(96)
    (a <=> BignumHelper.sbm(196)).should == -1
    (a <=> a).should == 0
    (a <=> 4.5).should == 1
  end
end

describe "Bignum#==" do
  it "should true if self has the same value as other" do
    a = BignumHelper.sbm(67)
    (a == a).should == true
    (a == 5.4).should == false
    (a == 121).should == false
  end

  it "calls 'other == self' if coercion fails" do
    class X; def ==(other); BignumHelper.sbm(123) == other; end; end

    (BignumHelper.sbm(120) == X.new).should == false
    (BignumHelper.sbm(123) == X.new).should == true
  end
end

describe "Bignum#>" do
  it "returns true if self is greater than other" do
    a = BignumHelper.sbm(732)
    (a > (a + 500)).should == false
    (a > 14.6).should == true
    (a > (BignumHelper.sbm - 1)).should == true
  end
end

describe "Bignum#>=" do
  it "returns true if self is greater than or equal to other" do
    a = BignumHelper.sbm(14)
    (a >= a).should == true
    (a >= (a + 2)).should == false
    (a >= 5664.2).should == true
    (a >= 4).should == true
  end
end

describe "Bignum#abs" do
  it "returns the absolute value" do
    BignumHelper.sbm(39).abs.should == 1073741863
    (-BignumHelper.sbm(18)).abs.should == 1073741842
  end
end

describe "Bignum#<<" do
  it "returns self shifted left other bits" do
    a = BignumHelper.sbm(9)
    (a << 4).should == 17179869328
    (a << 1.5).should == 2147483666
    (a << 9).should == 549755818496
  end
end

describe "Bignum#>>" do
  it "returns self shifted right other bits" do
    a = BignumHelper.sbm(90812)
    (a >> 3.45).should == 134229079
    (a >> 2).should == 268458159
    (a >> 21).should == 512
  end
  
  # This test was added as the result of ruby-core:9020.
  it "return the right shift alignment" do
     ((1 - 2**31) >> 31).should == -1
     ((1 - 2**32) >> 32).should == -1
     ((1 - 2**63) >> 63).should == -1 
     ((1 - 2**64) >> 64).should == -1 
  end
end

describe "Bignum#[]" do
  it "returns the nth bit in the binary representation of self" do
    a = BignumHelper.sbm(4996)
    a[2].should == 1
    a[9.2].should == 1
    a[21].should == 0
  end
end

describe "Bignum#^" do
  it "returns self bitwise EXCLUSIVE OR other" do
    a = BignumHelper.sbm(18)
    (a ^ 2).should == 1073741840
    (a ^ a).should == 0
    (a ^ 14.5).should == 1073741852
  end
end

describe "Bignum#coerce" do
  it "returns [other, self] both as Bignum if other is an Integer" do
    a = BignumHelper.sbm
    a.coerce(2).should == [2, 1073741824]
    a.coerce(BignumHelper.sbm(701)).should == [1073742525, 1073741824]
  end
  
  it "returns [Bignum, Bignum] if other is a Bignum" do
    a = 0xffffffff.coerce(0xffffffff)
    a.should == [4294967295, 4294967295]
    a.collect { |i| i.class }.should == [Bignum, Bignum]
  end
end

describe "Bignum#divmod" do
  it "returns an [quotient, modulus] from dividing self by other" do
    a = BignumHelper.sbm(55)
    a.divmod(5).inspect.should == '[214748375, 4]'
    a.divmod(15.2).inspect.should == '[70640913, 1.40000005019339]'
    a.divmod(a + 9).inspect.should == '[0, 1073741879]'
  end
  
  it "raises ZeroDivisionError if other is zero and not a Float" do
    should_raise(ZeroDivisionError) { BignumHelper.sbm(2).divmod(0) }
  end
  
  # This failed for me on MRI. I'm assuming it is platform dependent -- flgr
  if RUBY_PLATFORM["darwin"] && !defined?(RUBY_ENGINE) then
    it "returns [NaN, NaN] if other is zero and is a Float" do
      BignumHelper.sbm(9).divmod(0.0).inspect.should == '[NaN, NaN]'
    end
  else
    it "raises FloatDomainError if other is zero and is a Float" do
      should_raise(FloatDomainError) { BignumHelper.sbm(9).divmod(0.0) }
    end
  end
end

describe "Bignum#eql?" do
  it "returns true if other is a Bignum with the same value" do
    a = BignumHelper.sbm(13)
    a.eql?(BignumHelper.sbm(13)).should == true
    a.eql?(2).should == false
    a.eql?(3.14).should == false
  end
end

describe "Bignum#hash" do
  it "is provided" do
    BignumHelper.sbm.respond_to?(:hash).should == true
  end
end

describe "Bignum#quo" do
  it "returns the floating-point result of self divided by other" do
    a = BignumHelper.sbm(3)
    a.quo(2.5).should_be_close(429496730.8, TOLERANCE)
    a.quo(13).should_be_close(82595525.1538462, TOLERANCE)
    a.quo(BignumHelper.sbm).should_be_close(1.00000000279397, TOLERANCE)
  end

  it "does NOT raise an exception when other is zero" do
    # a.quo(0) should also not raise (i.e works in irb and from a file),
    # but fails here.
    a = BignumHelper.sbm(91)
    b = BignumHelper.sbm(28)
    a.quo(0.0).to_s.should == "Infinity"
    b.quo(-0.0).to_s.should == "-Infinity"
  end
end

describe "Bignum#remainder" do
  it "returns the remainder of dividing self by other" do
    a = BignumHelper.sbm(79)
    a.remainder(2).should == 1
    a.remainder(97.345).should_be_close(75.16000001254, TOLERANCE)
    a.remainder(BignumHelper.sbm).should == 79
  end
  
  it "raises ZeroDivisionError if other is zero and not a Float" do
    should_raise(ZeroDivisionError) { BignumHelper.sbm(66).remainder(0) }
  end
  
  it "does NOT raise ZeroDivisionError if other is zero and is a Float" do
    a = BignumHelper.sbm(7)
    b = BignumHelper.sbm(32)
    a.remainder(0.0).to_s.should == 'NaN'
    b.remainder(-0.0).to_s.should == 'NaN'
  end
end

describe "Bignum#size" do
  it "is provided" do
    BignumHelper.sbm.respond_to?(:size).should == true
  end

  it "returns number of bytes in self" do
    (256**7).size.should == 8
    (256**8).size.should == 12
    (256**9).size.should == 12
    (256**10).size.should == 12
    (256**10-1).size.should == 12
    (256**11).size.should == 12
    (256**12).size.should == 16
    (256**20-1).size.should == 20
    (256**40-1).size.should == 40
  end
end

describe "Bignum#to_f" do
  it "returns self converted to Float" do
    BignumHelper.sbm(2).to_f.should == 1073741826.0
    (-BignumHelper.sbm(99)).to_f.should == -1073741923.0
    BignumHelper.sbm(14).to_f.should == 1073741838.0
  end
end

describe "Bignum#|" do
  it "returns self bitwise OR other" do
    a = BignumHelper.sbm(11)
    (a | 2).should == 1073741835
    (a | 9.9).should == 1073741835
    (a | BignumHelper.sbm).should == 1073741835
  end
end

describe "Bignum#~" do
  it "returns self bitwise inverted" do
    (~BignumHelper.sbm(48)).should == -1073741873
    (~(-BignumHelper.sbm(21))).should == 1073741844
    (~BignumHelper.sbm(1)).should == -1073741826
  end
end

describe "Bignum#&" do 
  # 18446744073709551616 == 2**64
  # 36893488147419103232 == 2**65
  it "returns self bitwise AND other" do
      (18446744073709551616).&(1).should_be_close(0,0.00001)
      (18446744073709551616).&(-1).should == 18446744073709551616
      (36893488147419103232).&(-1).should == 36893488147419103232
      (18446744073709551616).&(18446744073709551616).should == 18446744073709551616
  end
end

describe "Bignum#to_s" do  
  it "returns a string representation of self" do
    BignumHelper.sbm(9).to_s.should == "1073741833"
    BignumHelper.sbm.to_s.should == "1073741824"
    (-BignumHelper.sbm(675)).to_s.should == "-1073742499"
  end
  
  it "returns a string with the representation of self in base x"  do 
    a = 18446744073709551616 #2**64
    a.to_s(2).should == "10000000000000000000000000000000000000000000000000000000000000000" 
    a.to_s(8).should == "2000000000000000000000"
    a.to_s(16).should == "10000000000000000"
    a.to_s(32).should == "g000000000000" 
  end
  
  it "raises an ArgumentError exception if argument is 0" do
    should_raise(ArgumentError){ 18446744073709551616.to_s(0) }
  end
  
  it "raises an ArgumentError exception if argument is bigger than 36" do 
    should_raise(ArgumentError){ 18446744073709551616.to_s(37) } # Max is 36
  end
end

require File.dirname(__FILE__) + '/spec_helper'

load_extension("bignum")

def ensure_bignum(n)
  0xffff_ffff_ffff_ffff_ffff.coerce(n)[0]
end

describe "CApiBignumSpecs" do
  before :each do
    @s = CApiBignumSpecs.new
    @max_long = ensure_bignum(2**(0.size * 8 - 1) - 1)
    @min_long = ensure_bignum(-@max_long - 1)
    @max_ulong = ensure_bignum(2**(0.size * 8) - 1)
  end

  describe "rb_big2long" do
    it "converts a Bignum" do
      @s.rb_big2long(@max_long).should == @max_long
      @s.rb_big2long(@min_long).should == @min_long
      @s.rb_big2long(ensure_bignum(0)).should == 0
    end

    it "raises RangeError if passed Bignum overflow long" do
      lambda { @s.rb_big2long(ensure_bignum(@max_long + 1)) }.should raise_error(RangeError)
      lambda { @s.rb_big2long(ensure_bignum(@min_long - 1)) }.should raise_error(RangeError)
    end
  end

  describe "rb_big2ulong" do
    it "converts a Bignum" do
      @s.rb_big2ulong(@max_ulong).should == @max_ulong
      @s.rb_big2long(ensure_bignum(0)).should == 0
    end

    it "wraps around if passed a negative bignum" do
      @s.rb_big2ulong(ensure_bignum(-1)).should == @max_ulong
      @s.rb_big2ulong(ensure_bignum(@min_long + 1)).should == -(@min_long - 1)
    end

    it "raises RangeError if passed Bignum overflow long" do
      lambda { @s.rb_big2ulong(ensure_bignum(@max_ulong + 1)) }.should raise_error(RangeError)
      lambda { @s.rb_big2ulong(ensure_bignum(@min_long)) }.should raise_error(RangeError)
    end
  end

  describe "rb_big2dbl" do
    it "converts a Bignum to a double value" do
      @s.rb_big2dbl(ensure_bignum(1)).eql?(1.0).should == true
      @s.rb_big2dbl(ensure_bignum(Float::MAX.to_i)).eql?(Float::MAX).should == true
    end

    it "returns Infinity if the number is too big for a double" do
      huge_bignum = ensure_bignum(Float::MAX.to_i * 2)
      infinity = 1.0 / 0.0
      @s.rb_big2dbl(huge_bignum).should == infinity
      @s.rb_big2dbl(-huge_bignum).should == infinity
    end

    it "prints a warning to $stderr if Bignum is too big for a double" do
      verbose, $VERBOSE = $VERBOSE, true
      stderr, $stderr = $stderr, IOStub.new

      @s.rb_big2dbl(ensure_bignum(Float::MAX.to_i * 2))
      $stderr.should =~ /Bignum out of Float range/

      $stderr = stderr
      $VERBOSE = verbose
    end
  end
end

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
end

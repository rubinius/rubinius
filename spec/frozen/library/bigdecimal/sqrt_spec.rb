require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require 'bigdecimal'

describe "BigDecimal#sqrt" do
  before(:each) do
    @one = BigDecimal("1")
    @zero = BigDecimal("0")
    @zero_pos = BigDecimal("+0")
    @zero_neg = BigDecimal("-0")
    @two = BigDecimal("2")
    @three = BigDecimal("3")
    @nan = BigDecimal("NaN")
    @infinity = BigDecimal("Infinity")
    @infinity_minus = BigDecimal("-Infinity")
    @one_minus = BigDecimal("-1")
    @frac_1 = BigDecimal("1E-99999")
    @frac_2 = BigDecimal("0.9E-99999")
  end

  it "returns sqrt of self with at least the given precision" do
    string = "1.41421356237309504880168872420969807856967187537694807317667973799073247846210703885038753432764157"
    (1..99).each { |idx|
      @two.sqrt(idx).should be_close(BigDecimal(string), BigDecimal("1E-#{idx-1}"))
    }
    sqrt_3 = "1.732050807568877293527446341505872366942805253810380628055806979451933016908800037081146186757248575"
    (1..99).each { |idx|
      @three.sqrt(idx).should be_close(BigDecimal(sqrt_3), BigDecimal("1E-#{idx-1}"))
    }
    BigDecimal('121').sqrt(5).should be_close(11, 0.00001)
    @frac_2.sqrt(1).to_s.should == "0.3E-49999"
  end

  it "requires a single fixnum argument" do
    lambda {
      @one.sqrt
    }.should raise_error(ArgumentError)
    lambda {
      @one.sqrt(-1)
    }.should raise_error(ArgumentError)
    lambda {
      @one.sqrt(1, 1)
    }.should raise_error(ArgumentError)
    lambda {
      @one.sqrt(nil)
    }.should raise_error(TypeError)
    lambda {
      @one.sqrt("stuff")
    }.should raise_error(TypeError)
    lambda {
      @one.sqrt(Object.new)
    }.should raise_error(TypeError)
    @one.sqrt(1).should == 1
    @one.sqrt(0).should == 1
  end

  it "raises FloatDomainError on negative values" do
    lambda {
      BigDecimal('-1').sqrt(10)
    }.should raise_error(FloatDomainError)
  end

  it "properly handles special values" do
    @infinity.sqrt(1).should == @infinity
    lambda {
      @infinity_minus.sqrt(1)
    }.should raise_error(FloatDomainError)
    lambda {
      @nan.sqrt(1)
    }.should raise_error(FloatDomainError)
    @zero.sqrt(1).should == 0
    @zero_pos.sqrt(1).should == 0
    @zero_neg.sqrt(1).should == 0
  end

end
